/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

namespace cpp nebula.meta
namespace java com.vesoft.nebula.meta
namespace go nebula.meta

include "common.thrift"

/*
 *
 *  Note: In order to support multiple languages, all strings
 *        have to be defined as **binary** in the thrift file
 *
 */

typedef i64 (cpp.type = "nebula::SchemaVer") SchemaVer

typedef i32 (cpp.type = "nebula::UserID") UserID
typedef i64 (cpp.type = "nebula::ClusterID") ClusterID
typedef i32 (cpp.type = "nebula::TagIndexID") TagIndexID
typedef i32 (cpp.type = "nebula::EdgeIndexID") EdgeIndexID


enum ErrorCode {
    SUCCEEDED          = 0,

    // RPC Failure
    E_DISCONNECTED     = -1,
    E_FAIL_TO_CONNECT  = -2,
    E_RPC_FAILURE      = -3,

    E_LEADER_CHANGED   = -11,

    // Operation Failure
    E_NO_HOSTS         = -21,
    E_EXISTED          = -22,
    E_NOT_FOUND        = -23,
    E_INVALID_HOST     = -24,
    E_UNSUPPORTED      = -25,
    E_NOT_DROP         = -26,
    E_BALANCER_RUNNING = -27,
    E_CONFIG_IMMUTABLE = -28,
    E_CONFLICT         = -29,
    E_INVALID_PARM     = -30,
    E_WRONGCLUSTER     = -31,

    E_STORE_FAILURE             = -32,
    E_STORE_SEGMENT_ILLEGAL     = -33,
    E_BAD_BALANCE_PLAN          = -34,
    E_BALANCED                  = -35,
    E_NO_RUNNING_BALANCE_PLAN   = -36,
    E_NO_VALID_HOST             = -37,
    E_CORRUPTTED_BALANCE_PLAN   = -38,

    E_INVALID_PASSWORD       = -41,
    E_INPROPER_ROLE          = -42,

    E_SNAPSHOT_FAILURE   = -51;

    E_UNKNOWN        = -99,
} (cpp.enum_strict)


enum AlterSchemaOp {
    ADD    = 0x01,
    CHANGE = 0x02,
    DROP   = 0x03,
    UNKNOWN = 0x04,
} (cpp.enum_strict)

/*
 * GOD is A global senior administrator.like root of Linux systems.
 * ADMIN is an administrator for a given Graph Space.
 * USER is a normal user for a given Graph Space. A User can access (read and write)
 *      the data in the Graph Space.
 * GUEST is a read-only role for a given Graph Space. A Guest cannot modify the data
 *       in the Graph Space.
 *
 * Refer to header file src/graph/PermissionManager.h for details.
 */

enum RoleType {
    GOD    = 0x01,
    ADMIN  = 0x02,
    USER   = 0x03,
    GUEST  = 0x04,
} (cpp.enum_strict)


union ID {
    1: common.GraphSpaceID  space_id,
    2: common.TagID         tag_id,
    3: common.EdgeType      edge_type,
    4: TagIndexID           tag_index_id,
    5: EdgeIndexID          edge_index_id
    6: UserID               user_id,
    7: ClusterID            cluster_id,
}


// These are all data types supported in the graph properties
enum PropertyType {
    UNKNOWN = 0,

    // Simple types
    BOOL = 1,
    INT64 = 2,          // This is the same as INT in v1
    VID = 3,            // Deprecated, only supported by v1
    FLOAT = 4,
    DOUBLE = 5,
    STRING = 6,
    // String with fixed length. If the string content is shorteri
    // than the given length, '\0' will be padded to the end
    FIXED_STRING = 7,   // New in v2
    INT8 = 8,           // New in v2
    INT16 = 9,          // New in v2
    INT32 = 10,         // New in v2

    // Date time
    TIMESTAMP = 21,
    DATE = 24,
    DATETIME = 25,
} (cpp.enum_strict)


struct ColumnDef {
    1: required binary          name,
    2: required PropertyType    type,
    3: optional common.Value    default_value,
    // type_length is valid for fixed_string type
    4: optional i16             type_length = 0,
    5: optional bool            nullable = false,
}

