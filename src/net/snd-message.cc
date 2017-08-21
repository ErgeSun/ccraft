/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../common/buffer.h"
#include "../common/codec-utils.h"

#include "snd-message.h"
#include "../common/common-utils.h"

namespace ccraft {
namespace net {
#if WITH_MSG_ID
common::spin_lock_t SndMessage::s_idLock = UNLOCKED;
#if BULK_MSG_ID
Message::Id SndMessage::s_lastId = Id(0, 0);
#else
Message::Id SndMessage::s_lastId = 0;
#endif
#endif

SndMessage::SndMessage() {
#ifdef WITH_MSG_ID
    m_header.id = get_new_id();
#endif
}

SndMessage::SndMessage(common::MemPool *mp, net_peer_info_t &&peerInfo) : Message(mp) {
#ifdef WITH_MSG_ID
    m_header.id = get_new_id();
#endif
    m_peerInfo = std::move(peerInfo);
}

#ifdef WITH_MSG_ID
SndMessage::SndMessage(common::MemPool *mp, net_peer_info_t &&peerInfo, Id id) : Message(mp) {
    m_header.id = id;
    m_peerInfo = std::move(peerInfo);
}
#endif

common::Buffer* SndMessage::Encode() {
    auto deriveBufferSize = GetDerivePayloadLength();
    if (UNLIKELY(deriveBufferSize <= 0)) {
        throw std::runtime_error("message payload cannot less than 0 bytes");
    }

    if (UNLIKELY(deriveBufferSize > MAX_MSG_PAYLOAD_SIZE)) {
        throw std::runtime_error("message payload cannot more than 64MiB");
    }

    auto totalBufferSize = Message::HeaderSize() + deriveBufferSize;

    m_header.magic = MESSAGE_MAGIC_NO;
    m_header.len = deriveBufferSize;

    auto memPoolObject = m_pMemPool->Get(totalBufferSize);
    auto buf = Message::GetNewAvailableBuffer(memPoolObject, totalBufferSize);
    SndMessage::encode_header(buf, m_header);
    EncodeDerive(buf);

    buf->Pos = buf->Start;
    return buf;
}

void SndMessage::encode_header(common::Buffer *b, Header &h) {
    ByteOrderUtils::WriteUInt32(b->Pos, h.magic);
    b->Pos += sizeof(h.magic);
#if WITH_MSG_ID
#if BULK_MSG_ID
    ByteOrderUtils::WriteUInt64(b->Pos, (uint64_t)(h.id.ts));
    b->Pos += sizeof(uint64_t);
    ByteOrderUtils::WriteUInt32(b->Pos, h.id.seq);
    b->Pos += sizeof(h.id.seq);
#elif BIG_MSG_ID
    ByteOrderUtils::WriteUInt64(b->Pos, h.id);
    b->Pos += sizeof(h.id);
#else
    ByteOrderUtils::WriteUInt32(b->Pos, h.id);
    b->Pos += sizeof(h.id);
#endif
#endif
    ByteOrderUtils::WriteUInt32(b->Pos, h.len);
    b->Pos += sizeof(h.len);
}

#if WITH_MSG_ID
Message::Id SndMessage::get_new_id() {
    common::SpinLock l(&s_idLock);
#if BULK_MSG_ID
    ++s_lastId.seq;
    if (UNLIKELY(s_lastId.seq == UINT32_MAX)) {
        s_lastId.seq = 1;
        s_lastId.ts = common::CommonUtils::GetCurrentTime().sec;
    }

    if (UNLIKELY(s_lastId.ts == 0)) {
        // 0说明没有初始化过，那就
        s_lastId.ts = common::CommonUtils::GetCurrentTime().sec;
    }

    return Id(s_lastId.ts, s_lastId.seq);
#else
    ++s_lastId;
#ifdef BIG_MSG_ID
    if (UNLIKELY(s_lastId == UINT64_MAX)) {
        s_lastId = 1;
    }
#else
    if (UNLIKELY(s_lastId == UINT32_MAX)) {
        s_lastId = 1;
    }
#endif
    return s_lastId;
#endif
}
#endif
} // namespace net
} // namespace ccraft
