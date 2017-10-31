/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_NET_CORE_COMMON_DEF_H
#define CCRAFT_NET_CORE_COMMON_DEF_H

#include <string>
#include <iostream>
#include <sstream>

#include "../common/hash-algorithms.h"

using std::ostream;

namespace ccraft {
namespace net {
enum class NonBlockingEventModel {
    DPDK   = 0,
    Posix
};

enum class SocketProtocol {
    None = 0,
    Tcp,
    Udp
};

typedef struct net_addr_s {
    std::string addr;
    uint16_t port;

    net_addr_s() = default;

    net_addr_s(const std::string &a, uint16_t p) : addr(a), port(p) {}
    net_addr_s(std::string &&a, uint16_t p) : addr(std::move(a)), port(p) {}

    net_addr_s(const net_addr_s &nas) noexcept {
        addr = nas.addr;
        port = nas.port;
    }

    net_addr_s &operator=(const net_addr_s &nas) = default;
    net_addr_s(net_addr_s &&nas) noexcept {
        addr = std::move(nas.addr);
        port = nas.port;
    }

    net_addr_s &operator=(net_addr_s &&nas) noexcept {
        addr = std::move(nas.addr);
        port = nas.port;
        return *this;
    }
} net_addr_t;

typedef struct net_peer_info_s {
    net_addr_t nat;
    SocketProtocol sp;

    net_peer_info_s() {
        sp = SocketProtocol::None;
    }

    net_peer_info_s(const net_addr_t &n, SocketProtocol s) : nat(n), sp(s) {}
    net_peer_info_s(net_addr_t &&n, SocketProtocol s) : nat(std::move(n)), sp(s) {}
    net_peer_info_s(const std::string &addr, uint16_t port, SocketProtocol s) {
        nat = net_addr_t(addr, port);
        sp = s;
    }
    net_peer_info_s(std::string &&addr, uint16_t port, SocketProtocol s) {
        nat = net_addr_t(std::move(addr), port);
        sp = s;
    }

    net_peer_info_s(const net_peer_info_s &npis) {
        nat = npis.nat;
        sp = npis.sp;
    }

    net_peer_info_s(net_peer_info_s &&npis) noexcept {
        nat = std::move(npis.nat);
        sp = npis.sp;
    }

    net_peer_info_s& operator=(const net_peer_info_s &npis) = default;

    net_peer_info_s& operator=(net_peer_info_s &&npis) {
        nat = std::move(npis.nat);
        sp = npis.sp;

        return *this;
    }

    friend ostream& operator<<(ostream &os, const net_peer_info_s &peer);
} net_peer_info_t, net_local_info_t;

inline bool operator==(const net_peer_info_t &a, const net_peer_info_t &b) {
    return a.sp == b.sp && a.nat.port == b.nat.port && (0 == a.nat.addr.compare(b.nat.addr));
}

inline ostream& operator<<(ostream &os, const net_peer_info_s &peer){
    os << "peer{protocol = " << int32_t(peer.sp) << ", addr = " <<  peer.nat.addr.c_str() << ":" << peer.nat.port << "}";
    return os;
}
} // namespace net
} // namespace ccraft

// declare hash<net_peer_info_t>
namespace std {
template<>
struct hash<ccraft::net::net_peer_info_t> {
    uint32_t operator()(const ccraft::net::net_peer_info_t &npit) const {
        std::stringstream ss;

        ss << npit.nat.addr << ":" << npit.nat.port << "-" << (int) (npit.sp);
        auto key = ss.str();

        uint32_t hashcode;

        MurmurHash3_x86_32(key.c_str(), (int) (key.length()), 22, &hashcode);

        return hashcode;
    }
};
}

#endif //CCRAFT_NET_CORE_COMMON_DEF_H
