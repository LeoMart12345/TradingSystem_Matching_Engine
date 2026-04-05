# C++ Limit Order Book Matching Engine Thesis project

A low-latency limit order book matching engine written in C++23. Orders are submitted over TCP, matched against the order book, and market data is broadcast over UDP multicast to connected clients.

## What it does

- Accepts buy/sell orders from clients over TCP
- Matches orders using price-time priority (best bid meets best ask)
- Broadcasts live market data (best bid, best ask, volumes) over UDP multicast every 50ms
- Uses a bitmap-based order book for O(1) best price discovery
- Pre-allocates order object memory at startup using an object pool


## Dependencies

- `g++` with C++23 support
- Boost.Asio (`libboost-dev`)
- pthreads
- Google Test (`libgtest-dev`) — for tests only

**On Fedora:**
```bash
sudo dnf install boost-devel gtest-devel
```
