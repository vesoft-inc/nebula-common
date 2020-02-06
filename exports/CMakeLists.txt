# Copyright (c) 2020 vesoft inc. All rights reserved.
#
# This source code is licensed under Apache 2.0 License,
# attached with Common Clause Condition 1.0, found in the LICENSES directory.
#

export(
  TARGETS
    # base
    base_obj
    # clients
    graph_client_obj
    meta_client_obj
    file_based_cluster_id_man_obj
    graph_storage_client_obj
    general_storage_client_obj
    # concurrent
    concurrent_obj
    # conf
    conf_obj
    # datatypes
    datatypes_obj
    # fs
    fs_obj
    # hdfs
    hdfs_helper_obj
    # http
    http_client_obj
    # interface
    # meta
    meta_obj
    # network
    network_obj
    # process
    process_obj
    # stats
    stats_obj
    # thread
    thread_obj
    # thrift
    thrift_obj
    # time
    time_obj
    # webservice
    ws_common_obj
    ws_obj
  FILE nebula-common-exports.cmake
  )
