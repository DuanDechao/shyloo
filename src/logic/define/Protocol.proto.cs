//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

// Option: missing-value detection (*Specified/ShouldSerialize*/Reset*) enabled
    
// Generated from: Protocol.proto
namespace Protocol
{
    [global::ProtoBuf.ProtoContract(Name=@"ErrorCode")]
    public enum ErrorCode
    {
            
      [global::ProtoBuf.ProtoEnum(Name=@"ERROR_NO_ERROR", Value=0)]
      ERROR_NO_ERROR = 0,
            
      [global::ProtoBuf.ProtoEnum(Name=@"ERROR_ACCOUNT_AUTHEN_FAILED", Value=1)]
      ERROR_ACCOUNT_AUTHEN_FAILED = 1,
            
      [global::ProtoBuf.ProtoEnum(Name=@"ERROR_GET_ROLE_LIST_FAILED", Value=2)]
      ERROR_GET_ROLE_LIST_FAILED = 2,
            
      [global::ProtoBuf.ProtoEnum(Name=@"ERROR_DISTRIBUTE_LOGIC_FAILED", Value=3)]
      ERROR_DISTRIBUTE_LOGIC_FAILED = 3,
            
      [global::ProtoBuf.ProtoEnum(Name=@"ERROR_LOAD_PLAYER_FAILED", Value=4)]
      ERROR_LOAD_PLAYER_FAILED = 4,
            
      [global::ProtoBuf.ProtoEnum(Name=@"ERROR_TOO_MUCH_ROLE", Value=5)]
      ERROR_TOO_MUCH_ROLE = 5,
            
      [global::ProtoBuf.ProtoEnum(Name=@"ERROR_CREATE_ROLE_FAILED", Value=6)]
      ERROR_CREATE_ROLE_FAILED = 6,
            
      [global::ProtoBuf.ProtoEnum(Name=@"ERROR_SYSTEM_ERROR", Value=7)]
      ERROR_SYSTEM_ERROR = 7
    }
  
}