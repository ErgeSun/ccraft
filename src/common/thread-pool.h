/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_COMMON_THREAD_POOL_H
#define CCRAFT_COMMON_THREAD_POOL_H

#include <thread>
#include <vector>
#include <unordered_set>
#include <atomic>

#include "blocking-queue.h"
#include "common-def.h"
#include "spin-lock.h"

namespace ccraft {
namespace common {
/**
 * 当前仅支持FIFO调度的线程池。
 * TODO(sunchao): 1、添加调度策略 2、考虑task相关联性，关联task分配到不同的线程 3、cpu亲缘性绑定？再思考，感觉没必要。
 *                4、添加指定的task的wait task、cancel task
 */
class ThreadPool {
public:
    typedef std::function<void(void)> Task;

    /**
     *
     * @param threads_cnt 线程个数。如果小于等于0则为cpu核数 * 2个。
     */
    ThreadPool(int threads_cnt = 0);

    /**
     * 会join所有threads。
     */
    ~ThreadPool();

    /**
     * 添加一个任务到线程池中执行。
     * @param t 添加的task。
     */
    inline void AddTask(Task t) {
        m_bqTasks.Push(t);
    }

    /**
     * 等待所有的task完成。
     */
    void WaitAll();

    /**
     * 等待所有都完成或者指定的时间到了。
     * @param duration_since_epoch epoch开始到现在的duration。
     */
    void WaitAllUntilTimeAt(cctime_s duration_since_epoch);

    /**
     * 等待所有都完成或者指定的时间到了。
     * @param duration 从现在开始最多等待的持续时间。
     */
    void WaitAllUntilAfter(cctime_s duration);

private:
    void proc();

private:
    bool                           m_bStopping = false;
    std::vector<std::thread*>      m_vThreadps;
    common::BlockingQueue<Task>    m_bqTasks;
    std::atomic<int>               m_iActiveWorkersCnt;
    std::mutex                     m_mtxActiveWorkerCnt;
    std::condition_variable        m_cvActiveWorkerCnt;
}; // class ThreadPool
}  // namespace common
}  // namespace ccraft
#endif //CCRAFT_COMMON_THREAD_POOL_H
