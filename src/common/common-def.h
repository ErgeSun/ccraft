/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_COMMON_COMMON_DEF_H
#define CCRAFT_COMMON_COMMON_DEF_H

#include <time.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <cstring>

/**
 * 控制目标不导出，即仅库内部可见。
 */
#define GCC_INTERNAL                   __attribute__ ((visibility("hidden")))

#define LIKELY(x)                      __builtin_expect(!!(x), 1)
#define UNLIKELY(x)                    __builtin_expect(!!(x), 0)

#define hw_rw_memory_barrier()         __sync_synchronize()
#define soft_yield_cpu()               __asm__ ("pause")
#define hard_yield_cpu()               sched_yield()
#define atomic_cas(lock, old, set)     __sync_bool_compare_and_swap(lock, old, set)
#define atomic_zero(lock)              __sync_fetch_and_and(lock, 0)
#define atomic_addone_and_fetch(lock)  __sync_add_and_fetch(lock, 1)

#define DELETE_PTR(p) if (p) {delete (p); (p) = nullptr;}
#define DELETE_ARR_PTR(p) if (p) {delete [](p); (p) = nullptr;}
#define FREE_PTR(p) if (p) {free (p); (p) = NULL;}

typedef unsigned char uchar;

namespace ccraft {
namespace common {
const int  CPUS_CNT = get_nprocs();
const long CACHELINE_SIZE = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
const long PAGE_SIZE = sysconf(_SC_PAGESIZE);

typedef struct uctime_s {
    uctime_s() : sec(-1), nsec(-1) {}

    uctime_s(long s, long n) : sec(s), nsec(n) {}

    explicit uctime_s(const struct timespec ts) {
        sec = ts.tv_sec;
        nsec = ts.tv_nsec;
    }

    uctime_s(const uctime_s &ut) {
        this->sec = ut.sec;
        this->nsec = ut.nsec;
    }

    uctime_s &operator=(const uctime_s &ut) {
        this->sec = ut.sec;
        this->nsec = ut.nsec;
        return *this;
    }

    long sec;
    long nsec;

    long get_total_nsecs() const {
        return sec * 1000000000 + nsec;
    }
} uctime_t;

// arithmetic operators
inline uctime_t &operator+=(uctime_t &l, const uctime_t &r) {
    l.sec += r.sec + (l.nsec + r.nsec) / 1000000000L;
    l.nsec += r.nsec;
    l.nsec %= 1000000000L;
    return l;
}

// comparators
inline bool operator>(const uctime_t &a, const uctime_t &b) {
    return (a.sec > b.sec) || (a.sec == b.sec && a.nsec > b.nsec);
}

inline bool operator<=(const uctime_t &a, const uctime_t &b) {
    return !(operator>(a, b));
}

inline bool operator<(const uctime_t &a, const uctime_t &b) {
    return (a.sec < b.sec) || (a.sec == b.sec && a.nsec < b.nsec);
}

inline bool operator>=(const uctime_t &a, const uctime_t &b) {
    return !(operator<(a, b));
}

inline bool operator==(const uctime_t &a, const uctime_t &b) {
    return a.sec == b.sec && a.nsec == b.nsec;
}

inline bool operator!=(const uctime_t &a, const uctime_t &b) {
    return a.sec != b.sec || a.nsec != b.nsec;
}
} // namespace common
} // namespace ccraft
#endif //CCRAFT_COMMON_COMMON_DEF_H
