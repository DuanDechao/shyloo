cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/kernel/sl/api")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/kernel/sl/public")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet")

INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/common"
)

IF(WIN32)
	SET(SYS_LIB_DEBUG
		debug ""
	)
ENDIF(WIN32)

#SET(SYS_LIBS ws2_32.lib Iphlpapi.lib)

SET(SLNET
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/sladdress.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/sladdress.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slbundle.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slbundle.inl"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slbundle.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slbundle_broadcast.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slbundle_broadcast.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slchannel.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slchannel.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slnetbase.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slnetbase.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slendpoint.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slendpoint.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slepoll.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slevent_dispatcher.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slevent_dispatcher.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slevent_poller.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slevent_poller.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slfixed_messages.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slfixed_messages.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slinterfaces.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/sllistener_receiver.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/sllistener_receiver.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slmessage_handler.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slmessage_handler.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slnetwork_interface.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slnetwork_interface.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slpacket.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slpacket_filter.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slpacket_filter.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slpacket_reader.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slpacket_reader.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slpacket_receiver.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slpacket_receiver.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slpacket_sender.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slpacket_sender.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slpoller_select.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slpoller_select.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slsocket.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slsocket_utils.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/sltcp_packet.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/sltcp_packet.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/sltcp_packet_receiver.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/sltcp_packet_receiver.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/sltcp_packet_sender.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/sltcp_packet_sender.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/sludp_packet.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/sludp_packet.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/sludp_packet_receiver.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/sludp_packet_receiver.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/sllistener.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/sllistener.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slnet_module.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet/slnet_module.cpp"

)

SOURCE_GROUP(api FILES ${API})
SOURCE_GROUP(\\ FILES ${SLNET})

ADD_LIBRARY(slnet SHARED
	${API}
	${SLNET}
)

TARGET_LINK_LIBRARIES(slnet "ws2_32.lib" "Iphlpapi.lib")

#SET_PROPERTY(TARGET slnet PROPERTY FOLDER "libs")