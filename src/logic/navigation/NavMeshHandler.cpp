#include "NavMeshHandler.h"
#include "IResMgr.h"
#include "slfile_utils.h"
#include "DetourCommon.h"
#include "IDebugHelper.h"
static float frand(){
	return (float)rand() / (float)RAND_MAX;
}
INavigationHandler* NavMeshHandler::create(const char* resPath, const std::map<int32, std::string>& params){
	if(!resPath || strcmp(resPath, "") == 0)
		return NULL;

	std::string path = resPath;
	path = SLMODULE(ResMgr)->matchRes(path);
	if(path.size() == 0)
		return NULL;

	NavMeshHandler* pNavMeshHandler = NULL;
	if(params.size() == 0){
		std::vector<std::string> results;
		sl::CFileUtils::ListFileInDirection(path.c_str(), ".navmesh", [&results](const char * name, const char * path) {
			results.push_back(path);
		});

		if(results.size() == 0){
			ECHO_ERROR("NavMeshHandler::create: path[%s] not found navmesh", path.c_str());
			return NULL;
		}

		pNavMeshHandler = NEW NavMeshHandler();
		int32 layer = 0;
		for(auto itor : results){
			_create(layer++, resPath, itor.c_str(), pNavMeshHandler);
		}
	}
	else{
		pNavMeshHandler = NEW NavMeshHandler();
		for(auto param : params){
			std::string filePath = path + "/" + param.second;
			_create(param.first, resPath, filePath.c_str(), pNavMeshHandler);
		}
	}

	return pNavMeshHandler;
}


template<typename NAVMESH_SET_HEADER>
dtNavMesh* tryReadNavMesh(char* data, size_t readSize, const char* res){
	if(readSize < sizeof(NAVMESH_SET_HEADER)){
		ECHO_ERROR("NavMeshHandler::readNavMesh: open[%s], NavMeshSetHeader error", res);
		return NULL;
	}

	int32 pos = 0;
	int32 size = 0;

	NAVMESH_SET_HEADER header;
	size = sizeof(NAVMESH_SET_HEADER);
	memcpy(&header, data, size);

	if(header.version != NavMeshHandler::RCN_NAVMESH_VERSION){
		ECHO_ERROR("NavMeshHandler::readNavMesh: navmesh version[%d] is not match[%d]", header.version,NavMeshHandler::RCN_NAVMESH_VERSION);
		return NULL;
	}

	dtNavMesh* mesh = dtAllocNavMesh();
	if(!mesh){
		ECHO_ERROR("NavMeshHandler::readNavMesh: dtAllocNavMesh is failed");
		return NULL;
	}

	dtStatus status = mesh->init(&header.params);
	if(dtStatusFailed(status)){
		ECHO_ERROR("NavMeshHandler::readNavMesh: mesh init error[%s]", status);
		dtFreeNavMesh(mesh);
		return NULL;
	}

	//read tiles;
	bool success = true;
	pos += size;
	for(int32 i = 0; i < header.tileCount; i++){
		NavMeshHandler::NavMeshTileHeader tileHeader;
		size = sizeof(NavMeshHandler::NavMeshTileHeader);

		memcpy(&tileHeader, &data[pos], size);
		pos += size;

		size = tileHeader.dataSize;
		if(!tileHeader.tileRef || !tileHeader.dataSize){
			success = false;
			status = DT_FAILURE + DT_INVALID_PARAM;
			break;
		}

		unsigned char* tileData = (unsigned char*)dtAlloc(size, DT_ALLOC_PERM);
		if(!tileData){
			success = false;
			status = DT_FAILURE + DT_OUT_OF_MEMORY;
			break;
		}

		memcpy(tileData, &data[pos], size);
		pos += size;

		status = mesh->addTile(tileData, size, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);

		if(dtStatusFailed(status)){
			success = false;
			break;
		}
	}

	if(!success){
		ECHO_ERROR("NavMeshHandler::readNavMesh: error[%d]", status);
		dtFreeNavMesh(mesh);
		return NULL;
	}

	return mesh;
}

