#include "OrderIdGenerator.hpp"

std::atomic<u_int64_t> OrderIdGenerator::atomicOrderID{0};

u_int64_t OrderIdGenerator::incrementOrder(){
    return ++atomicOrderID;
}

void OrderIdGenerator::reset(){
    atomicOrderID = 0;
}