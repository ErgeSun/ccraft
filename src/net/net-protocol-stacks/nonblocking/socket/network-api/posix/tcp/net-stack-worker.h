/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_NET_CORE_POSIX_TCP_NET_STACK_WORKER_H
#define CCRAFT_NET_CORE_POSIX_TCP_NET_STACK_WORKER_H

#include "../../../../../../../common/common-def.h"
#include "../../net-stack-msg-worker.h"
#include "stack/connection-socket.h"
#include "../../abstract-event-manager.h"

namespace ccraft {
namespace ccsys {
class MemPool;
}

namespace net {
/**
 * Posix tcp的消息处理类。事件管理器有事件了会调用。
 */
class PosixTcpNetStackWorker : public ANetStackMessageWorker {
public:
    enum CreatorType {
        Client = 0,
        Server
    };
public:
    PosixTcpNetStackWorker(CreatorType ct, AFileEventHandler *eventHandler, ccsys::MemPool *memPool,
    PosixTcpClientSocket *socket, NotifyMessageCallbackHandler msgCallbackHandler,
    uint16_t logicPort, ConnectFunc logicConnect);
    PosixTcpNetStackWorker(CreatorType ct, AFileEventHandler *eventHandler, ccsys::MemPool *memPool,
    PosixTcpClientSocket *socket, NotifyMessageCallbackHandler msgCallbackHandler,
    ConnectFunc logicConnect);
    ~PosixTcpNetStackWorker() override;

    /**
     * 为了对联接进行管理，需要加一层握手逻辑。握手期间通过消息传递必要的连接管理信息，目前连接发起者会在第一条消息中携带逻辑端口信息
     * 以便连接管理器INetStackWorkerManager通过逻辑端口对连接进行管理。比如UniqueWorkerManager会通过逻辑端口作为连接的唯一标识，
     * 只保留一个连接。
     * @return
     */
    bool Initialize() override;
    /**
     * 错误: 返回false(无论是[socket错误或对端关闭]还是[codec校验错误])
     * 正常: 返回true(即便是遇到了EAGAIN，只要没有发生错误)
     * @return
     */
    bool Recv(bool breakWhenRecvOne = false) override;

    /**
     * 发送缓冲队列里面的数据。
     * @return
     */
    bool Send() override;

private:
    enum class ConnectionState {
        Connecting,
        ConnectSe,    /* client发起connect之后 */
        ConnectingRe, /* server回复之后 */
        WaitLastACK,  /* server connected之后需要回复 */
        Connected
    };

private:
    void handshake(RcvMessage *rm);

private:
    static WorkerNotifyMessage* get_closed_by_peer_worker_message(net::net_peer_info_t peer, std::string &&msg);
    static WorkerNotifyMessage* get_broken_worker_message(net::net_peer_info_t peer, std::string &&msg);

private:
    /**
     * 创建者类型，用于框架概念内的握手逻辑。
     */
    CreatorType              m_ct;
    /**
     * 此为弱引用关系，关联关系，外部创建者会释放，本类无需释放。
     */
    PosixTcpClientSocket    *m_pSocket;
    NetWorkerState           m_rcvState       = NetWorkerState::StartToRcvHeader;
    Message::Header          m_header;
    common::Buffer          *m_payloadBuffer  = nullptr;
    common::Buffer          *m_pSendingBuffer = nullptr;
    ConnectionState          m_connState      = ConnectionState::Connecting;
    bool                     m_bConnHandShakeCompleted = false;
    std::mutex               m_initWaitMtx;
    std::condition_variable  m_initWaitCv;
    uint16_t                 m_iLogicPort;
    ConnectFunc              m_onLogicConnect;
};
} // namespace net
} // namespace ccraft

#endif //CCRAFT_NET_CORE_POSIX_TCP_NET_STACK_WORKER_H
