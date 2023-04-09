#pragma once

#include "queue.h"
#include "wait_group.h"

#include <vector>
#include <thread>


namespace tp {

    using Task = std::function<void()>;

// Fixed-size pool of worker threads

    class ThreadPool {
    public:
        explicit ThreadPool(
                size_t threads = std::thread::hardware_concurrency());

        ~ThreadPool();

        // Non-copyable
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;

        // Non-movable
        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;

        // Launches worker threads
        void Start();

        // Schedules task for execution in one of the worker threads
        void Submit(Task);

        // Locates current thread pool from worker thread
        static ThreadPool* Current();

        // Waits until outstanding work count reaches zero
        void WaitIdle();

        // Stops the worker threads as soon as possible
        void Stop();

    private:
        void Worker();

        const size_t threads_num_;
        // Worker threads, task queue, etc
        std::vector<std::thread> workers_;
        UnboundedBlockingQueue<Task> tasks_;
        WaitGroup non_finished_counter_;
        std::atomic<bool> stopped_{false};
    };

}  // namespace tp
