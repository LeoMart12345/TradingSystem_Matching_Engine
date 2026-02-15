#pragma once
#include <iostream>
#include <atomic>

class OrderIdGenerator{

    private:
    static std::atomic<u_int64_t> atomicOrderID;

    public:
    static u_int64_t incrementOrder();
    static void reset();
};