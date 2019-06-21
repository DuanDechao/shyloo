#include <mutex>
#include <stdlib.h>  // For random(), RAND_MAX
#include <cmath>
#include <iostream>

#pragma once

// Copied from
// http://stackoverflow.com/questions/2509679/how-to-generate-a-random-number-from-within-a-range
// Assumes 0 <= max <= RAND_MAX
// Returns in the half-open interval [0, max]
inline long random_at_most(long max) {
    unsigned long
        // max <= RAND_MAX < ULONG_MAX, so this is okay.
        num_bins = (unsigned long)max + 1,
        num_rand = (unsigned long)RAND_MAX + 1, bin_size = num_rand / num_bins,
        defect = num_rand % num_bins;

    long x;
    do {
        // x = random();
        x = rand();
    }
    // This is carefully written not to overflow
    while (num_rand - defect <= (unsigned long)x);

    // Truncated division is intentional
    return x / bin_size;
}

inline double nowMs() {
    using namespace std::chrono;
    static steady_clock::time_point start = steady_clock::now();
    return duration<double, std::milli>(steady_clock::now() - start).count();
}

//
// Class to spin the CPU for a specified time.
struct Spinner {
public:
    Spinner() {
        // A one-off calibration, by spinning for a while to calculate how many
        // iterations we can do per milliseconds
        auto start = nowMs();
        const unsigned int count = 1000000;
        spin(count);
        auto elapsed = nowMs() - start;
        countPerMs = count / elapsed;
    }

    void spinMs(unsigned int ms) {
        spin((unsigned int)(ms * countPerMs));
    }

private:
    void spin(unsigned int count) {
        double r = 100000;
        while (count--) r /= std::sqrt(double(count));

        res = r;
    }

    double countPerMs;
    static thread_local double res;
};

template <class Clock>
void display_precision() {
    typedef std::chrono::duration<double, std::nano> NS;
    NS ns = typename Clock::duration(1);
    std::cout << ns.count() << " ns\n";
}

template <typename... Args>
const char* formatStr(const char* fmt, Args... args) {
    static thread_local char buf[256];
    sprintf(buf, fmt, args...);
    return buf;
}
