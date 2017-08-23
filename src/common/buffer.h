/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_COMMON_BUFFER_H
#define CCRAFT_COMMON_BUFFER_H

#include "common-def.h"
#include "mem-pool.h"

namespace ccraft {
namespace common {
/**
 * [约定] 本buffer的使用规则需要统一如下：
 *   - last为空格，last最大到end为止
 *   - 总长度为  ：[start, end] --> 闭区间
 *   - 有效长度为：[pos, last]  --> 闭区间
 */
class Buffer {
public:
    Buffer() = default;
    Buffer(uchar *pos, uchar *last, uchar *start, uchar *end, MemPoolObject *mpo) :
        Pos(pos), Last(last), Start(start), End(end), MpObject(mpo) {}
    ~Buffer() {
        Put();
    }

    Buffer(Buffer&&) = delete;
    Buffer(Buffer&) = delete;
    Buffer& operator=(Buffer&) = delete;

    inline void Put() {
        Refresh(nullptr, nullptr, nullptr, nullptr, nullptr);
    }

    inline void Refresh(uchar *pos, uchar *last, uchar *start, uchar *end, MemPoolObject *mpo) {
        if (MpObject) {
            MpObject->Put();
        } else {
            DELETE_ARR_PTR(Start);
        }

        MpObject = mpo;
        Pos = pos;
        Last = last;
        Start = start;
        End = end;
    }

    inline bool Valid() {
        return Pos && Last && (uintptr_t)Last >= (uintptr_t)Pos;
    }

    inline void BZero() {
        Pos = nullptr;
        Last = nullptr;
        bzero(Start, (uintptr_t)End - (uintptr_t)Start + 1);
    }

    /**
     * 向后移动有效头指针Pos
     * @param n
     */
    inline void MoveHeadBack(uint32_t n) {
        if (Pos && Last) {
            Pos += n;
        }
    }

    /**
     * 向后移动有效尾指针Last
     * @param n
     */
    inline void MoveTailBack(uint32_t n) {
        if (!Pos) {
            Pos = Start;
        }

        if (Last) {
            Last += n;
        } else {
            Last = Start + n - 1;
        }
    }

    inline int32_t TotalLength() const {
        if (!Start || !End) {
            return 0;
        }

        return (int32_t)((uintptr_t)End - (uintptr_t)Start) + 1;
    }

    inline int32_t AvailableLength() const {
        if (!Last || !Pos) {
            return 0;
        }

        return (int32_t)((int64_t)(uintptr_t)Last - (int64_t)(uintptr_t)Pos) + 1;
    }

    inline size_t UnusedSize() const {
        if (Last) {
            return (size_t)(End - Last);
        } else {
            return (size_t)(End - Start) + 1;
        }
    }

    inline void Reset() {
        Refresh(nullptr, nullptr, nullptr, nullptr, nullptr);
    }

    uchar *Pos              = nullptr;
    uchar *Last             = nullptr;
    uchar *Start            = nullptr;
    uchar *End              = nullptr;
    MemPoolObject *MpObject = nullptr;
};
}  // namespace common
}  // namespace ccraft

#endif //CCRAFT_COMMON_BUFFER_H
