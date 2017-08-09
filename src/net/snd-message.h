/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_NET_CORE_SND_MESSAGE_H
#define CCRAFT_NET_CORE_SND_MESSAGE_H

#include "message.h"

namespace ccraft {
    namespace net {
        class SndMessage : public Message, public IEncoder {
        public:
            /**
             * 调用此构造一般用与发送新消息，其内部会自动产生一个id以便回应时进行区分。
             * @param mp
             * @param peerInfo 标识所走的协议以及本地socket信息
             */
            SndMessage(common::MemPool *mp, net_peer_info_t peerInfo);

#ifdef WITH_MSG_ID // 如果开启了这个，可能你回复消息的时候需要保持id不变，就需要用到此构造函数。
            /**
             * 一旦发送，则SndMessage的所有权便属于了框架，user无需再管理此SndMessage。生命周期由框架控制。
             * @param mp
             * @param peerInfo 标识所走的协议以及本地socket信息
             * @param id 作为响应时赋予的接收到的消息的id，以便接受者用其分发。
             */
            SndMessage(common::MemPool *mp, net_peer_info_t peerInfo, Id id);
#endif

        public:
            common::Buffer* Encode() override final;

        protected:
            /**
             * 获取派生类的消息encode之后待发送字节的长度。
             * 负载的最大长度为MAX_MSG_PAYLOAD_SIZE。
             * @return
             */
            virtual uint32_t GetDerivePayloadLength() = 0;

            /**
             * 派生类需要实现的对自己内容的编码。
             * @param b
             */
            virtual void EncodeDerive(common::Buffer *b) = 0;

        private:
            static void encode_header(common::Buffer *b, Header &h);
#ifdef WITH_MSG_ID
            static Id get_new_id();

        private:
            static common::spin_lock_t              s_idLock;
            static Id                               s_lastId;
#endif
        };
    } // namespace net
} // namespace ccraft

#endif //CCRAFT_NET_CORE_SND_MESSAGE_H