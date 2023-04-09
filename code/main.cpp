#include <bits/stdc++.h>
#include "geometry.h"
#include "concurrency/thread_pool/thread_pool.h"
#include "concurrency/barrier.h"
#include "aligned_allocator.h"

using namespace std;


void count_and_write_polygons(int C) {
    using ans_t = double;
    vector<vec<int>> vv;
    vv.push_back({0, 1});
    for (int vx = 1; vx < C; ++vx) {
        for (int vy = 1; vy < C; ++vy) {
            int g = gcd(vx, vy);
            if (g == 1) {
                vv.push_back({vx, vy});
            }
        }
    }
    cout << vv.size() << "\n";
    sort(vv.begin(), vv.end(), [](const auto &v1, const auto &v2) {
        return v1[v2] < 0;
    });

    vector<vector<ans_t, AlignedAllocator<ans_t, Alignment::AVX>>>
            prev_dp(C, vector<ans_t, AlignedAllocator<ans_t, Alignment::AVX>>(C + 5));
    vector<vector<ans_t, AlignedAllocator<ans_t, Alignment::AVX>>>
            dp_convex_quarter(C, vector<ans_t, AlignedAllocator<ans_t, Alignment::AVX>>(C + 5));

    dp_convex_quarter[0][0] = prev_dp[0][0] = 1;

    const int thread_cnt = 8;
    const int kSize = (C - 1) / thread_cnt + 1;

    tp::ThreadPool pool(thread_cnt);
    pool.Start();

    CyclicBarrier barrier(thread_cnt);

    auto st = chrono::high_resolution_clock::now();

    for (int thread_num = 0; thread_num < thread_cnt; ++thread_num) {
        pool.Submit([&dp_convex_quarter, &prev_dp, &vv, &barrier, thread_num, kSize, C]() mutable {
            for (auto vec : vv) {
                for (int dx = vec.x, dy = vec.y; dx < C && dy < C; dx += vec.x, dy += vec.y) {
                    int x_block = (C - dx - 1) / thread_cnt + 1;
                    for (int lx = thread_num * x_block, rx = lx + dx;
                            rx < C && lx < (thread_num + 1) * x_block; ++lx, ++rx) {
                        for (int ly = 0, ry = dy; ry < C; ly += 1, ry += 1) {
                            dp_convex_quarter[rx][ry] += prev_dp[lx][ly];
                        }
                    }
                    barrier.ArriveAndWait();
                }

                for (int i = kSize * thread_num; i < min(C, kSize * (thread_num + 1)); ++i) {
                    for (int j = 0; j < C; j += 1) {
                        prev_dp[i][j] = dp_convex_quarter[i][j];
                    }
                }

                barrier.ArriveAndWait();
            }
        });
    }

    pool.WaitIdle();
    cout << "first part in: " << (chrono::high_resolution_clock::now() - st).count() / 1e9 << " seconds\n";

    st = chrono::high_resolution_clock::now();

    vector<vector<ans_t, AlignedAllocator<ans_t, Alignment::AVX>>>
            convex_half(C, vector<ans_t, AlignedAllocator<ans_t, Alignment::AVX>>(C));

    vector<vector<vector<ans_t, AlignedAllocator<ans_t, Alignment::AVX>>>>
            per_thread_accurate(thread_cnt, vector<vector<ans_t, AlignedAllocator<ans_t, Alignment::AVX>>>
            (C, vector<ans_t, AlignedAllocator<ans_t, Alignment::AVX>>(C, 0)));

    const ans_t multiplier = 1e-300; // use all range of double
    for (int thread_num = 0; thread_num < thread_cnt; ++thread_num) {
        pool.Submit([&dp_convex_quarter, &convex_half, &barrier, &accurate = per_thread_accurate[thread_num],
                            thread_num, kSize, C, multiplier]() mutable {
            for (int x = 0; x < C; ++x) {
                for (int l = thread_num * kSize; l < min(C, kSize * (thread_num + 1)); ++l) {
                    convex_half[l].assign(C, 0);
                    for (int i = 0; i <= x; ++i) {
                        for (int r = 0; r < C; ++r) {
                            convex_half[l][r] += dp_convex_quarter[x - i][r] * dp_convex_quarter[l][i];
                        }
                    }
                }
                barrier.ArriveAndWait();

                for (int c = thread_num * kSize; c < min(C, kSize * (thread_num + 1)); ++c) {
                    for (int y1 = 0; y1 < C; ++y1) {
                        for (int y = max(c, y1); y < C; ++y) {
                            accurate[x][y] += multiplier * convex_half[y1][y - c] * convex_half[c][y - y1];
                        }
                    }
                }
                barrier.ArriveAndWait();
            }
        });
    }

    pool.WaitIdle();
    pool.Stop();
    cout << "second part in " << (chrono::high_resolution_clock::now() - st).count() / 1e9 << " seconds\n";

    using final_ans_t = long double;
    vector<vector<final_ans_t, AlignedAllocator<final_ans_t, Alignment::AVX>>>
            accurate(C, vector<final_ans_t, AlignedAllocator<final_ans_t, Alignment::AVX>>(C, -2)); // not take into account rectangle diagonals

    for (int i = 0; i < C; ++i) {
        accurate[i][0] += 1;
        accurate[0][i] += 1;
    }
    accurate[0][0] -= 1;

    const final_ans_t inv_multiplier = 1e300;
    for (int k = 0; k < thread_cnt; ++k) {
        for (int i = 0; i < C; ++i) {
            for (int j = 0; j < C; ++j) {
                accurate[i][j] += inv_multiplier * per_thread_accurate[k][i][j];
            }
        }
    }

    vector<final_ans_t> square(C, 0);
    vector<pair<final_ans_t, pair<int, int>>> sorted_accurate; // for double accuracy
    for (int i = 0; i < C; ++i) {
        for (int j = 0; j < C; ++j) {
            sorted_accurate.push_back({accurate[i][j], {i, j}});
        }
    }
    sort(sorted_accurate.begin(), sorted_accurate.end());
    for (int i = 1; i < C; ++i) {
        for (const auto& elem : sorted_accurate) {
            if (elem.second.first <= i && elem.second.second <= i) {
                square[i] += elem.first;
            }
        }
    }

    ofstream out_accurate("../../data/accurate.txt");
    out_accurate << setprecision(15);
    for (int i = 0; i < C; ++i) {
        for (int j = 0; j < C; ++j) {
            out_accurate << i << " " << j << " " << accurate[i][j] << "\n";
        }
    }

    ofstream out_square("../../data/square.txt");
    out_square << setprecision(15);
    for (int i = 0; i < C; ++i) {
        out_square << i + 1 << " " << square[i] << "\n";
    }
    out_square << "\n";
    for (int i = 2; i < C; ++i) {
        out_square << i + 1 << " " << square[i] / square[i - 1] << "\n";
    }
}

int main() {
    auto st = chrono::high_resolution_clock::now();
    count_and_write_polygons(100);
    cout << "total time " << (chrono::high_resolution_clock::now() - st).count() / 1e9 << "\n";
    return 0;
}