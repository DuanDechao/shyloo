#include "slwebsocket_packet_filter.h"
#include "slwebsocket_protocol.h"
#include "slmemorystream.h"
#define MAX_TCP_PACKET_SIZE 1460
namespace sl { 
namespace network{
//-------------------------------------------------------------------------------------
WebSocketPacketFilter::WebSocketPacketFilter():
	pFragmentDatasRemain_(0),
	fragmentDatasFlag_(FRAGMENT_MESSAGE_HREAD),
	msg_opcode_(0),
	msg_fin_(0),
	msg_masked_(0),
	msg_mask_(0),
	msg_length_field_(0),
	msg_payload_length_(0),
	msg_frameType_(websocket::WebSocketProtocol::ERROR_FRAME)
{

	_recvFilterBuf = (sl::SLRingBuffer*)SLMALLOC(62235);
	_recvFilterBuf->init(62235, true);
	_sendFilterBuf = (sl::SLRingBuffer*)SLMALLOC(62235);
	_sendFilterBuf->init(62235, true);
}




//-------------------------------------------------------------------------------------
WebSocketPacketFilter::~WebSocketPacketFilter(){
	reset();
}

//-------------------------------------------------------------------------------------
void WebSocketPacketFilter::reset(){
	msg_opcode_ = 0;
	msg_fin_ = 0;
	msg_masked_ = 0;
	msg_mask_ = 0;
	msg_length_field_ = 0;
	msg_payload_length_ = 0;
	pFragmentDatasRemain_ = 0;
	fragmentDatasFlag_ = FRAGMENT_MESSAGE_HREAD;
}

//-------------------------------------------------------------------------------------
void WebSocketPacketFilter::send(Channel * pChannel, const char* data, const int32 datalen){
    static int32 lastPacketLen = 0;
	
    websocket::WebSocketProtocol::FrameType frameType = websocket::WebSocketProtocol::BINARY_FRAME;
    

    if(lastPacketLen == 0){
        if(datalen < 8){
            printf("error!! send packet length is < 8!\n");
            return;
        }
        
        int32 packetLen = *((int32*)(data + sizeof(int32)));
        if(packetLen > datalen){
            frameType = websocket::WebSocketProtocol::INCOMPLETE_BINARY_FRAME;
            lastPacketLen = packetLen - datalen;
        }
    }
    else{
        lastPacketLen -= datalen;
        if(lastPacketLen < 0){
            printf("error! packet len is invalid!\n");
            return;
        }
        frameType = lastPacketLen == 0 ? websocket::WebSocketProtocol::END_FRAME : websocket::WebSocketProtocol::NEXT_FRAME;
    }

    MemoryStream retStream; 
	websocket::WebSocketProtocol::makeFrame(frameType, datalen, &retStream);
	PacketFilter::send(pChannel, (const char*)retStream.data() + retStream.rpos(), retStream.length());
    PacketFilter::send(pChannel, data, datalen);
}

//-------------------------------------------------------------------------------------
int32 WebSocketPacketFilter::recv(Channel* pChannel, char* dataBuf, int32 bufSize){
	int32 recvSize = 0;
	char* recvBuf = _recvFilterBuf->writePtr(recvSize);
	if (recvSize <= 0){
        printf("error!! filter buffer is out!!!\n");
		return -1;
	}
	int32 len = pChannel->getEndPoint()->recv(recvBuf, recvSize);
	if (len > 0)
		_recvFilterBuf->writeIn(len);


    int32 dataSize = _recvFilterBuf->getDataSize();
	if (dataSize <= 0)
		return -1;

	char* temp = (char*)alloca(dataSize);
	const char* data = _recvFilterBuf->get(temp, dataSize, true);
    
    bool isBreak = false;
	while(!isBreak){
		if(fragmentDatasFlag_ == FRAGMENT_MESSAGE_HREAD){
			if(pFragmentDatasRemain_ == 0){
				reset();

				// 如果没有创建过缓存，先尝试直接解析包头，如果信息足够成功解析则继续到下一步
				int32 readOut = websocket::WebSocketProtocol::getFrame(const_cast<char*>(data), dataSize, msg_opcode_, msg_fin_, msg_masked_, msg_mask_, msg_length_field_, msg_payload_length_, msg_frameType_);

				if(readOut > 0){
					fragmentDatasFlag_ = FRAGMENT_MESSAGE_DATAS;
					pFragmentDatasRemain_ = (int32)msg_payload_length_;
                    //PacketFilter::recv(pChannel, data, readOut);
                    _recvFilterBuf->readOut(readOut);
                    data += readOut;
                    dataSize -= readOut;
				}
                else{
                    isBreak = true;
                }
			}

			if(websocket::WebSocketProtocol::ERROR_FRAME == msg_frameType_){
                printf("WebSocketPacketReader::recv: frame is error!\n");
				
                pChannel->destroy();
				reset();
                
                return -1;
			}
			else if(msg_frameType_ == websocket::WebSocketProtocol::TEXT_FRAME || 
					msg_frameType_ == websocket::WebSocketProtocol::INCOMPLETE_TEXT_FRAME ||
					msg_frameType_ == websocket::WebSocketProtocol::PING_FRAME ||
					msg_frameType_ == websocket::WebSocketProtocol::PONG_FRAME)
			{
                printf("WebSocketPacketReader::recv: Does not support FRAME_TYPE(%d)!\n", (int)msg_frameType_);

				pChannel->destroy();
				reset();
				
                return -1;
			}
			else if(msg_frameType_ == websocket::WebSocketProtocol::CLOSE_FRAME){
				pChannel->destroy();
				reset();

				return -1;
			}
			else if(msg_frameType_ == websocket::WebSocketProtocol::INCOMPLETE_FRAME){
				// 继续等待后续内容到达
			}
		}
		else
		{
			if (pFragmentDatasRemain_ <= 0){
                printf("WebSocketPacketReader::recv: pFragmentDatasRemain_ <= 0!\n");

				pChannel->destroy();
				reset();

				return -1;
			}

            int32 decodeSize = pFragmentDatasRemain_ > dataSize ? dataSize : pFragmentDatasRemain_;
            decodeSize = decodeSize > bufSize ? bufSize : decodeSize;            
			if(!websocket::WebSocketProtocol::decodingDatas(const_cast<char*>(data), decodeSize, msg_masked_, msg_mask_)){
                printf("WebSocketPacketReader::recv: decoding-frame is error!\n");
				pChannel->destroy();
				reset();

				return -1;
			}
                
            SafeMemcpy(dataBuf, bufSize, data, decodeSize);

            _recvFilterBuf->readOut(decodeSize);
            pFragmentDatasRemain_ -= decodeSize;
            data += decodeSize;
            dataSize -= decodeSize;

            if(pFragmentDatasRemain_ <= 0)
                reset();

            return decodeSize;
		}
	}
    return -1;
}

//-------------------------------------------------------------------------------------
} 
}
