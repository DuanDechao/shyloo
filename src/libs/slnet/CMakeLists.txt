INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/common"
)

IF(WIN32)
	SET(SYS_LIB_DEBUG
		debug ""
	)
ENDIF(WIN32)

SET(NETWORK
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/sladdress.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/sladdress.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slbundle.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slbundle.inl"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slbundle.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slbundle_broadcast.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slbundle_broadcast.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slchannel.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slchannel.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slnetbase.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slnetbase.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slendpoint.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slendpoint.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slepoll.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slevent_dispatcher.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slevent_dispatcher.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slevent_poller.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slevent_poller.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slfixed_messages.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slfixed_messages.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slinterfaces.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/sllistener_receiver.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/sllistener_receiver.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slmessage_handler.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slmessage_handler.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slnetwork_interface.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slnetwork_interface.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slpacket.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slpacket_filter.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slpacket_filter.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slpacket_reader.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slpacket_reader.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slpacket_receiver.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slpacket_receiver.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slpacket_sender.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slpacket_sender.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slpoller_select.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slpoller_select.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slsocket.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/slsocket_utils.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/sltcp_packet.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/sltcp_packet.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/sltcp_packet_receiver.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/sltcp_packet_receiver.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/sltcp_packet_sender.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/sltcp_packet_sender.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/sludp_packet.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/sludp_packet.cpp"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/sludp_packet_receiver.h"
	"${PROJECT_SOURCE_DIR}/src/libs/slnet/sludp_packet_receiver.cpp"

)

SOURCE_GROUP(common FILES ${COMMON})
SOURCE_GROUP(\\ FILES ${NETWORK})

ADD_LIBRARY(slnet SHARED
	${COMMON}
	${NETWORK}
)

TARGET_LINK_LIBRARIES(slnet "ws2_32.lib" "Iphlpapi.lib")

SET_PROPERTY(TARGET slnet PROPERTY FOLDER "libs")