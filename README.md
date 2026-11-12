# C++ Limit Order Book Matching Engine

A low-latency limit order book and matching engine written in C++23. Orders
arrive over TCP, are matched with price-time priority, and the resulting top of
book and trades are published over UDP multicast.

## What it does

- Accepts new and cancel requests from clients over TCP (Boost.Asio, one thread
  per session).
- Matches with price-time priority: best bid against best ask, one trade per
  `matchLimitOrders()` call.
- Finds the best price in O(1) from a per-side bitmap plus a cached best level,
  so inserts and cancels never scan the book.
- Queues resting orders in intrusive FIFO price levels, so cancelling a known
  order id is O(1) and never touches the allocator.
- Publishes a market data snapshot every 50 ms and each trade immediately, on
  the UDP multicast group `239.0.0.1:9999`.

## Repository layout

```
CMakeLists.txt              build options, warnings, sanitizer switch
TradingSystem/
  CMakeLists.txt            libraries and executables
  main.cpp                  entry point: starts the system on ports 5555/12345
  TradingSystem.{hpp,cpp}   wires the matching engine, TCP gateway and UDP
                            publisher together
  MatchingEngine/
    MatchingEngine.{hpp,cpp}  matching logic over the order book
    OrderBook/                order pool, price levels, bitmaps, trades
  Network/                  Boost.Asio transport: TCP gateway, UDP publisher,
                            market data, client and simulated traders
  Benchmark/                order book microbenchmark, end-to-end benchmark
  Tests/                    Google Test suite
  Utilities/                order id generation
  Debug/                    DEBUG_PRINT helper
```

## Dependencies

- `cmake` 3.20 or newer
- `g++` with C++23 support
- Boost 1.70+ (`libboost-dev`)
- pthreads
- Google Test (`libgtest-dev`), for the tests only

Fedora:

```bash
sudo dnf install cmake gcc-c++ boost-devel gtest-devel
```

Debian/Ubuntu:

```bash
sudo apt install cmake g++ libboost-dev libgtest-dev
```

## Build

The default build type is `Release` and every executable lands in `build/bin`.

```bash
cmake -S . -B build
cmake --build build --parallel
```

Debug build with AddressSanitizer and `DEBUG_PRINT` output:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DTS_ENABLE_ASAN=ON -DTS_DEBUG_PRINTS=ON
cmake --build build --parallel
```

## Run

The server starts a TCP order gateway, the UDP market data publisher and five
simulated traders, then matches orders until the process is killed:

```bash
./build/bin/TS
```

An interactive client that connects over TCP:

```bash
./build/bin/client
```

## Benchmarks

`benchmark_ob` measures the order book in isolation: raw INSERT, CANCEL and
MATCH latency with no sockets, no serialisation and no scheduler noise. It
reports p50, p99 and p999 for each operation.

```bash
cmake --build build --target benchmark_ob
./build/bin/benchmark_ob
```

`benchmark_sys` measures a full round trip: the client serialises an order,
sends it over TCP to the server and waits for the reply, so the numbers include
every layer of overhead.

```bash
cmake --build build --target benchmark_sys
./build/bin/benchmark_sys
```

For comparable numbers across runs, pin both the measurement and the machine
down with `taskset -c 2` and compare medians of several alternating runs rather
than single samples.

## Tests

```bash
cmake --build build --target run_tests
ctest --test-dir build --output-on-failure
```

The suite covers bid/ask price priority, insertion and cancellation, trade
generation, partial and full fills, serialisation round trips and volume
reduction. The same suite runs under AddressSanitizer with `-DTS_ENABLE_ASAN=ON`.

## Design notes

- **Best price discovery.** Each side keeps a bitmap of active price levels, so
  the best level is a bit scan rather than a search. The best bid and ask are
  also cached and only recomputed when a level empties.
- **Resting orders.** Each price level is a head/tail pair and orders are linked
  through their own `mPrev`/`mNext` pointers, so append, unlink and cancel are
  O(1) with no allocations.
- **Order storage.** Orders come from a fixed-size `ObjectPool` with a
  contiguous index free list. The pool is not internally synchronised; the
  matching engine serialises access with its own mutex.
