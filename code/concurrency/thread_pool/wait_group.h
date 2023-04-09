#pragma once

#include "bits/stdc++.h"


namespace tp {

    class WaitGroup {
    public:
        // += count
        void Add(size_t count) {
            empty_.store(0);
            counter_.fetch_add(count);
        }

        // =- 1
        void Done() {
            if (counter_.fetch_sub(1) == 1) {
                empty_.store(1);
                empty_.notify_all();
            }
        }

        // == 0
        // One-shot
        void Wait() {
            while (empty_.load() != 1) {
                empty_.wait(0);
            }
        }

    private:
        std::atomic<uint32_t> counter_{0};
        std::atomic<uint32_t> empty_{1};
    };

}
