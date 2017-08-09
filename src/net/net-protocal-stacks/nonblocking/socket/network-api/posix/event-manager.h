/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_NET_CORE_NB_SOCKET_NA_POSIX_TCP_EVENTMANAGER_H
#define CCRAFT_NET_CORE_NB_SOCKET_NA_POSIX_TCP_EVENTMANAGER_H

#include <thread>

#include "../../../../../common-def.h"

#include "../abstract-event-manager.h"
#include "../../event-drivers/ievent-driver.h"
#include "../../event-drivers/event-worker.h"
#include "../../../../../../common/spin-lock.h"

namespace ccraft {
    namespace net {
        typedef std::function<bool(AFileEventHandler*)> ValidHandlerFunc;
        /**
         * socket出错了manager检测到后会释放。
         */
        class PosixEventManager : public AEventManager {
        public:
            PosixEventManager(SocketProtocal sp, std::shared_ptr<net_addr_t> sspNat, common::MemPool *memPool, uint32_t maxEvents,
                                 uint32_t connWorkersCnt, ConnectHandler connectHandler, FinishHandler finishHandler,
                                 NotifyMessageCallbackHandler msgCallbackHandler);

            ~PosixEventManager();

            bool Start(NonBlockingEventModel m) override;
            bool Stop() override;

            int AddEvent(AFileEventHandler *socketEventHandler, int cur_mask, int mask) override;

        private:
            void worker_loop(EventWorker *ew);
            void on_connect(AFileEventHandler *handler);
            inline void process_event(NetEvent *netEvent);

        private:
            SocketProtocal                                     m_sp;
            std::shared_ptr<net_addr_t>                        m_sspNat;
            uint32_t                                           m_iConnWorkersCnt;
            int32_t                                            m_iCurWorkerIdx = -1;
            bool                                               m_bStopped = true;
            AFileEventHandler                                 *m_pServerEventHandler = nullptr;
            std::pair<std::thread*, EventWorker*>              m_pListenWorkerEventLoopCtx;
            std::vector<std::pair<std::thread*, EventWorker*>> m_vConnsWorkerEventLoopCtxs;
            common::spin_lock_t                                m_slSelectEvents = UNLOCKED;
            ConnectHandler                                     m_onConnect;
            FinishHandler                                      m_onFinish;
            NotifyMessageCallbackHandler                       m_msgCallback;
        };
    } // namespace net
} // namespace ccraft

#endif //CCRAFT_NET_CORE_NB_SOCKET_NA_POSIX_TCP_EVENTMANAGER_H
