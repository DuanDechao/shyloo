/********************************************************************
    CrazyGaze (http://www.crazygaze.com)
    Author : Rui Figueira
    Email  : rui@crazygaze.com

    purpose:

*********************************************************************/

#pragma once

#include <mutex>
#include <condition_variable>

namespace cz {

class Semaphore {
public:
    Semaphore(unsigned int count = 0) : m_count(count) {}
    void notify();
    void wait();
    bool trywait();

private:
    std::mutex m_mtx;
    std::condition_variable m_cv;
    unsigned int m_count;
};

//!
// Blocks until the counter reaches zero
class ZeroSemaphore {
public:
    ZeroSemaphore() {}
    void increment();
    void decrement();
    void wait();
    bool trywait();

private:
    std::mutex m_mtx;
    std::condition_variable m_cv;
    int m_count = 0;
};
}