struct SchemaProp {
    1: optional i64      ttl_duration,
    2: optional binary   ttl_col,
}

struct Schema {
    1: list<ColumnDef> columns,
    2: SchemaProp schema_prop,
}

struct IdName {
    1: ID     id,
    2: binary name,
}

struct SpaceProperties {
    1: binary   space_name,
    2: i32      partition_num,
    3: i32      replica_factor,
    4: i32      vid_size = 8,
}

struct SpaceItem {
    1: common.GraphSpaceID  space_id,
    2: SpaceProperties      properties,
}

struct TagItem {
    1: common.TagID     tag_id,
    2: binary           tag_name,
    3: SchemaVer        version,
    4: Schema           schema,
}

struct AlterSchemaItem {
    1: AlterSchemaOp    op,
    2: Schema           schema,
}

struct EdgeItem {
    1: common.EdgeType  edge_type,
    2: binary           edge_name,
    3: SchemaVer        version,
    4: Schema           schema,
}

struct IndexProperties {
    1: map<binary, list<binary>>(cpp.template = "std::map")  fields,
}

struct IndexFields {
    1: map<binary, list<ColumnDef>>(cpp.template = "std::map")  fields,
}

struct TagIndexItem {
    1: TagIndexID   index_id,
    2: binary       index_name,
    3: IndexFields  fields,
}

struct EdgeIndexItem {
    1: EdgeIndexID  index_id,
    2: binary       index_name,
    3: IndexFields  fields ,
}

enum HostStatus {
    ONLINE  = 0x00,
    OFFLINE = 0x01,
    UNKNOWN = 0x02,
} (cpp.enum_strict)

enum SnapshotStatus {
    VALID    = 0x00,
    INVALID  = 0x01,
} (cpp.enum_strict)

struct HostItem {
    1: common.HostAddr      hostAddr,
    2: HostStatus           status,
    3: map<binary, list<common.PartitionID>>
        (cpp.template = "std::unordered_map") leader_parts,
    4: map<binary, list<common.PartitionID>>
        (cpp.template = "std::unordered_map") all_parts,
}

struct ExecResp {
    1: ErrorCode        code,
    // For custom kv operations, it is useless.
    2: ID               id,
    // Valid if ret equals E_LEADER_CHANGED.
    3: common.HostAddr  leader,
}

// Graph space related operations.
struct CreateSpaceReq {
    1: SpaceProperties  properties,
    2: bool             if_not_exists,
}

struct DropSpaceReq {
    1: binary space_name
}

struct ListSpacesReq {
}

struct ListSpacesResp {
    1: ErrorCode code,
    // Valid if ret equals E_LEADER_CHANGED.
    2: common.HostAddr  leader,
    3: list<IdName> spaces,
}

struct GetSpaceReq {
    1: binary   space_name,
}

struct GetSpaceResp {
    1: ErrorCode         code,
    2: common.HostAddr   leader,
    3: SpaceItem         item,
}

// Tags related operations
struct CreateTagReq {
    1: common.GraphSpaceID  space_id,
    2: binary               tag_name,
    3: Schema               schema,
    4: bool                 if_not_exists,
}

struct AlterTagReq {
    1: common.GraphSpaceID      space_id,
    2: binary                   tag_name,
    3: list<AlterSchemaItem>    tag_items,
    4: SchemaProp               schema_prop,
}

struct DropTagReq {
    1: common.GraphSpaceID space_id,
    2: binary              tag_name,
}

struct ListTagsReq {
    1: common.GraphSpaceID space_id,
}

struct ListTagsResp {
    1: ErrorCode code,
    // Valid if ret equals E_LEADER_CHANGED.
    2: common.HostAddr  leader,
    3: list<TagItem> tags,
}

struct GetTagReq {
    1: common.GraphSpaceID  space_id,
    2: binary               tag_name,
    3: SchemaVer            version,
}

struct GetTagResp {
    1: ErrorCode        code,
    2: common.HostAddr  leader,
    3: Schema           schema,
}

