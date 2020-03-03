/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */


namespace cpp nebula
namespace java com.vesoft.nebula
namespace go nebula

cpp_include "thrift/ThriftTypes.h"
cpp_include "datatypes/DateOps.h"
cpp_include "datatypes/PathOps.h"
cpp_include "datatypes/ValueOps.h"
cpp_include "datatypes/MapOps.h"
cpp_include "datatypes/ListOps.h"
cpp_include "datatypes/KeyValueOps.h"
cpp_include "datatypes/HostAddrOps.h"

/*
 *
 *  Note: In order to support multiple languages, all strings
 *        have to be defined as **binary** in the thrift file
 *
 */

typedef i32 (cpp.type = "nebula::GraphSpaceID") GraphSpaceID
typedef i32 (cpp.type = "nebula::PartitionID") PartitionID
typedef i32 (cpp.type = "nebula::TagID") TagID
typedef i32 (cpp.type = "nebula::EdgeType") EdgeType
typedef i64 (cpp.type = "nebula::EdgeRanking") EdgeRanking
typedef binary (cpp.type = "nebula::VertexID") VertexID

typedef i64 (cpp.type = "nebula::Timestamp") Timestamp

typedef i32 (cpp.type = "nebula::IPv4") IPv4
typedef i32 (cpp.type = "nebula::Port") Port

typedef i32 (cpp.type = "nebula::UserID") UserID

// !! Struct Date has a shadow data type defined in the ThriftTypes.h
// So any change here needs to be reflected to the shadow type there
struct Date {
    1: i16 year;    // Calendar year, such as 2019
    2: byte month;    // Calendar month: 1 - 12
    3: byte day;      // Calendar day: 1 -31
} (cpp.type = "nebula::Date")


// !! Struct DateTime has a shadow data type defined in the ThriftTypes.h
// So any change here needs to be reflected to the shadow type there
struct DateTime {
    1: i16 year;
    2: byte month;
    3: byte day;
    4: byte hour;         // Hour: 0 - 23
    5: byte minute;       // Minute: 0 - 59
    6: byte sec;          // Second: 0 - 59
    7: i32 microsec;    // Micro-second: 0 - 999,999
    8: i32 timezone;    // Time difference in seconds
} (cpp.type = "nebula::DateTime")


struct Step {
    1: EdgeType type;
    2: EdgeRanking ranking;
    3: VertexID dst;
} (cpp.type = "nebula::Step")


// Special type to support path during the query
struct Path {
    1: VertexID src;
    2: list<Step> steps;
} (cpp.type = "nebula::Path")


enum NullType {
    __NULL__ = 0,
    NaN      = 1,
    BAD_DATA = 2,
    BAD_TYPE = 3,
} (cpp.enum_strict cpp.type = "nebula::NullType")


// The type to hold any supported values during the query
union Value {
    1: NullType                             nVal;
    2: bool                                 bVal;
    3: i64                                  iVal;
    4: double                               fVal;
    5: binary                               sVal;
    6: Date                                 dVal;
    7: DateTime                             tVal;
    8: Path                                 pVal;
    9: List (cpp.type = "nebula::List")     lVal (cpp.ref_type = "unique");
    10: Map (cpp.type = "nebula::Map")      mVal (cpp.ref_type = "unique");
} (cpp.type = "nebula::Value")


struct List {
    1: list<Value> values;
} (cpp.type = "nebula::List")


struct Map {
    1: map<binary, Value> (cpp.template = "std::unordered_map") kvs;
} (cpp.type = "nebula::Map")


struct HostAddr {
    1: IPv4  ip,
    2: Port  port,
} (cpp.type = "nebula::HostAddr")


struct KeyValue {
    1: binary key,
    2: binary value,
} (cpp.type = "nebula::KeyValue")


/**
** GOD is A global senior administrator.like root of Linux systems.
** ADMIN is an administrator for a given Graph Space.
** DBA is an schema administrator for a given Graph Space.
** USER is a normal user for a given Graph Space. A User can access (read and write) the data in the Graph Space.
** GUEST is a read-only role for a given Graph Space. A Guest cannot modify the data in the Graph Space.
** Refer to header file src/graph/PermissionManager.h for details.
**/

enum RoleType {
    GOD    = 0x01,
    ADMIN  = 0x02,
    DBA    = 0x03,
    USER   = 0x04,
    GUEST  = 0x05,
} (cpp.enum_strict)

enum UserLoginType {
   PASSWORD = 0x01,
   LDAP     = 0x02,
}

struct RoleItem {
    1: string        user,
    2: string        space,
    3: RoleType      role_type,
}

struct UserItem {
    1: required string        account;
    // Disable user if lock status is true.
    2: optional bool          is_lock,
    // user login type
    3: optional UserLoginType login_type,
    // encoded password
    4: optional string        encoded_pwd,
    // The number of queries an account can issue per hour
    5: optional i32           max_queries_per_hour,
    // The number of updates an account can issue per hour
    6: optional i32           max_updates_per_hour,
    // The number of times an account can connect to the server per hour
    7: optional i32           max_connections_per_hour,
    // The number of simultaneous connections to the server by an account
    8: optional i32           max_user_connections,
}
