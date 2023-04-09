#pragma once
#include "bits/stdc++.h"

class CyclicBarrier {
public:
    explicit CyclicBarrier(uint32_t participants)
            : participants_(participants) {
    }

    void ArriveAndWait() {
        uint32_t my_wave = wave_.load();

        if (counter_.fetch_add(1) == participants_ - 1) {
            counter_.store(0);
            wave_.fetch_add(1);
            wave_.notify_all();
        } else {
            while (wave_.load() == my_wave) {
                wave_.wait(my_wave);
            }
        }
    }

private:
    const size_t participants_;

    std::atomic<uint32_t> counter_{0};
    std::atomic<uint32_t> wave_{0};
};
