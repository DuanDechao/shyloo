#include "slave/Slave.h"
#include "master/Master.h"
#include "monitor/Monitor.h"
#include "starter/Starter.h"
#include "Cluster.h"
using namespace sl::api;

GET_DLL_ENTRANCE

CREATE_MODULE(Slave)
CREATE_MODULE(Master)
CREATE_MODULE(Monitor)
CREATE_MODULE(Starter)
CREATE_MODULE(Cluster)
