#include "thread_pool.h"

namespace tp {

    static thread_local ThreadPool* current_pool = nullptr;

    ThreadPool* ThreadPool::Current() {
        return current_pool;
    }

    ThreadPool::ThreadPool(size_t threads)
            : threads_num_(threads) {
    }

    ThreadPool::~ThreadPool() {
        assert(stopped_.load());
    }

    void ThreadPool::Start() {
        workers_.reserve(threads_num_);
        for (size_t i = 0; i < threads_num_; ++i) {
            workers_.emplace_back([this] {
                Worker();
            });
        }
    }

    void ThreadPool::Worker() {
        current_pool = this;

        while (auto task = tasks_.Take()) {
            task.value()();
            non_finished_counter_.Done();
        }
    }

    void ThreadPool::Submit(Task task) {
        non_finished_counter_.Add(1);
        if (!tasks_.Put(std::move(task))) {
            // std::cout << "wtf\n";
        }
        // std::cout << "put";
    }

    void ThreadPool::WaitIdle() {
        non_finished_counter_.Wait();
    }

    void ThreadPool::Stop() {
        tasks_.Close();
        for (auto& worker : workers_) {
            worker.join();
        }
        stopped_.store(true);
    }

}  // namespace tp