bool NavMeshHandler::_create(int32 layer, const char* resPath, const char* res, NavMeshHandler* pNavMeshHandler){
	FILE* fp = fopen(res, "rb");
	if(!fp){
		ECHO_ERROR("NavMeshHandler::create: open[%s] error", res);
		return false;
	}

	fseek(fp, 0, SEEK_END);
	size_t flen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* data = NEW char[flen];
	if(!data){
		ECHO_ERROR("NavMeshHandler::create: open(%s), memory[size=%d] error", res, flen);
		fclose(fp);
		DEL [] data;
		data = NULL;
		return false;
	}
	
	size_t readSize = fread(data, 1, flen, fp);
	if(readSize != flen){
		ECHO_ERROR("NavMeshHandler::create: open(%s), memory[size=%d] error", res, flen);
		fclose(fp);
		DEL [] data;
		data = NULL;
		return false;
	}

	dtNavMesh* mesh = tryReadNavMesh<NavMeshHandler::NavMeshSetHeader>(data, readSize, res);
	if(!mesh){
		mesh = tryReadNavMesh<NavMeshHandler::NavMeshSetHeaderEx>(data, readSize, res);
	}

	if(!mesh){
		ECHO_ERROR("NavMeshHandler::create: dtAllocNavMesh is failed");
		fclose(fp);
		DEL [] data;
		data = NULL;
		return false;
	}

	fclose(fp);
	DEL [] data;
	data = NULL;

	dtNavMeshQuery* pNavMeshQuery = NEW dtNavMeshQuery();
	pNavMeshQuery->init(mesh, 1024);
	pNavMeshHandler->_resPath = resPath;
	pNavMeshHandler->_navMeshLayer[layer].pNavMeshQuery = pNavMeshQuery;
	pNavMeshHandler->_navMeshLayer[layer].pNavMesh = mesh;

	uint32 tileCount = 0;
	uint32 nodeCount = 0;
	uint32 polyCount = 0;
	uint32 vertCount = 0;
	uint32 triCount = 0;
	uint32 triVertCount = 0;
	uint32 dataSize = 0;

	const dtNavMesh* navmesh = mesh;
	for(int32 i = 0; i < navmesh->getMaxTiles(); ++i){
		const dtMeshTile* tile = navmesh->getTile(i);
		if(!tile || !tile->header){
			continue;
		}

		tileCount++;
		nodeCount += tile->header->bvNodeCount;
		polyCount += tile->header->polyCount;
		vertCount += tile->header->vertCount;
		triCount += tile->header->detailTriCount;
		triVertCount += tile->header->detailVertCount;
		dataSize += tile->dataSize;
	}
	
	ECHO_TRACE("==> tiles loaded: %d", tileCount);
	ECHO_TRACE("==> BVTree nodes: %d", nodeCount);
	ECHO_TRACE("==> %d polygons (%d vertices)", polyCount, vertCount);
	ECHO_TRACE("==> %d triangles (%d vertices)", triCount, triVertCount);
	ECHO_TRACE("==> %.2f MB of data", (((float)dataSize / sizeof(unsigned char)) / 1048576));
}