// Edge related operations.
struct CreateEdgeReq {
    1: common.GraphSpaceID  space_id,
    2: binary               edge_name,
    3: Schema               schema,
    4: bool                 if_not_exists,
}

struct AlterEdgeReq {
    1: common.GraphSpaceID      space_id,
    2: binary                   edge_name,
    3: list<AlterSchemaItem>    edge_items,
    4: SchemaProp               schema_prop,
}

struct GetEdgeReq {
    1: common.GraphSpaceID  space_id,
    2: binary               edge_name,
    3: SchemaVer            version,
}

struct GetEdgeResp {
    1: ErrorCode        code,
    2: common.HostAddr  leader,
    3: Schema           schema,
}

struct DropEdgeReq {
    1: common.GraphSpaceID space_id,
    2: binary              edge_name,
}

struct ListEdgesReq {
    1: common.GraphSpaceID space_id,
}

struct ListEdgesResp {
    1: ErrorCode code,
    // Valid if ret equals E_LEADER_CHANGED.
    2: common.HostAddr  leader,
    3: list<EdgeItem> edges,
}

struct ListHostsReq {
}

struct ListHostsResp {
    1: ErrorCode code,
    // Valid if ret equals E_LEADER_CHANGED.
    2: common.HostAddr  leader,
    3: list<HostItem> hosts,
}

struct PartItem {
    1: required common.PartitionID       part_id,
    2: optional common.HostAddr          leader,
    3: required list<common.HostAddr>    peers,
    4: required list<common.HostAddr>    losts,
}

struct ListPartsReq {
    1: common.GraphSpaceID space_id,
}

struct ListPartsResp {
    1: ErrorCode code,
    2: common.HostAddr leader,
    3: list<PartItem> parts,
}

struct GetPartsAllocReq {
    1: common.GraphSpaceID space_id,
}

struct GetPartsAllocResp {
    1: ErrorCode code,
    // Valid if ret equals E_LEADER_CHANGED.
    2: common.HostAddr  leader,
    3: map<common.PartitionID, list<common.HostAddr>>(cpp.template = "std::unordered_map") parts,
}

struct MultiPutReq {
    // segment is used to avoid conflict with system data.
    // it should be comprised of numbers and letters.
    1: binary                   segment,
    2: list<common.KeyValue>    pairs,
}

struct GetReq {
    1: binary segment,
    2: binary key,
}

struct GetResp {
    1: ErrorCode        code,
    2: common.HostAddr  leader,
    3: binary           value,
}

struct MultiGetReq {
    1: binary       segment,
    2: list<binary> keys,
}

struct MultiGetResp {
    1: ErrorCode        code,
    2: common.HostAddr  leader,
    3: list<binary>     values,
}

struct RemoveReq {
    1: binary segment,
    2: binary key,
}

struct RemoveRangeReq {
    1: binary segment,
    2: binary start,
    3: binary end,
}

struct ScanReq {
    1: binary segment,
    2: binary start,
    3: binary end,
}

struct ScanResp {
    1: ErrorCode        code,
    2: common.HostAddr  leader,
    3: list<binary>     values,
}

struct HBResp {
    1: ErrorCode        code,
    2: common.HostAddr  leader,
    3: ClusterID        cluster_id,
}

struct HBReq {
    1: common.HostAddr  host,
    2: ClusterID        cluster_id,
}

struct CreateTagIndexReq {
    1: common.GraphSpaceID space_id,
    2: binary              index_name,
    3: IndexProperties     properties,
}

struct DropTagIndexReq {
    1: common.GraphSpaceID space_id,
    2: binary              index_name,
}

struct GetTagIndexReq {
    1: common.GraphSpaceID space_id,
    2: binary              index_name,
}

struct GetTagIndexResp {
    1: ErrorCode              code,
    2: common.HostAddr        leader,
    3: TagIndexItem           item,
}

struct ListTagIndexesReq {
    1: common.GraphSpaceID space_id,
}

struct ListTagIndexesResp {
    1: ErrorCode              code,
    2: common.HostAddr        leader,
    3: list<TagIndexItem>     items,
}

