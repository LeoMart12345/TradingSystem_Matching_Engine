#include "OrderIdGenerator.hpp"

static std::atomic<int> OrderIdGenerator{0};

u_int64_t OrderIdGenerator::incrementOrder(){
    return ++atomicOrderID;
}

void OrderIdGenerator::reset(){
    atomicOrderID = 0;
}