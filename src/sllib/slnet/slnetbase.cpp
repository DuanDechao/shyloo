#include "sltype.h"
namespace sl
{
namespace network
{
uint32	g_SOMAXCONN = 5;
float g_channelExternalTimeout = 330;
float g_channelInternalTimeout = 330;
uint64 g_numBytesReceived = 0;
uint64 g_numBytesSent = 0;
uint64 g_numPacketsReceived = 0;
uint64 g_numPacketsSent = 0;
}
}