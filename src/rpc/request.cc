/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <google/protobuf/message.h>

#include "../common/buffer.h"
#include "../common/codec-utils.h"
#include "../common/protobuf-utils.h"

#include "request.h"
#include "common-def.h"

namespace ccraft {
namespace rpc {
uint32_t RpcRequest::getDerivePayloadLength() {
    if (m_sspMsg.get()) {
        return sizeof(MessageTypeType) + sizeof(uint16_t) + m_sspMsg->ByteSize();
    } else {
        return sizeof(MessageTypeType) + sizeof(uint16_t);
    }
}

void RpcRequest::encodeDerive(common::Buffer *b) {
    *(b->GetPos()) = (uchar)((uint8_t)(MessageType::Request));
    b->MoveHeadBack(1);
    ByteOrderUtils::WriteUInt16(b->GetPos(), (uint16_t)m_iHandlerId);
    b->MoveHeadBack(sizeof(uint16_t));
    if (m_sspMsg.get()) {
        common::ProtoBufUtils::Serialize(m_sspMsg.get(), b);
    }
}
} // namespace rpc
} // namespace ccraft

