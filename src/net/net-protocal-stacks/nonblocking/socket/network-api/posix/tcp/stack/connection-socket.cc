/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <sys/socket.h>
#include <memory.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>

#include "../../../../../../../../common/common-utils.h"
#include "connection-socket.h"

#define SET_OPT_CHECK()                                                                      \
        if (m_connected) {                                                                   \
            throw new std::runtime_error("you should set this opt before connect.");         \
        }

namespace ccraft {
    namespace net {
        bool PosixTcpConnectionSocket::Socket() {
            m_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (-1 == m_fd) {
                fprintf(stderr, "Error socket %d - %s\n", errno, strerror(errno));
                return false;
            } else {
                return true;
            }
        }

        bool PosixTcpConnectionSocket::Connect(struct timeval *timeout) {
            struct sockaddr_in sa;
            bzero(&sa, sizeof(sa));
            sa.sin_family = AF_INET;
            sa.sin_port = htons(m_peer.nat.port);
            inet_pton(AF_INET, m_peer.nat.addr.c_str(), &sa.sin_addr);
            if (!timeout) {
                if (0 == connect(m_fd, (struct sockaddr *) &sa, sizeof(sa))) {
                    if (0 != common::CommonUtils::SetNonBlocking(m_fd)) {
                        return false;
                    }
                    m_connected = true;

                    return true;
                } else {
                    fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
                    return false;
                }
            } else {
                fd_set set;
                int valopt;
                socklen_t lon;

                if (0 != common::CommonUtils::SetNonBlocking(m_fd)) {
                    return false;
                }

                auto res = connect(m_fd, (struct sockaddr *) &sa, sizeof(sa));
                if (res < 0) {
                    if (errno == EINPROGRESS) {
                        FD_ZERO(&set);
                        FD_SET(m_fd, &set);
                        res = select(m_fd + 1, NULL, &set, NULL, timeout);
                        if (res < 0 && errno != EINTR) {
                            fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));

                            return false;
                        } else if (res > 0) {
                            // Socket selected for write
                            lon = sizeof(int);
                            if (getsockopt(m_fd, SOL_SOCKET, SO_ERROR, (void *) (&valopt), &lon) < 0) {
                                fprintf(stderr, "Error in getsockopt() %d - %s\n", errno, strerror(errno));

                                return false;
                            }
                            // Check the value returned...
                            if (valopt) {
                                fprintf(stderr, "Error in delayed connection() %d - %s\n", valopt, strerror(valopt));

                                return false;
                            }
                        } else {
                            fprintf(stderr, "Timeout in select() - Cancelling!\n");
                            return false;
                        }
                    } else {
                        fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
                        return false;
                    }
                }

                return true;
            }
        }

        ssize_t PosixTcpConnectionSocket::Read(void *buf, size_t nbytes, int &err) {
            ssize_t n;
            do {
                n = ::read(m_fd, buf, nbytes);
            } while (n < 0 && EINTR == (err = errno));

            return n;
        }

        ssize_t PosixTcpConnectionSocket::Write(void *buf, size_t nbytes, int &err) {
            ssize_t n;
            do {
                n = ::write(m_fd, buf, nbytes);
            } while (n < 0 && EINTR == (err = errno));

            return n;
        }

        bool PosixTcpConnectionSocket::SetNoDelay(bool ok) {
            int optVal = ok ? 1 : 0;
            return 0 == setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY, &optVal, sizeof(optVal));
        }

        bool PosixTcpConnectionSocket::SetRcvBufferSize(int nbytes) throw(std::runtime_error) {
            SET_OPT_CHECK();
            int buf_len = nbytes;
            return 0 == setsockopt(m_fd, SOL_SOCKET, SO_RCVBUF, &buf_len, sizeof(int));
        }

        bool PosixTcpConnectionSocket::SetSndBufferSize(int nbytes) throw(std::runtime_error) {
            SET_OPT_CHECK();
            int buf_len = nbytes;
            return 0 == setsockopt(m_fd, SOL_SOCKET, SO_SNDBUF, &buf_len, sizeof(int));
        }

        bool PosixTcpConnectionSocket::SetSoLinger(bool ok, int seconds) {
            struct linger lingerTime;
            lingerTime.l_onoff = ok ? 1 : 0;
            lingerTime.l_linger = seconds;

            return 0 == setsockopt(m_fd, SOL_SOCKET, SO_LINGER, &lingerTime, sizeof(lingerTime));
        }

        bool PosixTcpConnectionSocket::SetKeepalive(bool ok) {
            int opVal = ok ? 1 : 0;
            return 0 == setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE, &opVal, sizeof(int));
        }

        bool PosixTcpConnectionSocket::SetAddrReuse(bool ok) {
            int opVal = ok ? 1 : 0;
            return 0 == setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opVal, sizeof(int));
        }

        int PosixTcpConnectionSocket::SetSockOpt(int __level, int __optname,
                                                 const void *__optval, socklen_t __optlen) {
            return setsockopt(m_fd, __level, __optname, __optval, __optlen);
        }

        int PosixTcpConnectionSocket::Close() {
            if (-1 != m_fd) {
                return ::close(m_fd);
            }

            return 0;
        }
    } // namespace net
} // namespace ccraft
