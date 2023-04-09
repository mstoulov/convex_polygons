#pragma once

#include "bits/stdc++.h"
#include <optional>
#include <deque>

namespace tp {

// Unbounded blocking multi-producers/multi-consumers (MPMC) queue

    template <typename T>
    class UnboundedBlockingQueue {
    public:
        bool Put(T value) {
            std::lock_guard guard(mutex_);
            if (closed_) {
                return false;
            }
            buffer_.push_back(std::move(value));
            empty_.notify_one();
            return true;
        }

        std::optional<T> Take() {
            std::unique_lock lock(mutex_);
            while (!closed_ && buffer_.empty()) {
                empty_.wait(lock);
            }
            if (buffer_.empty()) {
                return std::nullopt;
            }
            T res = std::move(buffer_.front());
            buffer_.pop_front();
            return std::move(res);
        }

        void Close() {
            std::lock_guard guard(mutex_);
            closed_ = true;
            empty_.notify_all();
        }

    private:
        std::deque<T> buffer_;
        std::mutex mutex_;
        std::condition_variable empty_;
        bool closed_{false};
    };

}  // namespace tp
