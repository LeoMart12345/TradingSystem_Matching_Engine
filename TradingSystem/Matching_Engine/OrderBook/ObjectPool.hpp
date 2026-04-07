// ObjectPool.hpp
#pragma once
#include <array>
#include <mutex>
#include <stack>

// pre-allocates size objects of type T
// acquire() and release replace malloc and free() for the hot path
template <typename T, size_t Size> class ObjectPool {
private:
  std::array<T, Size> pool;     // fixed blocks of pre- allocated objects
  std::stack<size_t> freeStack; // indices of available slots
  std::mutex poolMutex;

public:
  ObjectPool() {
    for (size_t i = 0; i < Size; i++) {
      freeStack.push(i);
    }
  }

  T *acquire() {
    std::lock_guard<std::mutex> lock(poolMutex);
    if (freeStack.empty())
      throw std::runtime_error(
          "freeStack is full: there are no objects to acquire");
    size_t slot = freeStack.top(); // using top as pop returns void.
    freeStack.pop();
    return &pool[slot];
  }

  void release(T *obj) {
    std::lock_guard<std::mutex> lock(poolMutex);
    size_t slot = obj - &pool[0];
    if (slot >= Size) {
      throw std::runtime_error(
          "ObjectPool::release received pointer outside pool bounds");
    }
    freeStack.push(slot);
  }
};
