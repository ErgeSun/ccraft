/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_NET_CORE_ICONNECTIONMANAGER_H
#define CCRAFT_NET_CORE_ICONNECTIONMANAGER_H

#include "../../common/common-def.h"
#include "../common-def.h"

namespace ccraft {
namespace net {
class AFileEventHandler;

/**
 * worker管理器。
 */
class INetStackWorkerManager {
public:
    virtual ~INetStackWorkerManager() = default;

    virtual AFileEventHandler *GetWorkerEventHandler(net_peer_info_t npt) = 0;
    virtual bool PutWorkerEventHandler(AFileEventHandler *handler) = 0;
    virtual AFileEventHandler* RemoveWorkerEventHandler(net_peer_info_t logicNpt, net_peer_info_t realNpt) = 0;
}; // interface INetStackWorkerManager
}  // namespace net
} // namespace ccraft

#endif //CCRAFT_NET_CORE_ICONNECTIONMANAGER_H
