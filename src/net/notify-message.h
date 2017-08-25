/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_NET_CORE_MSG_CALLBACK_H
#define CCRAFT_NET_CORE_MSG_CALLBACK_H

#include <functional>
#include <memory>

namespace ccraft {
namespace net {
enum class NotifyMessageType {
    Server = 0,
    Worker,
    Message
};

enum class ServerNotifyMessageCode {
    OK    = 0,
    Error
};

enum class WorkerNotifyMessageCode {
    OK    = 0,
    ClosedByPeer,
    Error
};

/**
 * user不需要释放。
 */
class NotifyMessage {
public:
    NotifyMessage(NotifyMessageType type, std::string &&msg) : m_type(type), m_msg(std::move(msg)) {}
    virtual ~NotifyMessage() = default;

    inline NotifyMessageType GetType() {
        return m_type;
    }

    inline std::string What() {
        return m_msg;
    }
private:
    NotifyMessageType m_type;
    std::string       m_msg;
};

class ServerNotifyMessage : public NotifyMessage {
public:
    ServerNotifyMessage(ServerNotifyMessageCode code, std::string &&msg) :
        NotifyMessage(NotifyMessageType::Server, std::move(msg)), m_code(code) {}

    inline ServerNotifyMessageCode GetCode() {
        return m_code;
    }

private:
    ServerNotifyMessageCode m_code;
};

class WorkerNotifyMessage : public NotifyMessage {
public:
    WorkerNotifyMessage(WorkerNotifyMessageCode code, net::net_peer_info_t &&peer, std::string &&msg) :
        NotifyMessage(NotifyMessageType::Worker, std::move(msg)), m_code(code), m_peer(std::move(peer)) {}

    inline WorkerNotifyMessageCode GetCode() {
        return m_code;
    }

    inline net::net_peer_info_t GetPeer() {
        return m_peer;
    }

private:
    WorkerNotifyMessageCode m_code;
    net::net_peer_info_t    m_peer;
};

class RcvMessage;
class MessageNotifyMessage : public NotifyMessage {
public:
    MessageNotifyMessage(RcvMessage* rm, std::function<void(RcvMessage*)> releaseHandle) :
        NotifyMessage(NotifyMessageType::Message, ""), m_ref(rm) {}

    ~MessageNotifyMessage() override {
        if (m_releaseHandle) {
            m_releaseHandle(m_ref);
            m_ref = nullptr;
        }
    }

    inline const RcvMessage* GetContent() const {
        return m_ref;
    }

private:
    RcvMessage                      *m_ref;
    std::function<void(RcvMessage*)> m_releaseHandle;
};

/**
 * user不需要负责释放。
 */
typedef std::function<void(std::shared_ptr<NotifyMessage>)> NotifyMessageCallbackHandler;
} // namespace net
} // namespace ccraft


#endif //CCRAFT_NET_CORE_MSG_CALLBACK_H
