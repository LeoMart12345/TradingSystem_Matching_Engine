# C++ Limit Order Book Matching Engine Thesis project

A low-latency limit order book matching engine written in C++23. Orders are submitted over TCP, matched against the order book, and market data is broadcast over UDP multicast to connected clients.

## What it does

- Accepts buy/sell orders from clients over TCP
- Matches orders using price-time priority (best bid meets best ask)
- Broadcasts live market data (best bid, best ask, volumes) over UDP multicast every 50ms
- Uses a bitmap-based order book for O(1) best price discovery
- Pre-allocates order object memory at startup using an object pool


## Dependencies

- `cmake` 3.20 or newer
- `g++` with C++23 support
- Boost 1.70+ (`libboost-dev`)
- pthreads
- Google Test (`libgtest-dev`) — for tests only

**On Fedora:**
```bash
sudo dnf install cmake gcc-c++ boost-devel gtest-devel
```
# Build and Run

The default configuration is a release build. Everything is written to `build/bin`.

### Build
```bash
cmake -S . -B build
cmake --build build
```

Debug build with AddressSanitizer and `DEBUG_PRINT` output:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DTS_ENABLE_ASAN=ON -DTS_DEBUG_PRINTS=ON
cmake --build build
```

### Run the full trading system
```bash
./build/bin/TS
```

### Run a simulated client (connects over TCP)
```bash
./build/bin/client
```

---

## Benchmarks

### Order book microbenchmark (isolated, no networking)

Measures raw INSERT, CANCEL, and MATCH latency with no TCP overhead, mutex contention, or scheduler interference. This is the best-case performance of the core data structures.
```bash
cmake --build build --target benchmark_ob
./build/bin/benchmark_ob
```

Reports p50, p99, and p999 latency percentiles for each operation.

### End-to-end system benchmark (full round-trip over TCP)

Measures the full round-trip latency of a single order submission from the client serialising the order, through TCP, through the server, and back. Includes all networking and system overhead.
```bash
cmake --build build --target benchmark_sys
./build/bin/benchmark_sys
```

---

## Tests

Google Test unit tests covering order book correctness, matching engine logic, serialisation, and order operations.
```bash
cmake --build build --target run_tests
ctest --test-dir build --output-on-failure
```

Tests cover:
- Bid/ask price priority
- Order insertion and cancellation
- Trade generation on matching orders
- Partial and full fills
- Serialisation round trips
- Volume reduction

---
