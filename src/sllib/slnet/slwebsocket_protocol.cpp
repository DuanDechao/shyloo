#include "slwebsocket_protocol.h"
#include "slmemorystream.h"
#include "slmemorystream_converter.h"
//#include "network/channel.h"
//#include "network/packet.h"
#include "slbase64.h"
#include "slsha1.h"
#include <string>
#include "slstring_utils.h"
#include <unordered_map>
/*#if KBE_PLATFORM == PLATFORM_WIN32
#ifdef _DEBUG
#pragma comment(lib, "libeay32_d.lib")
#pragma comment(lib, "ssleay32_d.lib")
#else
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#endif
#endif
*/
namespace sl{
namespace network{
namespace websocket{

//-------------------------------------------------------------------------------------
bool WebSocketProtocol::isWebSocketProtocol(const char* data, const int32 datalen){
    if(!data || datalen < 2)
        return false;

    int32 strLen = strlen(data);
	strLen = strLen > datalen ? datalen : strLen;
    std::string webData(data, strLen);

	size_t fi = webData.find_first_of("Sec-WebSocket-Key");
	if(fi == std::string::npos){
        return false;
	}

	fi = webData.find_first_of("GET");
	if(fi == std::string::npos){
        return false;
	}

	std::vector<std::string> header_and_data;
	header_and_data = sl::CStringUtils::splits(webData, "\r\n\r\n");
	
	if(header_and_data.size() != 2){
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------
int32 WebSocketProtocol::handshake(network::Channel* pChannel, const char* data, const int32 datalen){
	
	// �ַ������Ͻ��������ٳ�����Ҫ����2�����򷵻�����MemoryStream�����쳣
	if(!data || datalen < 2)
		return -1;
	
    int32 strLen = strlen(data);
	strLen = strLen > datalen ? datalen : strLen;
    std::string webData(data, strLen);
	
    std::vector<std::string> header_and_data;
	header_and_data = sl::CStringUtils::splits(webData, "\r\n\r\n");
	
	if(header_and_data.size() != 2)	{
		return -1;
	}

    std::unordered_map<std::string, std::string> headers;
	std::vector<std::string> values;
	
	values = sl::CStringUtils::splits(header_and_data[0], "\r\n");
	std::vector<std::string>::iterator iter = values.begin();

	for(; iter != values.end(); ++iter){
		header_and_data = sl::CStringUtils::splits((*iter), ": ");

		if(header_and_data.size() == 2)
			headers[header_and_data[0]] = header_and_data[1];
	}

	std::string szKey, szOrigin, szHost;

    std::unordered_map<std::string, std::string>::iterator findIter = headers.find("Sec-WebSocket-Origin");
	if (findIter != headers.end()){
        char tmp[1024] = {0};
        SafeSprintf(tmp, 1024, "WebSocket-Origin: %s\r\n", 1024, findIter->second.c_str());
        szOrigin = tmp;
    }

	findIter = headers.find("Sec-WebSocket-Key");
	if(findIter == headers.end()){
		return -1;
	}

	szKey = findIter->second;

	findIter = headers.find("Host");
	if(findIter == headers.end()){
		return -1;
	}

	szHost = findIter->second;

    std::string server_key = szKey;

	//RFC6544_MAGIC_KEY
    server_key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

	SHA1 sha;
	unsigned int message_digest[5];

	sha.Reset();
	sha << server_key.c_str();
	sha.Result(message_digest);

	for (int i = 0; i < 5; ++i)
		message_digest[i] = htonl(message_digest[i]);

    server_key = base64_encode(reinterpret_cast<const unsigned char*>(message_digest), 20);

    std::string ackHandshake = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
    ackHandshake += server_key;
    ackHandshake += "\r\n";
    ackHandshake += szOrigin;
    ackHandshake += "WebSocket-Location: ws://";
    ackHandshake += szHost;
    ackHandshake += "/WebManagerSocket\r\nWebSocket-Protocol: WebManagerSocket\r\n\r\n";
    
    pChannel->getEndPoint()->send(ackHandshake.c_str(), ackHandshake.length());
	return strLen;
}

//-------------------------------------------------------------------------------------
int WebSocketProtocol::makeFrame(WebSocketProtocol::FrameType frame_type, uint64 size, MemoryStream * pOutStream){
	// д��frame����
	(*pOutStream) << ((uint8)frame_type); 

	if(size <= 125){
		(*pOutStream) << ((uint8)size);
	}
	else if (size <= 65535){
		uint8 bytelength = 126;
		(*pOutStream) << bytelength; 

		(*pOutStream) << ((uint8)(( size >> 8 ) & 0xff));
		(*pOutStream) << ((uint8)(( size ) & 0xff));
	}
	else
	{
		uint8 bytelength = 127;
		(*pOutStream) << bytelength; 

		MemoryStreamConverter::apply<uint64>(&size);
		(*pOutStream) << size;
	}

	return pOutStream->length();
}

//-------------------------------------------------------------------------------------
int WebSocketProtocol::getFrame(char* data, uint64 datalen, uint8& msg_opcode, uint8& msg_fin, uint8& msg_masked, uint32& msg_mask, 
		int32& msg_length_field, uint64& msg_payload_length, FrameType& frameType)
{
	/*
	 	0                   1                   2                   3
	 	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		+-+-+-+-+-------+-+-------------+-------------------------------+
		|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
		|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
		|N|V|V|V|       |S|             |   (if payload len==126/127)   |
		| |1|2|3|       |K|             |                               |
		+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
		|     Extended payload length continued, if payload len == 127  |
		+ - - - - - - - - - - - - - - - +-------------------------------+
		|                               |Masking-key, if MASK set to 1  |
		+-------------------------------+-------------------------------+
		| Masking-key (continued)       |          Payload Data         |
		+-------------------------------- - - - - - - - - - - - - - - - +
		:                     Payload Data continued ...                :
		+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
		|                     Payload Data continued ...                |
		+---------------------------------------------------------------+
	*/

	// ����3�ֽڣ���Ҫ�����ȴ�
    const char* originData = data;
	int remainSize = 3 - datalen;
	if(remainSize > 0) {
		frameType = INCOMPLETE_FRAME;
		return 0;
	}
	
	// ��һ���ֽ�, ���λ����������Ϣ�Ƿ����, ���4λ����������Ϣ����
	uint8 bytedata = *((uint8*)data);
    data += 1;

	msg_opcode = bytedata & 0x0F;
	msg_fin = (bytedata >> 7) & 0x01;

	// �ڶ����ֽ�, ��Ϣ�ĵڶ����ֽ���Ҫ���������������Ϣ����, ���λ��0��1�������Ƿ������봦��
	bytedata = *((uint8*)data);
    data += 1; 
	msg_masked = (bytedata >> 7) & 0x01;

	// ��Ϣ����
	msg_length_field = bytedata & (~0x80);

	// ʣ�µĺ���7λ����������Ϣ����, ����7λ���ֻ������127�������ֵ������������
	// һ������Ϣ��������126�洢��Ϣ����, �����Ϣ��������UINT16�������ֵΪ126
	// ����Ϣ���ȴ���UINT16������´�ֵΪ127;
	// �������������Ϣ���ȴ洢����������byte[], �ֱ���UINT16(2λbyte)��UINT64(4λbyte)
	if(msg_length_field <= 125) {
		msg_payload_length = msg_length_field;
	}
	else if(msg_length_field == 126) { 
		// ����2�ֽڣ���Ҫ�����ȴ�
		remainSize = 2 - datalen;
		if(remainSize > 0) {
			frameType = INCOMPLETE_FRAME;
			return 0;
		}
	    
		uint8 bytedata1 = *((uint8*)data);
        data += 1;
        uint8 bytedata2 = *((uint8*)data);
        data += 1;
		msg_payload_length = (bytedata1 << 8) | bytedata2;
	}
	else if(msg_length_field == 127) {
		// ����8�ֽڣ���Ҫ�����ȴ�
		remainSize = 8 - datalen;
		if(remainSize > 0) {
			frameType = INCOMPLETE_FRAME;
			return 0;
		}
		
		msg_payload_length = ((uint64)(data + 0) << 56) |
                         ((uint64)(data + 1) << 48) |
                         ((uint64)(data + 2) << 40) |
                         ((uint64)(data + 3) << 32) |
                         ((uint64)(data + 4) << 24) |
                         ((uint64)(data + 5) << 16) |
                         ((uint64)(data + 6) << 8) |
                         ((uint64)(data + 7));

		data += 8;
	}

	// ����ɶ����Ȳ���
	/* ���ﲻ����飬ֻ����Э��ͷ
	if(pPacket->length() < (size_t)msg_payload_length) {
		frameType = INCOMPLETE_FRAME;
		return (size_t)msg_payload_length - pPacket->length();
	}
	*/

	// ����������������»�ȡ4�ֽ�����ֵ
	if(msg_masked) {
		// ����4�ֽڣ���Ҫ�����ȴ�
		remainSize = 4 - datalen;
		if(remainSize > 0) {
			frameType = INCOMPLETE_FRAME;
			return 0;
		}
		
        msg_mask = *((uint32*)data);
        data += sizeof(uint32);
	}
	
	if(NETWORK_MESSAGE_MAX_SIZE < msg_payload_length){
		//WARNING_MSG(fmt::format("WebSocketProtocol::getFrame: msglen exceeds the limit! msglen=({}), maxlen={}.\n", 
		//	msg_payload_length, NETWORK_MESSAGE_MAX_SIZE));
        printf("WebSocketProtocol::getFrame: msglen exceeds the limit! msglen=(%d), maxlen=%d.\n", msg_payload_length, NETWORK_MESSAGE_MAX_SIZE);

		frameType = ERROR_FRAME;
		return data - originData;
	}

	if(msg_opcode == 0x0) frameType = (msg_fin) ? BINARY_FRAME : INCOMPLETE_BINARY_FRAME; // continuation frame ?
	else if(msg_opcode == 0x1) frameType = (msg_fin) ? TEXT_FRAME : INCOMPLETE_TEXT_FRAME;
	else if(msg_opcode == 0x2) frameType = (msg_fin) ? BINARY_FRAME : INCOMPLETE_BINARY_FRAME;
	else if(msg_opcode == 0x8) frameType = CLOSE_FRAME;
	else if(msg_opcode == 0x9) frameType = PING_FRAME;
	else if(msg_opcode == 0xA) frameType = PONG_FRAME;
	else frameType = ERROR_FRAME;

	return data -originData;
}

//-------------------------------------------------------------------------------------
bool WebSocketProtocol::decodingDatas(char* data, int32 datalen, uint8 msg_masked, uint32 msg_mask){
	// ��������
	if(msg_masked) {
		for(int i=0; i< datalen; i++) {
			data[i] = ((uint8)data[i]) ^ ((uint8*)(&msg_mask))[i % 4];
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------
}
}
}
