#ifndef SL_COMMON_H_
#define SL_COMMON_H_
#include "slbase.h"
#include "slplatform.h"
namespace sl
{

#define MAX_BUF	256
#define SAFE_RELEASE(i)			\
	if(i)						\
	{                           \
	    delete i;				\
		i = NULL;				\
    }

enum ENTITY_MAILBOX_TYPE
{
	MAILBOX_TYPE_CELL				=	0,
	MAILBOX_TYPE_BASE				=	1,
	MAILBOX_TYPE_CLIENT				=	2,
	MAILBOX_TYPE_CELL_VIA_BASE		=	3,
	MAILBOX_TYPE_BASE_VIA_CELL		=	4,
	MAILBOX_TYPE_CLIENT_VIA_CELL	=	5,
	MAILBOX_TYPE_CLIENT_VIA_BASE	=	6,
};
enum COMPONENT_TYPE
{
	UNKNOWN_COMPONENT_TYPE	=	0,
	DBMGR_TYPE				=	1,
	LOGINAPP_TYPE			=	2,
	BASEAPPMGR_TYPE			=	3,
	CELLAPPMGR_TYPE			=	4,
	CELLAPP_TYPE			=	5,
	BASEAPP_TYPE			=	6,
	CLIENT_TYPE				=	7,
	MACHINE_TYPE			=	8,
	CONSOLE_TYPE			=	9,
	LOGGER_TYPE				=	10,
	BOTS_TYPE				=	11,
	WATCHER_TYPE			=	12,
	INTERFACES_TYPE			=	13,
	COMPONENT_END_TYPE		=	14,
};

//当前服务器组件类别和ID
extern COMPONENT_TYPE		g_componentType;
extern COMPONENT_ID			g_componentID;
}
#endif