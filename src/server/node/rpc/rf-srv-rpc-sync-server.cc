/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../../common/server-gflags-config.h"
#include "../../../rpc/rpc-handler.h"
#include "../../../codegen/node-raft.pb.h"

#include "../inode-internal-rpc-handler.h"
#include "common-def.h"

#include "rf-srv-rpc-sync-server.h"

namespace ccraft {
namespace server {
RfSrvInternalRpcServerSync::RfSrvInternalRpcServerSync(INodeInternalRpcHandler *handler, uint16_t workThreadsCnt,
                                                         net::ISocketService *ss, ccsys::MemPool *memPool) :
            m_pHandler(handler) {
    CHECK(handler);
    m_pRpcServer = new rpc::RpcServer(workThreadsCnt, ss, memPool);
}

RfSrvInternalRpcServerSync::~RfSrvInternalRpcServerSync() {
    DELETE_PTR(m_pRpcServer);
}

bool RfSrvInternalRpcServerSync::Start() {
    register_rpc_handlers();

    return m_pRpcServer->Start();
}

bool RfSrvInternalRpcServerSync::Stop() {
    return m_pRpcServer->Stop();
}

void RfSrvInternalRpcServerSync::HandleMessage(std::shared_ptr<net::NotifyMessage> sspNM) {
    m_pRpcServer->HandleMessage(sspNM);
}

void RfSrvInternalRpcServerSync::register_rpc_handlers() {
    // internal communication
    auto appendEntriesHandler = new rpc::TypicalRpcHandler(std::bind(&RfSrvInternalRpcServerSync::on_append_entries, this, std::placeholders::_1),
                                              std::bind(&RfSrvInternalRpcServerSync::create_append_entries_request, this));
    m_pRpcServer->RegisterRpc(APPEND_ENTRIES_RPC_ID, appendEntriesHandler);
    auto requestVoteHandler = new rpc::TypicalRpcHandler(std::bind(&RfSrvInternalRpcServerSync::on_request_vote, this, std::placeholders::_1),
                                              std::bind(&RfSrvInternalRpcServerSync::create_request_vote_request, this));
    m_pRpcServer->RegisterRpc(REQUEST_VOTE_RPC_ID, requestVoteHandler);

    m_pRpcServer->FinishRegisterRpc();
}

rpc::SP_PB_MSG RfSrvInternalRpcServerSync::on_append_entries(rpc::SP_PB_MSG sspMsg) {
    return m_pHandler->OnAppendEntries(sspMsg);
}

rpc::SP_PB_MSG RfSrvInternalRpcServerSync::create_append_entries_request() {
    return rpc::SP_PB_MSG(new protocol::AppendEntriesRequest());
}

rpc::SP_PB_MSG RfSrvInternalRpcServerSync::on_request_vote(rpc::SP_PB_MSG sspMsg) {
    return m_pHandler->OnRequestVote(sspMsg);
}

rpc::SP_PB_MSG RfSrvInternalRpcServerSync::create_request_vote_request() {
    return rpc::SP_PB_MSG(new protocol::RequestVoteRequest());
}
} // namespace server
} // namespace ccraft
