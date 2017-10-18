#ifndef SL_CHAT_PROTOCOL_H
#define SL_CHAT_PROTOCOL_H
#include "GameDefine.h"

enum ChatProtocolID{
	CHATGATE_MSG_VERIFY_TOKEN = 3,
	BALANCE_MSG_CHANNEL_ADDRESS_INFO_SYNC = 101,

	CHAT_API_RGS_LISTENER = 301,
};

#pragma pack(push, 1)
namespace ChatPublic{
	struct TicketID{
		TicketID(const TicketID& dst) : appid(dst.appid), ocid(dst.ocid) {}
		TicketID(int32 _appid, int64 _ocid) : appid(_appid), ocid(_ocid){}

		bool operator==(const TicketID& dst){
			return appid == dst.appid && ocid == dst.ocid;
		}

		operator size_t() const{
			return ocid + appid;
		}

		const int32 appid;
		const int64 ocid;
	};

	struct ticket{
		int32 appID;
		int64 ocID;
		char token[game::MAX_TOKEN_LEN];
	};
	
	struct ChannelID{
		ChannelID(const ChannelID& dst) :appid(dst.appid), ocid(dst.ocid), diyid(dst.diyid){}
		ChannelID(int32 _appid, int64 _ocid, int32 _diyid): appid(_appid), ocid(_ocid), diyid(_diyid){}

		bool operator==(const ChannelID& dst){
			return appid == dst.appid && ocid == dst.ocid && diyid == dst.diyid;
		}

		operator size_t() const{
			return ocid + appid + diyid;
		}
		
		const int32 appid;
		const int64 ocid;
		const int32 diyid;
	};
}

namespace std{
	template <>
	struct hash<ChatPublic::TicketID> {
		inline size_t operator()(const ChatPublic::TicketID& key) const{
			return (size_t)key;
		}
	};

	template<>
	struct hash<ChatPublic::ChannelID>{
		inline size_t operator()(const ChatPublic::ChannelID& key) const{
			return (size_t)key;
		}
	};
}

namespace ChatBalancer{
	struct addrinfo{
		int32 errCode;
		sl::SLString<game::MAX_IP_LEN> ip;
		int32 port;
	};
}

#endif