/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_NET_CORE_SOCKETAPI_NETSTACKMESSAGEWORKER_H
#define CCRAFT_NET_CORE_SOCKETAPI_NETSTACKMESSAGEWORKER_H

#include <unordered_map>

#include "../../../../../common/common-def.h"
#include "../../../../../common/blocking-queue.h"

#include "../../../../snd-message.h"
#include "../../../../notify-message.h"

namespace ccraft {
namespace common {
class MemPool;
class Buffer;
}

namespace net {
/**
 * 本类负责对message的处理。具体的socket实现类要继承于此类。
 * 事件管理器有事件了会调用读写。
 */
class ANetStackMessageWorker {
protected:
    enum class NetWorkerState {
        StartToRcvHeader = 0,
            RcvingHeader,
            StartToRcvPayload,
            RcvingPayload
    };

public:
    /**
     *
     * @param maxCacheMessageCnt 消息缓冲队列的最大消息个数。0为无限制。
     */
    ANetStackMessageWorker(AFileEventHandler *eventHandler, common::MemPool *memPool, NotifyMessageCallbackHandler msgCallbackHandler, uint32_t maxCacheMessageCnt = 0);
    virtual ~ANetStackMessageWorker();

    /**
     * 必须先调用此函数进行初始化。
     * @return
     */
    virtual bool Initialize() = 0;
    /**
     *
     */
    bool SendMessage(SndMessage *m);

    void HandleMessage(NotifyMessage *m);
    /**
     *
     */
    bool InsertMessage(SndMessage *m);
    /**
     * 错误: 返回false(无论是[socket错误或对端关闭]还是[codec校验错误])
     * 正常: 返回true(即便是遇到了EAGAIN，只要没有发生错误)
     *
     * !注意: 基类默认参数值会覆盖派生类的默认值。
     * @return
     */
    virtual bool Recv(bool breakWhenRecvOne = false) = 0;

    /**
     * 发送缓冲队列里面的数据。
     * @return
     */
    virtual bool Send() = 0;

    inline AFileEventHandler* GetEventHandler() {
        return m_pEventHandler;
    }

protected:
    static std::function<void(RcvMessage*)> s_release_rm_handle;
    static RcvMessage* getNewRcvMessage(common::MemPool *mp, net_peer_info_t peerInfo, Message::Header h,
                                        common::Buffer *buffer);
    static void releaseRcvMessage(RcvMessage *rm);

protected:
    common::MemPool                    *m_pMemPool;
    common::Buffer                     *m_pHeaderBuffer;
    common::BlockingQueue<SndMessage*> *m_bqMessages;
    AFileEventHandler                  *m_pEventHandler;
    NotifyMessageCallbackHandler        m_msgCallback;
};
} // namespace net
} // namespace ccraft

#endif //CCRAFT_NET_CORE_SOCKETAPI_NETSTACKMESSAGEWORKER_H
