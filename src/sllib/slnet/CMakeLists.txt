cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

IF(WIN32)
	SET(SYS_LIB_DEBUG
		debug ""
	)
ENDIF(WIN32)


SET(SL_LIB_NET
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/sladdress.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/sladdress.cpp"
	#"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slbundle.h"
	#"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slbundle.inl"
	#"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slbundle.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slchannel.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slchannel.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slnetbase.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slnetbase.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slendpoint.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slendpoint.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slevent_dispatcher.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slevent_dispatcher.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slevent_poller.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slevent_poller.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slinterfaces.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/sllistener_receiver.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/sllistener_receiver.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slnetwork_interface.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slnetwork_interface.cpp"
	#"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slpacket.h"
	#"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slpacket_reader.h"
	#"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slpacket_reader.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slpacket_receiver.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slpacket_receiver.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slpacket_sender.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slpacket_sender.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slpoller_select.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slpoller_select.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slpoller_epoll.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slpoller_epoll.cpp"
	#"${PROJECT_SOURCE_DIR}/src/sllib/slnet/sltcp_packet.h"
	#"${PROJECT_SOURCE_DIR}/src/sllib/slnet/sltcp_packet.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/sltcp_packet_receiver.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/sltcp_packet_receiver.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/sltcp_packet_sender.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/sltcp_packet_sender.cpp"
	#"${PROJECT_SOURCE_DIR}/src/sllib/slnet/sludp_packet.h"
	#"${PROJECT_SOURCE_DIR}/src/sllib/slnet/sludp_packet.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/sludp_packet_receiver.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/sludp_packet_receiver.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/sllistener.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/sllistener.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slconnector.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slconnector.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slnet_module.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slnet_module.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slconnector.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slnet/slconnector.cpp"
)

SOURCE_GROUP(api FILES ${SL_LIB_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(\\ FILES ${SL_LIB_NET})

ADD_LIBRARY(slnet SHARED
	${SL_LIB_API}
	${PUBLIC}
	${SL_LIB_NET}
)
#TARGET_LINK_LIBRARIES(slnet "ws2_32.lib" "Iphlpapi.lib")
SET_PROPERTY(TARGET slnet PROPERTY FOLDER "sllib")