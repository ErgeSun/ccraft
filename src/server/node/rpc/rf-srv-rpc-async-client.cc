/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../../ccsys/gcc-buildin.h"
#include "../../../net/notify-message.h"
#include "../../../net/rcv-message.h"
#include "../../../rpc/common-def.h"

#include "common-def.h"
#include "rf-srv-rpc-async-client.h"

namespace ccraft {
namespace server {

ImplStandardAsyncRpcWithNoMsgId(RfSrvInternalRpcClientAsync, AppendEntries)
ImplStandardAsyncRpcWithNoMsgId(RfSrvInternalRpcClientAsync, RequestVote)
ImplStandardAsyncRpcWithMsgId(RfSrvInternalRpcClientAsync, AppendEntries)
ImplStandardAsyncRpcWithMsgId(RfSrvInternalRpcClientAsync, RequestVote)

bool RfSrvInternalRpcClientAsync::register_rpc_handlers() {
    if (!registerRpc(RpcAppendEntries, APPEND_ENTRIES_RPC_ID)) {
        return false;
    }

    if (!registerRpc(RpcRequestVote, REQUEST_VOTE_RPC_ID)) {
        return false;
    }

    finishRegisterRpc();

    return true;
}

bool RfSrvInternalRpcClientAsync::onStart() {
    return register_rpc_handlers();
}

bool RfSrvInternalRpcClientAsync::onStop() {
    return true;
}

void RfSrvInternalRpcClientAsync::onRecvMessage(std::shared_ptr<net::NotifyMessage> sspNM) {
    if (LIKELY(m_fCallback)) {
        m_fCallback(sspNM);
    }
}
} // namespace server
} // namespace ccraft
