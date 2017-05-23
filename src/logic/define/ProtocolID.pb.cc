// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ProtocolID.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "ProtocolID.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace protobuf_ProtocolID_2eproto {


namespace {

const ::google::protobuf::EnumDescriptor* file_level_enum_descriptors[2];

}  // namespace

PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::ParseTableField
    const TableStruct::entries[] = {
  {0, 0, 0, ::google::protobuf::internal::kInvalidMask, 0, 0},
};

PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::AuxillaryParseTableField
    const TableStruct::aux[] = {
  ::google::protobuf::internal::AuxillaryParseTableField(),
};
PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::ParseTable const
    TableStruct::schema[] = {
  { NULL, NULL, 0, -1, -1, false },
};

const ::google::protobuf::uint32 TableStruct::offsets[] = { ~0u };
static const ::google::protobuf::internal::MigrationSchema* schemas = NULL;
static const ::google::protobuf::Message* const* file_default_instances = NULL;
namespace {

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "ProtocolID.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      NULL, file_level_enum_descriptors, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
}

}  // namespace

void TableStruct::Shutdown() {
}

void TableStruct::InitDefaultsImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::internal::InitProtobufDefaults();
}

void InitDefaults() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &TableStruct::InitDefaultsImpl);
}
void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] = {
      "\n\020ProtocolID.proto*\234\001\n\013ClientMsgID\022\030\n\024CL"
      "IENT_MSG_LOGIN_REQ\020\001\022\036\n\032CLIENT_MSG_SELEC"
      "T_ROLE_REQ\020\002\022\036\n\032CLIENT_MSG_CREATE_ROLE_R"
      "EQ\020\003\022\036\n\032CLIENT_MSG_ENTER_SCENE_REQ\020\004\022\023\n\017"
      "CLIENT_MSG_TEST\020\005*\306\001\n\013ServerMsgID\022\030\n\024SER"
      "VER_MSG_LOGIN_RSP\020\001\022\036\n\032SERVER_MSG_SELECT"
      "_ROLE_RSP\020\002\022\036\n\032SERVER_MSG_CREATE_ROLE_RS"
      "P\020\003\022\032\n\026SERVER_MSG_ATTRIB_SYNC\020\004\022\036\n\032SERVE"
      "R_MSG_NEW_ROLE_NOTIFY\020\005\022!\n\035SERVER_MSG_RE"
      "MOVE_ROLE_NOTIFY\020\006"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 378);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "ProtocolID.proto", &protobuf_RegisterTypes);
  ::google::protobuf::internal::OnShutdown(&TableStruct::Shutdown);
}

void AddDescriptors() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;

}  // namespace protobuf_ProtocolID_2eproto

const ::google::protobuf::EnumDescriptor* ClientMsgID_descriptor() {
  protobuf_ProtocolID_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_ProtocolID_2eproto::file_level_enum_descriptors[0];
}
bool ClientMsgID_IsValid(int value) {
  switch (value) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* ServerMsgID_descriptor() {
  protobuf_ProtocolID_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_ProtocolID_2eproto::file_level_enum_descriptors[1];
}
bool ServerMsgID_IsValid(int value) {
  switch (value) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
