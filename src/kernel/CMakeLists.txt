
IF(WIN32)
INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/api"
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/interface"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/api"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/slnet"
	"${PROJECT_SOURCE_DIR}/src/kernel"
)
ENDIF(WIN32)


SET(KERNEL_SL_API
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/api/sltype.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/api/slnet.h"
)

SET(KERNEL_SL_PUBLIC
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slassert.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slbase.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slbase_define.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slcode_convert.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slcommon.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slcommon.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slconfig.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slfile.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slfile_utils.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/sllog.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slmemorystream.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slmemorystream.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slmemorystream_converter.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slmulti_object_mgr.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slmutex.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slobj_reload_check.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slobject_mgr.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slobjectpool.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slplatform.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slrefcountable.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slsingleton.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slsize_string.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slsmartpointer.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/slstring_utils.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/sltask.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/sltasks.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/sltasks.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/sltime.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/sltimer.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/sltimer.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/sltimestamp.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/sltimestamp.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/sl/public/sltype.h"	
)

SET(KERNEL_SL_NET
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

SET(KERNEL_INTERFACE
	"${PROJECT_SOURCE_DIR}/src/kernel/interface/slicore.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/interface/slinet_engine.h"
)

SET(KERNEL_NET
	"${PROJECT_SOURCE_DIR}/src/kernel/net/slnet_engine.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/net/slnet_engine.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/net/slnet_session.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/net/slnet_session.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/net/slnet_session_factory.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/net/slnet_session_factory.cpp"
)

SET(KERNEL
	"${PROJECT_SOURCE_DIR}/src/kernel/slkernel.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/slkernel.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/slmain.cpp"
)
SOURCE_GROUP(api FILES ${API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(src\\interface FILES ${KERNEL_INTERFACE})
SOURCE_GROUP(src\\net FILES ${KERNEL_NET})
SOURCE_GROUP(src\\sl\\api FILES ${KERNEL_SL_API})
SOURCE_GROUP(src\\sl\\public FILES ${KERNEL_SL_PUBLIC})
SOURCE_GROUP(src\\sl\\slnet FILES ${KERNEL_SL_NET})
SOURCE_GROUP(\\ FILES ${KERNEL})

IF(WIN32)
ADD_EXECUTABLE(shyloo
	${API}
	${PUBLIC}
	${KERNEL_INTERFACE}
	${KERNEL_NET}
	${KERNEL_SL_API}
	${KERNEL_SL_PUBLIC}
	${KERNEL_SL_NET}
	${KERNEL}
)
ENDIF(WIN32)

IF(WIN32)
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/windows")
ENDIF(WIN32)
TARGET_LINK_LIBRARIES(shyloo "ws2_32.lib" "Iphlpapi.lib")