/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "abstract-rpc-client.h"

namespace ccraft {
    namespace rpc {
        ARpcClient::ARpcClient(net::ISocketService *ss) : m_pSocketService(ss) {}

    }
}
