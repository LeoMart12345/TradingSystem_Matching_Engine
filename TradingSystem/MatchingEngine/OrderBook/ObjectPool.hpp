// ObjectPool.hpp
#pragma once
#include <array>
#include <stdexcept>

// pre-allocates size objects of type T
// acquire() and release replace malloc and free() for the hot path.
// Not internally synchronized: callers must serialise access (MatchingEngine
// does it with its own mutex, so the pool itself only needs to keep the free
// list contiguous and cheap).
template <typename T, size_t Size> class ObjectPool {
private:
  std::array<T, Size> pool;           // fixed blocks of pre- allocated objects
  std::array<size_t, Size> freeSlots; // indices of available slots, used as a
                                      // stack so the hot end stays in cache
  size_t freeCount = Size;            // number of slots still available

public:
  ObjectPool() {
    for (size_t i = 0; i < Size; i++) {
      freeSlots[i] = i;
    }
  }

  T *acquire() {
    if (freeCount == 0)
      throw std::runtime_error(
          "freeStack is full: there are no objects to acquire");
    return &pool[freeSlots[--freeCount]];
  }

  void release(T *obj) {
    size_t slot = obj - &pool[0];
    if (slot >= Size) {
      throw std::runtime_error(
          "ObjectPool::release received pointer outside pool bounds");
    }
    freeSlots[freeCount++] = slot;
  }
};
