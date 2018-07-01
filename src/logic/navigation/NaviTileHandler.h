#ifndef SL_MAVI_TILE_HANDLER_H
#define SL_MAVI_TILE_HANDLER_H
#include "INavigation.h"
class NaviTileHandler: public INavigationHandler{
public:
	NaviTileHandler();
	virtual ~NaviTileHandler(){}
	
	virtual int32 findStraightPath(int32 layer, const Position3D& start, const Position3D& end, std::vector<Position3D>& paths);
	virtual int32 findRandomPointAroundCircle(int32 layer, const Position3D& center, std::vector<Position3D>& points, uint32 max_points, float maxRadius);
	virtual int32 raycast(int32 layer, const Position3D& start, const Position3D& end, std::vector<Position3D>& hitPointVec);


};
#endif
