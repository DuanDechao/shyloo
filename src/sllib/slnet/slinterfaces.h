#ifndef _SL_LIB_NET_INTERFACES_H_
#define _SL_LIB_NET_INTERFACES_H_
namespace sl{
namespace network{
class Channel;
class MessageHandler;

//此类接口用于接收普通的Network输入消息
class InputNotificationHandler{
public:
	virtual ~InputNotificationHandler(){};
	virtual int handleInputNotification(int fd) = 0;
};

//此类接口用于接收普通的Network输出消息
class OutputNotificationHandler{
public:
	virtual ~OutputNotificationHandler(){};
	virtual int handleOutputNotification(int fd) = 0;
};
}
}
#endif
