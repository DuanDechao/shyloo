#ifndef _SL_INTERFACE_NAVIGATION_H__
#define _SL_INTERFACE_NAVIGATION_H__
#include "slimodule.h"
#include "slmath.h"
#include <vector>
#include <map>
class INavigationHandler{
public:
	virtual ~INavigationHandler() {}
	virtual int32 findStraightPath(int32 layer, const Position3D& start, const Position3D& end, std::vector<Position3D>& paths) = 0;
	virtual int32 findRandomPointAroundCircle(int32 layer, const Position3D& center, std::vector<Position3D>& points, uint32 max_points, float maxRadius) = 0;
	virtual int32 raycast(int32 layer, const Position3D& start, const Position3D& end, std::vector<Position3D>& hitPointVec) = 0;
};

class INavigation : public sl::api::IModule{
public:
	virtual ~INavigation() {}
	virtual INavigationHandler* loadNavigation(const char* resPath, const std::map<int, std::string>& params) = 0;
	virtual bool hasNavigation(const char* resPath) = 0;
	virtual bool removeNavigation(const char* resPath) = 0;
	virtual INavigationHandler* findNavigation(const char* resPath) = 0;
};
#endif
