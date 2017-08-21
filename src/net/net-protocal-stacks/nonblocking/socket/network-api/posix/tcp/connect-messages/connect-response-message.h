/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_CONNECT_RESPONSE_MESSAGE_H
#define CCRAFT_CONNECT_RESPONSE_MESSAGE_H

#include "../../../../../../../snd-message.h"
#include "../../../../../../../../common/codec-utils.h"
#include "../../../../../../../../common/buffer.h"

namespace ccraft {
namespace net {
class ConnectResponseMessage : public SndMessage {
public:
    enum class Status {
        OK     = 0,
        ERROR  = 1
    };

public:
    ConnectResponseMessage(common::MemPool *mp, Status stat, std::string &&what) :
        SndMessage(mp, net_peer_info_s()), m_stat(stat), m_sWhat(std::move(what)) {}

protected:
    uint32_t GetDerivePayloadLength() override {
        return sizeof(uint16_t)/*Status*/ + m_sWhat.length()/*what*/;
    }

    void EncodeDerive(common::Buffer *b) override {
        ByteOrderUtils::WriteUInt16(b->Pos, (uint16_t)m_stat);
        b->Pos += sizeof(uint16_t);
        memcpy(b->Pos, m_sWhat.c_str(), m_sWhat.length());
    }

private:
    Status                  m_stat;
    std::string             m_sWhat;
};
}
}

#endif //CCRAFT_CONNECT_RESPONSE_MESSAGE_H
