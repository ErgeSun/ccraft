/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_NET_CORE_SOCKETAPI_SOCKET_EVENT_HANDLER_H
#define CCRAFT_NET_CORE_SOCKETAPI_SOCKET_EVENT_HANDLER_H

#include "../../ievent-handler.h"
#include "file-descriptor.h"
#include "../../../../../common/common-def.h"

namespace ccraft {
namespace net {
class EventWorker;
class ANetStackMessageWorker;
class AFileEventHandler : public IEventHandler {
public:
    AFileEventHandler() = default;
    explicit AFileEventHandler(FileDescriptor *socketDesc) : m_socketDesc(socketDesc) {}

    virtual ~AFileEventHandler() = default;
    /**
     * 必须先调用此函数进行初始化。
     * @return
     */
    virtual bool Initialize() = 0;

    inline FileDescriptor* GetSocketDescriptor() {
        return m_socketDesc;
    }

    inline void SetOwnWorker(EventWorker* ew) {
        m_pOwnEvWorker = ew;
    }

    inline EventWorker* GetOwnWorker() {
        return m_pOwnEvWorker;
    }

    virtual ANetStackMessageWorker* GetStackMsgWorker() = 0;

protected:
    inline void SetSocketDescriptor(FileDescriptor *psd) {
        m_socketDesc = psd;
    }

private:
    FileDescriptor *m_socketDesc = nullptr;
    EventWorker    *m_pOwnEvWorker = nullptr;
};
}
}

#endif //CCRAFT_NET_CORE_SOCKETAPI_SOCKET_EVENT_HANDLER_H
