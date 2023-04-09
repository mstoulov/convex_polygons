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
