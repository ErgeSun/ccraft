/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_NET_CORE_POSIX_TCP_NET_STACK_WORKER_H
#define CCRAFT_NET_CORE_POSIX_TCP_NET_STACK_WORKER_H

#include "../../../../../../../common/common-def.h"
#include "../../net-stack-msg-worker.h"
#include "stack/connection-socket.h"

namespace ccraft {
    namespace common {
        class MemPool;
    }

    namespace net {
        /**
         * Posix tcp的消息处理类。事件管理器有事件了会调用。
         */
        class PosixTcpNetStackWorker : public ANetStackMessageWorker {
        public:
            PosixTcpNetStackWorker(AFileEventHandler *eventHandler, common::MemPool *memPool, PosixTcpClientSocket *socket,
                                   NotifyMessageCallbackHandler msgCallbackHandler);
            ~PosixTcpNetStackWorker();

            /**
             * 错误: 返回false(无论是[socket错误或对端关闭]还是[codec校验错误])
             * 正常: 返回true(即便是遇到了EAGAIN，只要没有发生错误)
             * @return
             */
            bool Recv();

            /**
             * 发送缓冲队列里面的数据。
             * @return
             */
            bool Send();

        private:
            static WorkerNotifyMessage* get_closed_by_peer_worker_message(std::string &&msg);
            static WorkerNotifyMessage* get_broken_worker_message(std::string &&msg);

        private:
            /**
             * 此为弱引用关系，关联关系，外部创建者会释放，本类无需释放。
             */
            PosixTcpClientSocket    *m_pSocket;
            NetWorkerState           m_rcvState = NetWorkerState::StartToRcvHeader;
            Message::Header          m_header;
            common::Buffer          *m_payloadBuffer = nullptr;
            common::Buffer          *m_pSendingBuffer = nullptr;
        };
    } // namespace net
} // namespace ccraft

#endif //CCRAFT_NET_CORE_POSIX_TCP_NET_STACK_WORKER_H
