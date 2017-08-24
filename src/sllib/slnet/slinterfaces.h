#ifndef _SL_LIB_NET_INTERFACES_H_
#define _SL_LIB_NET_INTERFACES_H_
namespace sl{
namespace network{
class Channel;
class MessageHandler;

//����ӿ����ڽ�����ͨ��Network������Ϣ
class InputNotificationHandler{
public:
	virtual ~InputNotificationHandler(){};
	virtual int handleInputNotification(int fd) = 0;
};

//����ӿ����ڽ�����ͨ��Network�����Ϣ
class OutputNotificationHandler{
public:
	virtual ~OutputNotificationHandler(){};
	virtual int handleOutputNotification(int fd) = 0;
};
}
}
#endif
