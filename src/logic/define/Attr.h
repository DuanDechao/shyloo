#ifndef __ATTR_H__
#define __ATTR_H__

#ifdef ATTR_EXPORT
#define ATTR_API __declspec (dllexport)
#else
#define ATTR_API __declspec (dllimport)
#endif

extern "C" ATTR_API struct attr_def{
	const IProp* exp		= nullptr;
	const IProp* id		= nullptr;
	const IProp* status		= nullptr;
}

#endif
