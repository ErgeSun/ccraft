/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <gflags/gflags.h>

/**
 * glog配置
 */
DEFINE_string(glog_dir, "/tmp/ccraft/logs", "glog的保存路径");
DEFINE_bool(glog_prefix, true, "设置日志前缀是否应该添加到每行输出");
DEFINE_int32(max_glog_size, 100, "设置最大日志文件大小（以MB为单位）");
DEFINE_int32(glogbufsecs, 0, "设置可以缓冲日志的最大秒数，0指实时输出");
DEFINE_int32(mingloglevel, 0, "设置log输出级别(包含)：INFO = 0, WARNING = 1, ERROR = 2,  FATAL = 3");
DEFINE_bool(stop_glogging_if_full_disk, true, "设置是否在磁盘已满时避免日志记录到磁盘");
DEFINE_bool(glogtostderr, false, "设置日志消息是否转到标准输出而不是日志文件");
DEFINE_bool(alsoglogtostderr, false, "设置日志消息除了日志文件之外是否去标准输出");
DEFINE_bool(colorglogtostderr, true, "设置记录到标准输出的颜色消息（如果终端支持）");

/**
 * net common
 */
DEFINE_int32(net_server_handshake_threads_cnt, 16, "网络框架服务端处理握手的线程个数");
DEFINE_int32(net_server_connect_timeout, 100, "tcp连接超时时间。单位ms。");

/**
 * rpc server common
 */
DEFINE_int32(internal_rpc_client_threads_cnt, 0, "设置用于rpc client处理任务的线程池线程个数。默认0为cpu逻辑核数的2倍。");
DEFINE_int32(internal_rpc_client_wait_timeout_secs, 10, "设置用rpc client等待请求的最大时长的秒数部分。");
DEFINE_int64(internal_rpc_client_wait_timeout_nsecs, 0, "设置用于rpc client等待请求的最大时长的纳秒数部分，与秒数共同组成等待时长。");
DEFINE_int32(internal_rpc_server_threads_cnt, 0, "设置用于rpc server处理任务的线程池线程个数。默认0为cpu逻辑核数的2倍。");
DEFINE_int32(internal_rpc_messenger_threads_cnt, 0, "设置用于internal rpc处理的信使的消息分发的线程数目。默认0为cpu逻辑核数的2倍。");
DEFINE_int32(internal_rpc_io_threads_cnt, 0, "设置用于node间通信的rpc服务的网络io处理线程个数。默认0为cpu逻辑核数的一半。");

/**
 * rf node
 */
DEFINE_int32(server_id, 0, "设置server id，大于0的整数，应包含在conf/servers的第一列中。");
DEFINE_string(data_dir, "/tmp/ccraft", "数据存放目录。");
DEFINE_string(rflogger_type, "realtime-disk", "rflogger类型。");
DEFINE_int32(raft_election_interval, 30, "raft leader超时的时间间隔，单位:second。");
DEFINE_double(raft_election_timeout_percent, 0.8, "raft发出选举到成功之前的超时时间，为百分比，通过乘以raft_election_interval得到最终结果。");
DEFINE_string(servers_conf_path, "/tmp/ccraft/conf/servers", "集群server地址信息。");
DEFINE_int32(leader_hb_timeout_duration, 30, "选举超时时间。即如果这段时间内没有收到leader的心跳则发起选举。单位:second。");
DEFINE_int32(start_election_rand_latency_low, 200, "开始选举的随机延迟时间的下限。单位:ms。");
DEFINE_int32(start_election_rand_latency_high, 500, "开始选举的随机延迟时间的上限。单位:ms。");
