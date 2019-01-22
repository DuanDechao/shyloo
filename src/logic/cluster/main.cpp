#include "slave/Slave.h"
#include "master/Master.h"
#include "starter/Starter.h"
#include "node/Node.h"
#include "machine/Machine.h"
#include "Cluster.h"
using namespace sl::api;

GET_DLL_ENTRANCE

CREATE_MODULE(Slave)
CREATE_MODULE(Master)
CREATE_MODULE(Node)
CREATE_MODULE(Machine)
CREATE_MODULE(Starter)
CREATE_MODULE(Cluster)
