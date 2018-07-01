#ifndef _SL_INTERFACE_CLUSTER_H__
#define _SL_INTERFACE_CLUSTER_H__
#include "slimodule.h"
class ICluster : public sl::api::IModule{
public:
	virtual ~ICluster() {}

	virtual bool clusterIsReady() = 0;
};

#endif
