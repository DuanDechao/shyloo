// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Protocol.proto

#ifndef PROTOBUF_Protocol_2eproto__INCLUDED
#define PROTOBUF_Protocol_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3003000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3003000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)
namespace protocol {
}  // namespace protocol

namespace protocol {

namespace protobuf_Protocol_2eproto {
// Internal implementation detail -- do not call these.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[];
  static const ::google::protobuf::uint32 offsets[];
  static void InitDefaultsImpl();
  static void Shutdown();
};
void AddDescriptors();
void InitDefaults();
}  // namespace protobuf_Protocol_2eproto

enum AttribType {
  DTYPE_INVAILED = 0,
  DTYPE_INT8 = 1,
  DTYPE_INT16 = 2,
  DTYPE_INT32 = 3,
  DTYPE_INT64 = 4,
  DTYPE_STRING = 5,
  DTYPE_FLOAT = 6,
  DTYPE_STRUCT = 7,
  DTYPE_BLOB = 8
};
bool AttribType_IsValid(int value);
const AttribType AttribType_MIN = DTYPE_INVAILED;
const AttribType AttribType_MAX = DTYPE_BLOB;
const int AttribType_ARRAYSIZE = AttribType_MAX + 1;

const ::google::protobuf::EnumDescriptor* AttribType_descriptor();
inline const ::std::string& AttribType_Name(AttribType value) {
  return ::google::protobuf::internal::NameOfEnum(
    AttribType_descriptor(), value);
}
inline bool AttribType_Parse(
    const ::std::string& name, AttribType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<AttribType>(
    AttribType_descriptor(), name, value);
}
enum ObjectType {
  OBJECT_TYPE_UNKNOWN = 0,
  OBJECT_TYPE_PLAYER = 1
};
bool ObjectType_IsValid(int value);
const ObjectType ObjectType_MIN = OBJECT_TYPE_UNKNOWN;
const ObjectType ObjectType_MAX = OBJECT_TYPE_PLAYER;
const int ObjectType_ARRAYSIZE = ObjectType_MAX + 1;

const ::google::protobuf::EnumDescriptor* ObjectType_descriptor();
inline const ::std::string& ObjectType_Name(ObjectType value) {
  return ::google::protobuf::internal::NameOfEnum(
    ObjectType_descriptor(), value);
}
inline bool ObjectType_Parse(
    const ::std::string& name, ObjectType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ObjectType>(
    ObjectType_descriptor(), name, value);
}
enum ErrorCode {
  ERROR_NO_ERROR = 0,
  ERROR_ACCOUNT_AUTHEN_FAILED = 1,
  ERROR_GET_ROLE_LIST_FAILED = 2,
  ERROR_DISTRIBUTE_LOGIC_FAILED = 3,
  ERROR_LOAD_PLAYER_FAILED = 4,
  ERROR_TOO_MUCH_ROLE = 5,
  ERROR_CREATE_ROLE_FAILED = 6,
  ERROR_SYSTEM_ERROR = 7,
  ERROR_LOGIN_CHECK_TICKET_FAILED = 8
};
bool ErrorCode_IsValid(int value);
const ErrorCode ErrorCode_MIN = ERROR_NO_ERROR;
const ErrorCode ErrorCode_MAX = ERROR_LOGIN_CHECK_TICKET_FAILED;
const int ErrorCode_ARRAYSIZE = ErrorCode_MAX + 1;

const ::google::protobuf::EnumDescriptor* ErrorCode_descriptor();
inline const ::std::string& ErrorCode_Name(ErrorCode value) {
  return ::google::protobuf::internal::NameOfEnum(
    ErrorCode_descriptor(), value);
}
inline bool ErrorCode_Parse(
    const ::std::string& name, ErrorCode* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ErrorCode>(
    ErrorCode_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)


}  // namespace protocol

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::protocol::AttribType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::protocol::AttribType>() {
  return ::protocol::AttribType_descriptor();
}
template <> struct is_proto_enum< ::protocol::ObjectType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::protocol::ObjectType>() {
  return ::protocol::ObjectType_descriptor();
}
template <> struct is_proto_enum< ::protocol::ErrorCode> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::protocol::ErrorCode>() {
  return ::protocol::ErrorCode_descriptor();
}

}  // namespace protobuf
}  // namespace google
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_Protocol_2eproto__INCLUDED
