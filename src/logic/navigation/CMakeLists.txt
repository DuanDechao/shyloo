INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
	"${PROJECT_SOURCE_DIR}/thirdlibrary/python/Include"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourCrowd"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourTileCache"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Recast"
)

SET(NAVIGATION
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour/DetourAlloc.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour/DetourAlloc.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour/DetourAssert.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour/DetourAssert.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour/DetourCommon.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour/DetourCommon.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour/DetourMath.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour/DetourNavMesh.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour/DetourNavMesh.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour/DetourNavMeshBuilder.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour/DetourNavMeshBuilder.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour/DetourNavMeshQuery.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour/DetourNavMeshQuery.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour/DetourNode.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour/DetourNode.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Detour/DetourStatus.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourCrowd/DetourCrowd.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourCrowd/DetourCrowd.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourCrowd/DetourLocalBoundary.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourCrowd/DetourLocalBoundary.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourCrowd/DetourObstacleAvoidance.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourCrowd/DetourObstacleAvoidance.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourCrowd/DetourPathCorridor.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourCrowd/DetourPathCorridor.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourCrowd/DetourPathQueue.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourCrowd/DetourPathQueue.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourCrowd/DetourProximityGrid.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourCrowd/DetourProximityGrid.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourTileCache/DetourTileCache.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourTileCache/DetourTileCache.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourTileCache/DetourTileCacheBuilder.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/DetourTileCache/DetourTileCacheBuilder.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Recast/Recast.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Recast/Recast.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Recast/RecastAlloc.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Recast/RecastAlloc.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Recast/RecastArea.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Recast/RecastAssert.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Recast/RecastAssert.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Recast/RecastContour.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Recast/RecastFilter.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Recast/RecastLayers.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Recast/RecastMesh.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Recast/RecastMeshDetail.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Recast/RecastRasterization.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Recast/RecastRegion.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/NavMeshHandler.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/NavMeshHandler.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Navigation.h"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/Navigation.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/navigation/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${NAVIGATION})

ADD_LIBRARY(Navigation SHARED
	${NAVIGATION}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)
SET_PROPERTY(TARGET Navigation PROPERTY FOLDER "logic")