struct CreateEdgeIndexReq {
    1: common.GraphSpaceID space_id,
    2: binary              index_name,
    3: IndexProperties     properties,
}

struct DropEdgeIndexReq {
    1: common.GraphSpaceID space_id,
    2: binary              index_name,
}

struct GetEdgeIndexReq {
    1: common.GraphSpaceID space_id,
    2: binary              index_name,
}

struct GetEdgeIndexResp {
    1: ErrorCode              code,
    2: common.HostAddr        leader,
    3: EdgeIndexItem          item,
}

struct ListEdgeIndexesReq {
    1: common.GraphSpaceID space_id,
}

struct ListEdgeIndexesResp {
    1: ErrorCode              code,
    2: common.HostAddr        leader,
    3: list<EdgeIndexItem>    items,
}
struct CreateUserReq {
    1: common.UserItem      user_item,
    2: bool          if_not_exists,
}

struct DropUserReq {
    1: string account,
    2: bool   if_exists,
}

struct AlterUserReq {
    1: common.UserItem      user_item,
}

struct GrantRoleReq {
    1: common.RoleItem role_item,
}

struct RevokeRoleReq {
    1: common.RoleItem role_item,
}

struct GetUserReq {
    1: string account,
}

struct GetUserResp {
    1: ErrorCode code,
    // Valid if ret equals E_LEADER_CHANGED.
    2: common.HostAddr  leader,
    3: common.UserItem  user_item,
}

struct ListUsersReq {
}

struct ListUsersResp {
    1: ErrorCode code,
    // Valid if ret equals E_LEADER_CHANGED.
    2: common.HostAddr  leader,
    3: map<common.UserID, common.UserItem>(cpp.template = "std::unordered_map") users,
}

struct ListRolesReq {
    1: string   space,
}

struct ListRolesResp {
    1: ErrorCode code,
    // Valid if ret equals E_LEADER_CHANGED.
    2: common.HostAddr  leader,
    3: list<common.RoleItem> roles,
}

struct ChangePasswordReq {
    1: binary account,
    2: binary new_encoded_pwd,
    3: binary old_encoded_pwd,
}

struct BalanceReq {
    1: optional common.GraphSpaceID     space_id,
    // Specify the balance id to check the status of the related balance plan
    2: optional i64                     id,
    3: optional list<common.HostAddr>   host_del,
    4: optional bool                    stop,
}

enum TaskResult {
    SUCCEEDED  = 0x00,
    FAILED = 0x01,
    IN_PROGRESS = 0x02,
    INVALID = 0x03,
} (cpp.enum_strict)


struct BalanceTask {
    1: binary id,
    2: TaskResult result,
}

struct BalanceResp {
    1: ErrorCode        code,
    2: i64              id,
    // Valid if code equals E_LEADER_CHANGED.
    3: common.HostAddr  leader,
    4: list<BalanceTask> tasks,
}

struct LeaderBalanceReq {
}

enum ConfigModule {
    UNKNOWN = 0x00,
    ALL     = 0x01,
    GRAPH   = 0x02,
    META    = 0x03,
    STORAGE = 0x04,
} (cpp.enum_strict)

enum ConfigType {
    INT64   = 0x00,
    DOUBLE  = 0x01,
    BOOL    = 0x02,
    STRING  = 0x03,
    NESTED  = 0x04,
} (cpp.enum_strict)

enum ConfigMode {
    IMMUTABLE   = 0x00,
    REBOOT      = 0x01,
    MUTABLE     = 0x02,
    IGNORED     = 0x03,
} (cpp.enum_strict)

struct ConfigItem {
    1: ConfigModule         module,
    2: binary               name,
    3: ConfigType           type,
    4: ConfigMode           mode,
    5: binary               value,
}

struct RegConfigReq {
    1: list<ConfigItem>     items,
}

struct GetConfigReq {
    1: ConfigItem item,
}

struct GetConfigResp {
    1: ErrorCode            code,
    2: common.HostAddr      leader,
    3: list<ConfigItem>     items,
}

