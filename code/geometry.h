#include <iostream>
#include <vector>
#include <cmath>
#include <set>
#include <iomanip>
#include <algorithm>
#include <cassert>
using std::cin;
using std::cout;
using std::vector;
using std::set;

#include <bits/stdc++.h>
using namespace std;


int64_t sign(int64_t a) {
    if (a == 0) return 0;
    return a / std::abs(a);
}

template <typename T>
struct vec {
    T x = 0, y = 0;
    vec& operator-=(const vec& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    vec& operator+=(const vec& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    vec& operator*=(T num) {
        x *= num;
        y *= num;
        return *this;
    }
    vec operator-(const vec<T>& v2) const {
        vec v1(*this);
        v1 -= v2;
        return v1;
    }
    vec operator+(const vec<T>& v2) const {
        vec v1(*this);
        v1 += v2;
        return v1;
    }
    vec operator*(T num) const {
        vec v1(*this);
        v1 *= num;
        return v1;
    }
    double len() const {
        return sqrt(square_len());
    }
    T square_len() const {
        return x * x + y * y;
    }
    friend std::istream& operator>>(std::istream& in, vec& v) {
        in >> v.x >> v.y;
        return in;
    }
    friend std::ostream& operator<<(std::ostream& out, const vec& v) {
        out << v.x << " " << v.y;
        return out;
    }
    T operator*(const vec& other) const {
        return x * other.x + y * other.y;
    }
    T operator[](const vec& other) const {
        return x * other.y - y * other.x;
    }
    bool operator==(const vec& other) const {
        return x == other.x && y == other.y;
    }
    bool operator<(const vec& other) const {
        return x < other.x || x == other.x && y < other.y;
    }
};

template <typename T>
struct line {
    T a, b, c;
    line() = default;
    line(const vec<T>& p1, const vec<T>& p2) {
        if (p1 == p2) {
            a = b = 1;
            c = -p1.x - p1.y;
            return;
        }
        a = p1.y - p2.y;
        b = p2.x - p1.x;
        c = p1.x * p2.y - p1.y * p2.x;
    }
    vec<T> normal_vec() const {
        return {a, b};
    }
    vec<T> direct_vec() const {
        return {-b, a};
    }
    friend std::istream& operator>>(std::istream& in, line& l) {
        in >> l.a >> l.b >> l.c;
        return in;
    }
    vec<double> intersect(const line& other) const {
        double delta = a * other.b - other.a * b;
        T delta1 = -c * other.b + other.c * b;
        T delta2 = -a * other.c + other.a * c;
        return {delta1 / delta, delta2 / delta};
    }
    bool parallel(const line& other) const {
        return abs(direct_vec()[other.direct_vec()]) == 0;
    }
    double dist(const line& other) const {
        return abs(c / direct_vec().len() - other.c / other.direct_vec().len()
                                            * (a * other.a < 0 || b * other.b < 0 ? -1 : 1));
    }
    T operator()(const vec<T>& p) const {
        return a * p.x + b * p.y + c;
    }
    bool contains(const vec<T>& p) const {
        return (*this)(p) == 0;
    }
};

template <typename T>
struct segment {
    vec<T> p1, p2;
    line<T> get_line() const {
        return line(p1, p2);
    }
    bool intersect(const segment& other) const {
        if (other.contains(p1) || other.contains(p2) || contains(other.p1) || contains(other.p2)) {
            return true;
        }
        line<T> l1 = get_line();
        line<T> l2 = other.get_line();
        return sign(l1(other.p1)) * sign(l1(other.p2)) == -1 && sign(l2(p1)) * sign(l2(p2)) == -1;
    }
    friend std::istream& operator>>(std::istream& in, segment& seg) {
        in >> seg.p1 >> seg.p2;
        return in;
    }
    bool degenerate() const {
        return p1.x == p2.x && p1.y == p2.y;
    }
    bool contains(const vec<T>& p) const {
        return get_line().contains(p) && sign(p.x - p1.x) * sign(p.x - p2.x) <= 0 && sign(p.y - p1.y) * sign(p.y - p2.y) <= 0;
    }
    bool contains(const segment& other) {
        return contains(other.p1) && contains(other.p2);
    }
};

bool inside_lower(const set<vec<int64_t>>& lower, const vec<int64_t>& point) {
    auto it = lower.lower_bound(point);
    if (it == lower.begin() || it == lower.end()) return false;
    if ((*it - *std::prev(it))[point - *std::prev(it)] <= 0) return false;
    return true;
}

bool inside_upper(const set<vec<int64_t>>& upper, const vec<int64_t>& point) {
    auto it = upper.lower_bound(point);
    if (it == upper.begin() || it == upper.end()) return false;
    return (*it - *std::prev(it))[point - *std::prev(it)] < 0;
}

template<typename T>
void shift_left(vector<T>& vec, int k) {
    int n = vec.size();
    vector<T> res(n);
    for (int i = k; i < n; ++i) {
        res[i - k] = vec[i];
    }
    for (int i = 0; i < k; ++i) {
        res[i - k + n] = vec[i];
    }
    swap(res, vec);
}

template<typename T>
int min_ind(const vector<T>& vec) {
    int ind = 0;
    auto mn = vec[0];
    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] < mn) {
            mn = vec[i];
            ind = i;
        }
    }
    return ind;
}

