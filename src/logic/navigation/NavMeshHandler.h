#ifndef SL_NAVMESH_HANDLER_H
#define SL_NAVMESH_HANDLER_H
#include <map>
#include "INavigation.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
class NavMeshHandler: public INavigationHandler{
public:
	struct NavMeshLayer{
		dtNavMesh* pNavMesh;
		dtNavMeshQuery* pNavMeshQuery;
	};


	struct NavMeshSetHeader{
		int32 version;
		int32 tileCount;
		dtNavMeshParams params;
	};

	struct NavMeshSetHeaderEx{
		int32 magic;
		int32 version;
		int32 tileCount;
		dtNavMeshParams params;
	};

	struct NavMeshTileHeader{
		dtTileRef tileRef;
		int32 dataSize;
	};

	static const long RCN_NAVMESH_VERSION = 1;

public:
	NavMeshHandler(){}
	virtual ~NavMeshHandler(){}
	
	static INavigationHandler* create(const char* resPath, const std::map<int32, std::string>& params);
	static bool _create(int32 layer, const char* resPath, const char* res, NavMeshHandler* pNavMeshHandler);
	virtual int32 findStraightPath(int32 layer, const Position3D& start, const Position3D& end, std::vector<Position3D>& paths);
	virtual int32 findRandomPointAroundCircle(int32 layer, const Position3D& center, std::vector<Position3D>& points, uint32 max_points, float maxRadius);
	virtual int32 raycast(int32 layer, const Position3D& start, const Position3D& end, std::vector<Position3D>& hitPointVec);

private:
	std::map<int32, NavMeshLayer> _navMeshLayer; 
	std::string _resPath;
};
#endif
