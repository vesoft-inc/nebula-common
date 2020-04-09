/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

namespace cpp nebula.storage
namespace java com.vesoft.nebula.storage
namespace go nebula.storage

include "common.thrift"
include "meta.thrift"

/*
 *
 *  Note: In order to support multiple languages, all strings
 *        have to be defined as **binary** in the thrift file
 *
 */

enum ErrorCode {
    SUCCEEDED = 0,

    // RPC Failure
    E_DISCONNECTED = -1,
    E_FAILED_TO_CONNECT = -2,
    E_RPC_FAILURE = -3,

    // storage failures
    E_LEADER_CHANGED = -11,
    E_KEY_HAS_EXISTS = -12,
    E_SPACE_NOT_FOUND = -13,
    E_PART_NOT_FOUND = -14,
    E_KEY_NOT_FOUND = -15,
    E_CONSENSUS_ERROR = -16,

    // meta failures
    E_EDGE_PROP_NOT_FOUND = -21,
    E_TAG_PROP_NOT_FOUND = -22,
    E_IMPROPER_DATA_TYPE = -23,
    E_EDGE_NOT_FOUND = -24,
    E_TAG_NOT_FOUND = -25,

    // Invalid request
    E_INVALID_FILTER = -31,
    E_INVALID_UPDATER = -32,
    E_INVALID_STORE = -33,
    E_INVALID_PEER  = -34,
    E_RETRY_EXHAUSTED = -35,
    E_TRANSFER_LEADER_FAILED = -36,

    // meta client failed
    E_LOAD_META_FAILED = -41,

    // checkpoint failed
    E_FAILED_TO_CHECKPOINT = -50,
    E_CHECKPOINT_BLOCKED = -51,

    E_UNKNOWN = -100,
} (cpp.enum_strict)


struct PartitionResult {
    1: required ErrorCode           code,
    2: required common.PartitionID  part_id,
    // Only valid when code is E_LEADER_CHANAGED.
    3: optional common.HostAddr     leader,
}


struct ResponseCommon {
    // Only contains the partition that returns error
    1: required list<PartitionResult>   failed_parts,
    // Query latency from storage service
    2: required i32                     latency_in_us,
}


/*
 *
 * Common types used by all services
 *
 */
// Define a vertex property
struct VertexProp {
    // This is the ID of the tag which the foloowing property name belongs to.
    // If the property name is "*", all properties of the tag will be returned.
    // If the tag ID is 0, the specified property on all tags will be returned
    // in the same column. If the tag ID is 0 and the property name is "*", then
    // all properties on all tags will be returned
    1: common.TagID tag,    // Tag ID
    // When name is "*", it means to return all properties
    2: binary       name,   // Property name
}


// Define an edge property
struct EdgeProp {
    // This is the edge type which the foloowing property name belongs to.
    // If the property name is "*", all properties of the edge will be returned.
    // If the edge type is 0, the specified property on all edges will be returned
    // in the same column. If the edge type is 0 and the property name is "*", then
    // all properties on all edges will be returned
    1: common.EdgeType  type,   // Edge type
    // When name is "*", it means to return all properties
    2: binary           name,   // Property name
}


// Enumeration of the statistic methods
enum StatType {
    SUM = 1,
    COUNT = 2,
    AVG = 3,
} (cpp.enum_strict)


// Define a statistic properties
struct StatProp {
    1: common.EdgeType  type,   // Edge type
    2: binary           name,   // Property name
    3: StatType         stat,   // Stats method
}


///////////////////////////////////////////////////////////
//
//  Requests, responses for the GraphStorageService
//
///////////////////////////////////////////////////////////

/*
 * Start of GetNeighbors section
 */
struct GetNeighborsRequest {
    1: common.GraphSpaceID space_id,
    // partId => ids
    2: map<common.PartitionID, list<common.VertexID>>(
        cpp.template = "std::unordered_map") parts,
    // When edge_type > 0, going along the out-edge, otherwise, along the in-edge
    // If the edge type list is empty, all edges will be scaned
    3: list<common.EdgeType> edge_types,
    // If there is no property specified, all properties will be returned
    4: list<VertexProp> vertex_props,
    5: list<StatProp> stat_props,
    6: list<EdgeProp> edge_props,
    7: optional binary filter,
}


struct GetNeighborsResponse {
    1: required ResponseCommon result,
    // The result will be returned in a dataset, which is in the following form
    //
    // Each row represents one source vertex and its neighbors
    // The name of the first column is "_vid", it's the ID of the source vertex
    // The name of the second column is "_stats", it's the list of Values for
    //   the stats properties, in the order specified as GetNeighborsRequest::stat_props.
    //   If there is no stat_props specified, it will be NULL
    // Starting from the third column, it's the vertex property, one column per tag.
    //   The column name is the Tag name. The value is a DataSet. If the vertex does
    //   NOT have the given tag, the value will be a NULL
    // The columns following the vertex properties are edges, one column per edge type.
    //   The column name is the edge type name. The value is a DataSet. The columns in
    //   the DataSet reflect the properties specified in the
    //   GetNeighborsRequest::edge_props. If the edge_props is empty, all properties
    //   will be returned
    2: optional common.DataSet vertices,
}
/*
 * End of GetNeighbors section
 */


