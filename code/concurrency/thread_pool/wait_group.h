#pragma once
#include "bits/stdc++.h"


namespace tp {

    class WaitGroup {
    public:
        // += count
        void Add(size_t count) {
            std::lock_guard guard(mutex_);
            counter_ += count;
            empty_ = false;
        }

        // =- 1
        void Done() {
            std::unique_lock lock(mutex_);
            if (--counter_ == 0) {
                empty_ = true;
                is_empty_.notify_all();
            }
        }

        // == 0
        // One-shot
        void Wait() {
            std::unique_lock lock(mutex_);
            while (!empty_) {
                is_empty_.wait(lock);
            }
        }

    private:
        size_t counter_{0};
        bool empty_{true};
        std::mutex mutex_;
        std::condition_variable is_empty_;
    };

}
