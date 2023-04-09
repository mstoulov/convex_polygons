#pragma once
#include "bits/stdc++.h"


namespace tp {

    class WaitGroup {
    public:
        // += count
        void Add(size_t count) {
            counter_.fetch_add(count);
            state_.store(State::NonEmpty | State::InAdd);
        }

        // =- 1
        void Done() {
            state_.store(State::NonEmpty | State::InDone);

            if (counter_.fetch_sub(1) == 1) {
                size_t expected = State::NonEmpty | State::InDone;

                if (state_.compare_exchange_strong(expected, State::Empty)) {
                    GetEmptinessState().notify_all();
                }
            }
        }

        // == 0
        // One-shot
        void Wait() {
            while (GetEmptinessState().load() == State::NonEmpty) {
                GetEmptinessState().wait(State::NonEmpty);
            }
        }

    private:
        enum State : size_t {
            Empty = 0,
            NonEmpty = 1,
            InAdd = 0,
            InDone = 1ull << 32  // doesn't affect on Emptiness (uint32_t) for futex
        };

        atomic<uint32_t>& GetEmptinessState() {
            return *reinterpret_cast<atomic<uint32_t>*>(&state_);
        }

    private:
        atomic<size_t> counter_{0};
        atomic<size_t> state_{State::Empty};
    };

}