//
// Response for data modification requests
//
struct ExecResponse {
    1: required ResponseCommon result,
}


/*
 * Start of GetVertexProp section
 */
struct VertexPropRequest {
    1: common.GraphSpaceID                      space_id,
    2: map<common.PartitionID, list<common.VertexID>>(
        cpp.template = "std::unordered_map")    parts,
    // If the property list is empty, return all properties
    3: list<VertexProp>                         vertex_props,
    // If a filter is provided, only vertices that are satisfied the filter
    // will be returned
    4: optional binary                          filter,
}


struct VertexPropResponse {
    1: ResponseCommon       result,
    // The result will be returned in a dataset, which is in the following form
    //
    // Each row represents one vertex and its properties
    // The name of the first column is "_vid", it's the ID of the vertex
    // Starting from the second column, it's the vertex property, one column per
    //   peoperty. The column name is in the form of "tag_name.prop_name".
    //   If the vertex does NOT have the given property, the value will be a NULL
    2: optional common.DataSet     vertices,
}
/*
 * End of GetVertexProp section
 */


/*
 * Start of GetEdgeProp section
 */
struct EdgeKey {
    1: common.VertexID      src,
    // When edge_type > 0, it's an out-edge, otherwise, it's an in-edge
    // When query edge props, the field could be unset.
    2: common.EdgeType      edge_type,
    3: common.EdgeRanking   ranking,
    4: common.VertexID      dst,
}


struct EdgePropRequest {
    1: common.GraphSpaceID                      space_id,
    // partId => edges
    2: map<common.PartitionID, list<EdgeKey>>(
        cpp.template = "std::unordered_map")    parts,
    // If the property list is empty, return all properties
    3: list<EdgeProp>                           edge_props,
    // If a filter is provided, only edges that are satisfied the filter
    // will be returned
    4: optional binary                          filter,
}


struct EdgePropResponse {
    1: ResponseCommon       result,
    // The result will be returned in a dataset, which is in the following form
    //
    // Each row represents one edge and its properties
    // The name of the first column is "_src", it's the ID of the source vertex
    // The name of the second column is "_type", it's the edge type
    // The name of the third column is "_ranking", it's the edge ranking
    // The name of the fource column is "_dst", it's the ID of the destination
    //   vertex
    // Starting from the fifth column, it's the edge property, one column per
    //   peoperty. The column name is in the form of "edge_type_name.prop_name".
    //   If the vertex does NOT have the given property, the value will be a NULL
    2: optional common.DataSet     edges,
}
/*
 * End of GetEdgeProp section
 */


/*
 * Start of AddVertices section
 */
struct NewTag {
    1: common.TagID             tag_id,
    2: list<common.Value>       props,
    // If the name list is empty, the order of the properties has to be
    // exactly same as that the schema defines
    3: optional list<binary>    names,
}


struct NewVertex {
    1: common.VertexID id,
    2: list<NewTag> tags,
}


struct NewEdge {
    1: EdgeKey                  key,
    2: list<common.Value>       props,
    // If the name list is empty, the order of the properties has to be
    // exactly same as that the schema defines
    3: optional list<binary>    names,
}


struct AddVerticesRequest {
    1: common.GraphSpaceID space_id,
    // partId => vertices
    2: map<common.PartitionID, list<NewVertex>>(
        cpp.template = "std::unordered_map") parts,
    // If true, it equals an (up)sert operation.
    3: bool overwritable = true,
}

struct AddEdgesRequest {
    1: common.GraphSpaceID space_id,
    // partId => edges
    2: map<common.PartitionID, list<NewEdge>>(
        cpp.template = "std::unordered_map") parts,
    // If true, it equals an upsert operation.
    3: bool overwritable = true,
}
/*
 * End of AddVertices section
 */


/*
 * Start of DeleteVertex section
 */
struct DeleteVerticesRequest {
    1: common.GraphSpaceID space_id,
    // partId => vertexId
    2: map<common.PartitionID, list<common.VertexID>>(
        cpp.template = "std::unordered_map") parts,
}


struct DeleteEdgesRequest {
    1: common.GraphSpaceID space_id,
    // partId => edgeKeys
    2: map<common.PartitionID, list<EdgeKey>>(
        cpp.template = "std::unordered_map") parts,
}
/*
 * End of DeleteVertex section
 */


