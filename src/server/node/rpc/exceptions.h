/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SRV_NODE_RPC_EXCEPTIONS_H
#define CCRAFT_SRV_NODE_RPC_EXCEPTIONS_H

#include <exception>
#include <cstdint>
#include <string>
#include <sstream>

#include "../../../rpc/exceptions.h"

using ccraft::rpc::RpcException;

namespace ccraft {
namespace server {
class BadRpcHandlerIdException : public RpcException {
public:
    BadRpcHandlerIdException(uint16_t code, uint16_t handlerId) : m_iCode(code), m_iHandlerId(handlerId) {
        std::stringstream ss;
        ss << "Bad handler id " << m_iHandlerId << ".";
        m_sWhat = ss.str();
    }

    const char *what() const noexcept override {
        return m_sWhat.c_str();
    }

    uint16_t code() const {
        return m_iCode;
    }

    uint16_t handler_id() const {
        return m_iHandlerId;
    }

private:
    uint16_t m_iCode;
    uint16_t m_iHandlerId;

};

class RpcClientInternalException : public RpcException {
public:
    RpcClientInternalException() {
        m_sWhat = "Rpc client internal error!";
    }

    const char *what() const noexcept override {
        return m_sWhat.c_str();
    }
};

class RpcServerInternalException : public RpcException {
public:
    RpcServerInternalException() {
        m_sWhat = "Rpc peer server internal error! Maybe oom.";
    }

    const char *what() const noexcept override {
        return m_sWhat.c_str();
    }
};

class RpcClientSendTimeoutException : public RpcException {
public:
    RpcClientSendTimeoutException() {
        m_sWhat = "Rpc client send timeout!";
    }

    const char *what() const noexcept override {
        return m_sWhat.c_str();
    }
};

class RpcBrokenPipeException : public RpcException {
public:
    RpcBrokenPipeException() {
        m_sWhat = "Rpc pipe is broken!";
    }

    const char *what() const noexcept override {
        return m_sWhat.c_str();
    }
};

class RpcMessageCorruptException : public RpcException {
public:
    RpcMessageCorruptException(uint16_t handlerId) {
        std::stringstream ss;
        ss << "Cannot parse sent message error for handler id " << handlerId << ".";
        m_sWhat = ss.str();
    }

    const char *what() const noexcept override {
        return m_sWhat.c_str();
    }
};

class RpcClientIsBusyException : public RpcException {
public:
    RpcClientIsBusyException() {
        m_sWhat = "Cannot push message into sent msg queue, the sent msg queue is full!";
    }

    const char *what() const noexcept override {
        return m_sWhat.c_str();
    }
};
}
}

#endif //CCRAFT_SRV_NODE_RPC_EXCEPTIONS_H