int32 NavMeshHandler::findStraightPath(int32 layer, const Position3D& start, const Position3D& end, std::vector<Position3D>& paths){
	auto itor = _navMeshLayer.find(layer);
	if(itor == _navMeshLayer.end()){
		ECHO_ERROR("NavMeshHandler::findStraightPath: not found layer[%d]", layer);
		return -1;
	}

	dtNavMeshQuery* navMeshQuery = itor->second.pNavMeshQuery;

	float spos[3];
	spos[0] = start.x;
	spos[1] = start.y;
	spos[2] = start.z;

	float epos[3];
	epos[0] = end.x;
	epos[1] = end.y;
	epos[2] = end.z;

	dtQueryFilter filter;
	filter.setIncludeFlags(0xffff);
	filter.setExcludeFlags(0);

	const float extents[3] = {2.f, 4.f, 2.f};
	dtPolyRef startRef = 0;
	dtPolyRef endRef = 0;

	float startNearestPt[3];
	float endNearestPt[3];
	navMeshQuery->findNearestPoly(spos, extents, &filter, &startRef, startNearestPt);
	navMeshQuery->findNearestPoly(epos, extents, &filter, &endRef, endNearestPt);

	if(!startRef || !endRef){
		ECHO_ERROR("NavMeshHandler::findStraightPath: could not find any nearby poly[%d %d]", startRef, endRef);
		return -2;
	}

	dtPolyRef polys[256];
	int32 nPolys = 0;
	float straightPath[256 * 3];
	unsigned char straightPathFlags[256];
	dtPolyRef straightPathPolys[256];
	int32 nStraightPath = 0;
	int32 pos = 0;
	
	navMeshQuery->findPath(startRef, endRef, startNearestPt, endNearestPt, &filter, polys, &nPolys, 256);
	if(nPolys){
		float epos1[3];
		dtVcopy(epos1, endNearestPt);
		if(polys[nPolys -1] != endRef)
			navMeshQuery->closestPointOnPoly(polys[nPolys - 1], endNearestPt, epos1, 0);

		navMeshQuery->findStraightPath(startNearestPt, endNearestPt, polys, nPolys, straightPath, straightPathFlags, straightPathPolys, &nStraightPath, 256);

		Position3D curPos;
		for(int32 i = 0; i < nStraightPath * 3;){
			curPos.x = straightPath[i++];
			curPos.y = straightPath[i++];
			curPos.z = straightPath[i++];
			paths.push_back(curPos);
			pos++;
		}
	}

	return pos;
}
int32 NavMeshHandler::findRandomPointAroundCircle(int32 layer, const Position3D& center, std::vector<Position3D>& points, uint32 maxPoints, float maxRadius){
	auto itor = _navMeshLayer.find(layer);
	if(itor == _navMeshLayer.end()){
		ECHO_ERROR("NavMeshHandler::findRandomPointAroundCircle: not find layer[%d]", layer);
		return -1;
	}

	dtNavMeshQuery* navMeshQuery = itor->second.pNavMeshQuery;

	dtQueryFilter filter;
	filter.setIncludeFlags(0xffff);
	filter.setExcludeFlags(0);

	if(maxRadius <= 0.0001f){
		Position3D curPos;
		for(int32 i = 0; i < maxPoints; i++){
			float pt[3];
			dtPolyRef ref;
			dtStatus status = navMeshQuery->findRandomPoint(&filter, frand, &ref, pt);
			if(dtStatusSucceed(status)){
				curPos.x = pt[0];
				curPos.y = pt[1];
				curPos.z = pt[2];

				points.push_back(curPos);
			}
		}
		return (int32)points.size();
	}

	const float extents[3] = {2.f, 4.f, 2.f};
	dtPolyRef startRef = 0;

	float spos[3];
	spos[0] = center.x;
	spos[1] = center.y;
	spos[2] = center.z;

	float startNearestPt[3];
	navMeshQuery->findNearestPoly(spos, extents, &filter, &startRef, startNearestPt);
	if(!startRef){
		ECHO_ERROR("NavMeshHandler::findRandomPointAroundCircle: could not find any nearby poly[%d]", startRef);
		return -2;
	}

	Position3D curPos;
	bool done = false;
	int32 itry = 0;
	while(itry++ < 3 && points.size() == 0){
		maxPoints -= points.size();
		for(int32 i = 0; i < maxPoints; i++){
			float pt[3];
			dtPolyRef ref;
			dtStatus status = navMeshQuery->findRandomPointAroundCircle(startRef, spos, maxRadius, &filter, frand, &ref, pt);
			if(dtStatusSucceed(status)){
				done = true;
				curPos.x = pt[0];
				curPos.y = pt[1];
				curPos.z = pt[2];

				Position3D v = center - curPos;
				float distLen = SLVec3Length(&v);
				if(distLen > maxRadius){
					continue;
				}

				points.push_back(curPos);
			}
		}

		if(!done)
			break;
	}

	return (int32)points.size();
}
int32 NavMeshHandler::raycast(int32 layer, const Position3D& start, const Position3D& end, std::vector<Position3D>& hitPointVec){
	auto itor = _navMeshLayer.find(layer);
	if(itor == _navMeshLayer.end()){
		ECHO_ERROR("NavMeshHandler::findRandomPointAroundCircle: not find layer[%d]", layer);
		return -1;
	}

	dtNavMeshQuery* navMeshQuery = itor->second.pNavMeshQuery;

	float hitPoint[3];

	float spos[3];
	spos[0] = start.x;
	spos[1] = start.y;
	spos[2] = start.z;

	float epos[3];
	epos[0] = end.x;
	epos[1] = end.y;
	epos[2] = end.z;

	dtQueryFilter filter;
	filter.setIncludeFlags(0xffff);
	filter.setExcludeFlags(0);
	
	const float extents[3] = {2.f, 4.f, 2.f};
	dtPolyRef startRef = 0;

	float nearestPt[3];
	navMeshQuery->findNearestPoly(spos, extents, &filter, &startRef, nearestPt);
	if(!startRef){
		ECHO_ERROR("NavMeshHandler::findRandomPointAroundCircle: could not find any nearby poly[%d]", startRef);
		return -2;
	}

	float t = 0;
	float hitNormal[3];
	memset(hitNormal, 0, sizeof(hitNormal));

	dtPolyRef polys[256];
	int32 nPolys = 0;

	navMeshQuery->raycast(startRef, spos, epos, &filter, &t, hitNormal, polys, &nPolys, 256);
	if(t > 1){
		// not hit
		return -1;
	}
	else{
		//hit
		hitPoint[0] = spos[0] + (epos[0] - spos[0]) * t;
		hitPoint[1] = spos[1] + (epos[1] - spos[1]) * t;
		hitPoint[2] = spos[2] + (epos[2] - spos[2]) * t;
		if(nPolys){
			float h = 0;
			navMeshQuery->getPolyHeight(polys[nPolys -1], hitPoint, &h);
			hitPoint[1] = h;
		}
	}

	hitPointVec.push_back(Position3D(hitPoint[0], hitPoint[1], hitPoint[2]));
	return 1;
}