// Response for update requests
struct UpdateResponse {
    1: required ResponseCommon  result,
    // The result will be returned in a dataset, which is in the following form
    //
    // The name of the first column is "_inserted". It has a boolean value. It's
    //   TRUE if insertion happens
    // Starting from the second column, it's the all returned properties, one column
    //   per peoperty. If there is no given property, the value will be a NULL
    2: optional common.DataSet         props,
}


/*
 * Start of UpdateVertex section
 */
struct UpdatedVertexProp {
    1: required common.TagID    tag_id,     // the Tag ID
    2: required binary          name,       // property name
    3: required common.Value    value,      // new value
}


struct UpdateVertexRequest {
    1: common.GraphSpaceID          space_id,
    2: common.PartitionID           part_id,
    3: common.VertexID              vertex_id,
    4: list<UpdatedVertexProp>      updated_props,
    5: optional bool                insertable = false,
    6: optional list<VertexProp>    return_props,
    // If provided, the update happens only when the condition evaluates true
    7: optional binary              condition,
}
/*
 * End of UpdateVertex section
 */


/*
 * Start of UpdateEdge section
 */
struct UpdatedEdgeProp {
    1: required binary          name,       // property name
    2: required common.Value    value,      // new value
}


struct UpdateEdgeRequest {
    1: common.GraphSpaceID      space_id,
    2: common.PartitionID       part_id,
    3: EdgeKey                  edge_key,
    4: list<UpdatedEdgeProp>    updated_props,
    5: optional bool            insertable = false,
    6: optional list<EdgeProp>  return_props,
    // If provided, the update happens only when the condition evaluates true
    7: optional binary          condition,
}
/*
 * End of UpdateEdge section
 */


/*
 * Start of GetUUID section
 */
struct GetUUIDReq {
    1: common.GraphSpaceID  space_id,
    2: common.PartitionID   part_id,
    3: binary               name,
}


struct GetUUIDResp {
    1: required ResponseCommon result,
    2: common.VertexID id,
}
/*
 * End of GetUUID section
 */


/*
 * Start of Index section
 */
struct LookUpVertexIndexResp {
    1: required ResponseCommon          result,
    // The result will be returned in a dataset, which is in the following form
    //
    // Each row represents one vertex and its properties
    // The name of the first column is "_vid", it's the ID of the vertex
    // Starting from the second column, it's the vertex property, one column per
    //   peoperty. The column name is in the form of "tag_name.prop_name".
    //   If the vertex does NOT have the given property, the value will be a NULL
    2: optional common.DataSet     vertices,
}

struct LookUpEdgeIndexResp {
    1: required ResponseCommon          result,
    // The result will be returned in a dataset, which is in the following form
    //
    // Each row represents one edge and its properties
    // The name of the first column is "_src", it's the ID of the source vertex
    // The name of the second column is "_type", it's the edge type
    // The name of the third column is "_ranking", it's the edge ranking
    // The name of the fource column is "_dst", it's the ID of the destination
    //   vertex
    // Starting from the fifth column, it's the edge property, one column per
    //   peoperty. The column name is in the form of "edge_type_name.prop_name".
    //   If the vertex does NOT have the given property, the value will be a NULL
    2: optional common.DataSet                 edges,
}

struct LookUpIndexRequest {
    1: common.GraphSpaceID      space_id,
    2: list<common.PartitionID> parts,
    3: common.IndexID           index_id,
    4: binary                   filter,
    // We only support specified fields here, not wild card "*"
    // If the list is empty, only the VertexID or the EdgeKey will
    // be returned
    5: list<binary>             return_columns,
}
/*
 * End of GetUUID section
 */


service GraphStorageService {
    GetNeighborsResponse getNeighbors(1: GetNeighborsRequest req)

    // Get vertex properties, can also filter out the vertices
    VertexPropResponse getVertexProps(1: VertexPropRequest req);
    // Get edge properties, can also filter out the edges
    EdgePropResponse getEdgeProps(1: EdgePropRequest req)

    ExecResponse addVertices(1: AddVerticesRequest req);
    ExecResponse addEdges(1: AddEdgesRequest req);

    ExecResponse deleteEdges(1: DeleteEdgesRequest req);
    ExecResponse deleteVertices(1: DeleteVerticesRequest req);

    UpdateResponse updateVertex(1: UpdateVertexRequest req);
    UpdateResponse updateEdge(1: UpdateEdgeRequest req);

    GetUUIDResp getUUID(1: GetUUIDReq req);

    // Interfaces for edge and vertex index scan
    LookUpVertexIndexResp lookUpVertexIndex(1: LookUpIndexRequest req);
    LookUpEdgeIndexResp   lookUpEdgeIndex(1: LookUpIndexRequest req);
}