struct SetConfigReq {
    1: ConfigItem           item,
    2: bool                 force,
}

struct ListConfigsReq {
    1: binary               space,
    2: ConfigModule         module,
}

struct ListConfigsResp {
    1: ErrorCode            code,
    2: common.HostAddr      leader,
    3: list<ConfigItem>     items,
}

struct CreateSnapshotReq {
}

struct DropSnapshotReq {
    1: binary   name,
}

struct ListSnapshotsReq {
}

struct Snapshot {
    1: binary         name,
    2: SnapshotStatus status,
    3: binary         hosts,
}

struct ListSnapshotsResp {
    1: ErrorCode            code,
    // Valid if code equals E_LEADER_CHANGED.
    2: common.HostAddr      leader,
    3: list<Snapshot>       snapshots,
}

service MetaService {
    ExecResp createSpace(1: CreateSpaceReq req);
    ExecResp dropSpace(1: DropSpaceReq req);
    GetSpaceResp getSpace(1: GetSpaceReq req);
    ListSpacesResp listSpaces(1: ListSpacesReq req);

    ExecResp createTag(1: CreateTagReq req);
    ExecResp alterTag(1: AlterTagReq req);
    ExecResp dropTag(1: DropTagReq req);
    GetTagResp getTag(1: GetTagReq req);
    ListTagsResp listTags(1: ListTagsReq req);

    ExecResp createEdge(1: CreateEdgeReq req);
    ExecResp alterEdge(1: AlterEdgeReq req);
    ExecResp dropEdge(1: DropEdgeReq req);
    GetEdgeResp getEdge(1: GetEdgeReq req);
    ListEdgesResp listEdges(1: ListEdgesReq req);

    ListHostsResp listHosts(1: ListHostsReq req);

    GetPartsAllocResp getPartsAlloc(1: GetPartsAllocReq req);
    ListPartsResp listParts(1: ListPartsReq req);

    ExecResp multiPut(1: MultiPutReq req);
    GetResp get(1: GetReq req);
    MultiGetResp multiGet(1: MultiGetReq req);
    ExecResp remove(1: RemoveReq req);
    ExecResp removeRange(1: RemoveRangeReq req);
    ScanResp scan(1: ScanReq req);

    ExecResp             createTagIndex(1: CreateTagIndexReq req);
    ExecResp             dropTagIndex(1: DropTagIndexReq req );
    GetTagIndexResp      getTagIndex(1: GetTagIndexReq req);
    ListTagIndexesResp   listTagIndexes(1:ListTagIndexesReq req);
    ExecResp             createEdgeIndex(1: CreateEdgeIndexReq req);
    ExecResp             dropEdgeIndex(1: DropEdgeIndexReq req );
    GetEdgeIndexResp     getEdgeIndex(1: GetEdgeIndexReq req);
    ListEdgeIndexesResp  listEdgeIndexes(1: ListEdgeIndexesReq req);

    ExecResp createUser(1: CreateUserReq req);
    ExecResp dropUser(1: DropUserReq req);
    ExecResp alterUser(1: AlterUserReq req);
    ExecResp grantRole(1: GrantRoleReq req);
    ExecResp revokeRole(1: RevokeRoleReq req);
    GetUserResp getUser(1: GetUserReq req);
    ListUsersResp listUsers(1: ListUsersReq req);
    ListRolesResp listRoles(1: ListRolesReq req);
    ExecResp changePassword(1: ChangePasswordReq req);

    HBResp           heartBeat(1: HBReq req);
    BalanceResp      balance(1: BalanceReq req);
    ExecResp         leaderBalance(1: LeaderBalanceReq req);

    ExecResp regConfig(1: RegConfigReq req);
    GetConfigResp getConfig(1: GetConfigReq req);
    ExecResp setConfig(1: SetConfigReq req);
    ListConfigsResp listConfigs(1: ListConfigsReq req);

    ExecResp createSnapshot(1: CreateSnapshotReq req);
    ExecResp dropSnapshot(1: DropSnapshotReq req);
    ListSnapshotsResp listSnapshots(1: ListSnapshotsReq req);
}

