#ifndef __ATTR_H__
#define __ATTR_H__

#ifdef ATTR_EXPORT
#define ATTR_API __declspec (dllexport)
#else
#define ATTR_API __declspec (dllimport)
#endif

class IProp;

struct ATTR_API attr_def{
	static const IProp* account;
	static const IProp* exp;
	static const IProp* gate;
	static const IProp* id;
	static const IProp* logic;
	static const IProp* recoverTimer;
	static const IProp* status;
};

#endif