//////////////////////////////////////////////////////////
//
//  Requests, responses for the StorageAdminService
//
//////////////////////////////////////////////////////////
// Common response for admin methods
struct AdminExecResp {
    1: required ResponseCommon result,
}


struct TransLeaderReq {
    1: common.GraphSpaceID space_id,
    2: common.PartitionID  part_id,
    3: common.HostAddr     new_leader,
}


struct AddPartReq {
    1: common.GraphSpaceID space_id,
    2: common.PartitionID  part_id,
    3: bool                as_learner,
}


struct AddLearnerReq {
    1: common.GraphSpaceID space_id,
    2: common.PartitionID  part_id,
    3: common.HostAddr     learner,
}


struct RemovePartReq {
    1: common.GraphSpaceID space_id,
    2: common.PartitionID  part_id,
}


struct MemberChangeReq {
    1: common.GraphSpaceID space_id,
    2: common.PartitionID  part_id,
    3: common.HostAddr     peer,
    // true means add a peer, false means remove a peer.
    4: bool                add,
}


struct CatchUpDataReq {
    1: common.GraphSpaceID space_id,
    2: common.PartitionID  part_id,
    3: common.HostAddr     target,
}


struct CreateCPRequest {
    1: common.GraphSpaceID  space_id,
    2: binary               name,
}


struct DropCPRequest {
    1: common.GraphSpaceID  space_id,
    2: binary               name,
}


enum EngineSignType {
    BLOCK_ON = 1,
    BLOCK_OFF = 2,
}


struct BlockingSignRequest {
    1: common.GraphSpaceID      space_id,
    2: required EngineSignType  sign,
}


struct GetLeaderPartsResp {
    1: required ResponseCommon result,
    2: map<common.GraphSpaceID, list<common.PartitionID>> (
        cpp.template = "std::unordered_map") leader_parts;
}


struct CheckPeersReq {
    1: common.GraphSpaceID space_id,
    2: common.PartitionID  part_id,
    3: list<common.HostAddr> peers,
}


//struct RebuildIndexRequest {
//    1: common.GraphSpaceID          space_id,
//    2: list<common.PartitionID>     parts,
//    3: common.IndexID               index_id,
//    4: bool                         is_offline,
//}


service StorageAdminService {
    // Interfaces for admin operations
    AdminExecResp transLeader(1: TransLeaderReq req);
    AdminExecResp addPart(1: AddPartReq req);
    AdminExecResp addLearner(1: AddLearnerReq req);
    AdminExecResp removePart(1: RemovePartReq req);
    AdminExecResp memberChange(1: MemberChangeReq req);
    AdminExecResp waitingForCatchUpData(1: CatchUpDataReq req);

    // Interfaces for nebula cluster checkpoint
    AdminExecResp createCheckpoint(1: CreateCPRequest req);
    AdminExecResp dropCheckpoint(1: DropCPRequest req);
    AdminExecResp blockingWrites(1: BlockingSignRequest req);

    // Interfaces for rebuild index
//    AdminExecResp rebuildTagIndex(1: RebuildIndexRequest req);
//    AdminExecResp rebuildEdgeIndex(1: RebuildIndexRequest req);

    // Return all leader partitions on this host
    GetLeaderPartsResp getLeaderParts();
    // Return all peers
    AdminExecResp checkPeers(1: CheckPeersReq req);
}


//////////////////////////////////////////////////////////
//
//  Requests, responses for the GeneralStorageService
//
//////////////////////////////////////////////////////////
struct KVGetRequest {
    1: common.GraphSpaceID space_id,
    2: map<common.PartitionID, list<binary>>(
        cpp.template = "std::unordered_map") parts,
    // When return_partly is true and some of the keys not found, will return the keys
    // which exist
    3: bool return_partly
}


struct KVGetResponse {
    1: required ResponseCommon result,
    2: map<binary, binary>(cpp.template = "std::unordered_map") key_values,
}


struct KVPutRequest {
    1: common.GraphSpaceID space_id,
    // part -> key/value
    2: map<common.PartitionID, list<common.KeyValue>>(
        cpp.template = "std::unordered_map") parts,
}


struct KVRemoveRequest {
    1: common.GraphSpaceID space_id,
    // part -> key
    2: map<common.PartitionID, list<binary>>(
        cpp.template = "std::unordered_map") parts,
}


struct KeyRange {
    1: binary start,
    2: binary end,
}


struct KVRemoveRangeRequest {
    1: common.GraphSpaceID space_id,
    2: map<common.PartitionID, list<KeyRange>>(
        cpp.template = "std::unordered_map") parts,
}


service GeneralStorageService {
    // Interfaces for key-value storage
    KVGetResponse   get(1: KVGetRequest req);
    ExecResponse    put(1: KVPutRequest req);
    ExecResponse    remove(1: KVRemoveRequest req);
    ExecResponse    removeRange(1: KVRemoveRangeRequest req);
}