template<typename T>
vector<T> sum(vector<T> M1, vector<T> M2) {
    shift_left(M1, min_ind(M1));
    shift_left(M2, min_ind(M2));
    vector<T> res;
    res.push_back(M1[0] + M2[0]);
    int n = M1.size(), m = M2.size();
    for (int i = 0, j = 0; i < M1.size() || j < M2.size(); ) {
        if (i == n) {
            res.push_back(res.back() + M2[(j + 1) % m] - M2[j]);
            ++j;
        } else if (j == m) {
            res.push_back(res.back() + M1[(i + 1) % n] - M1[i]);
            ++i;
        } else {
            if ((M1[(i + 1) % n] - M1[i])[M2[(j + 1) % m] - M2[j]] > 0) {
                res.push_back(res.back() + M1[(i + 1) % n] - M1[i]);
                ++i;
            } else {
                res.push_back(res.back() + M2[(j + 1) % m] - M2[j]);
                ++j;
            }
        }
    }
    res.pop_back();
    return res;
}

double dist(vec<int64_t> p1, vec<int64_t> p2) {
    return (p2 - p1).len();
}

bool is_ok(const vector<vec<int>>& points) {
    int n = points.size();
    bool have_plus = false;
    bool have_minus = false;
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        int k = (i + 2) % n;
        vec<int> v1 = points[j] - points[i];
        vec<int> v2 = points[k] - points[j];
        if (v1[v2] > 0) {
            have_plus = true;
        } else if (v1[v2] < 0) {
            have_minus = true;
        }
    }
    return !(have_plus && have_minus);
}

uint64_t dfs(vector<vec<int>>& v, int C, int mn_y, int mx_y) {
    uint64_t ans = 0;
    if (v.size() == 0) {
//        for (int x = 0; x < C; ++x) {
//            for (int y = 0; y < C; ++y) {
//                v.push_back({x, y});
//                ans += dfs(v, C);
//                v.pop_back();
//            }
//        }
        v.push_back({0, 0});
        mn_y = mx_y = 0;
        return dfs(v, C, mn_y, mx_y);
    } else if (v.size() == 1) {
        for (int x = v[0].x + 1; x < C; ++x) {
            for (int y = mx_y - C + 1; y < mn_y + C; ++y) {
                v.push_back({x, y});
                ans += dfs(v, C, std::min(mn_y, y), std::max(mx_y, y));
                v.pop_back();
            }
        }
    } else {
        vec<int> v1 = v.back() - v[v.size() - 2];
        for (int x = 0; x < C; ++x) {
            for (int y = mx_y - C + 1; y < mn_y + C; ++y) {
                vec<int> p{x, y};
                if (p == v[0]) {
                    if (v.size() > 2) {
                        ++ans;
                        //assert(is_ok(v));
                        //cout << v.size() << "\n";
                        //for (int i = 0; i < v.size(); ++i) cout << v[i] << "\n";
                    }
                    continue;
                }
                if (v1[p - v.back()] > 0 && (p - v.back())[v[0] - p] > 0) {
                    v.push_back(p);
                    ans += dfs(v, C, std::min(mn_y, y), std::max(mx_y, y));
                    v.pop_back();
                }
            }
        }
    }
    return ans;
}

std::ostream& operator<<( std::ostream& dest, __int128_t value ) {
    std::ostream::sentry s( dest );
    if ( s ) {
        __uint128_t tmp = value < 0 ? -value : value;
        char buffer[ 128 ];
        char* d = std::end( buffer );
        do
        {
            -- d;
            *d = "0123456789"[ tmp % 10 ];
            tmp /= 10;
        } while ( tmp != 0 );
        if ( value < 0 ) {
            -- d;
            *d = '-';
        }
        int len = std::end( buffer ) - d;
        if ( dest.rdbuf()->sputn( d, len ) != len ) {
            dest.setstate( std::ios_base::badbit );
        }
    }
    return dest;
}
