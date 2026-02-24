#pragma once
#include <vector>
#include <array>
#include <iostream> 
#include <stack>

template<typename T,size_t Size> 
class ObjectPool{
private: 
    std::array<T, Size> pool;
    std::stack<size_t> freeStack;
public: 
    ObjectPool(){
        for(size_t i=0; i < Size; i++){
            freeStack.push(i);
        }
    }
    
    T* acquire(){
        if(freeStack.empty())throw std::runtime_error("freeStack is full: there are no objects to acquire");
        size_t slot = freeStack.top(); // use top as pop returns void.
        freeStack.pop();
        return &pool[slot]; // get the memory address of the pool object that is next in the "freeStack" freeList
    }

    void release(T* Object){
        size_t slot = Object - &pool[0]; // pointer arithemetic to get the right slot.
        freeStack.push(slot);
    }

};