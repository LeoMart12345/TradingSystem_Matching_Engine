find_package(Threads REQUIRED)
find_package(Boost 1.70 REQUIRED)

# Order book core. No Boost and no networking, so the microbenchmark can link
# against it on its own.
add_library(orderbook STATIC
  MatchingEngine/MatchingEngine.cpp
  MatchingEngine/OrderBook/OrderBook.cpp
  MatchingEngine/OrderBook/Order.cpp
  Utilities/OrderIdGenerator.cpp
)

target_include_directories(orderbook PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/MatchingEngine
  ${CMAKE_CURRENT_SOURCE_DIR}/MatchingEngine/OrderBook
)

target_link_libraries(orderbook PUBLIC Threads::Threads)

add_library(trading_system STATIC TradingSystem.cpp)
target_link_libraries(trading_system PUBLIC orderbook Boost::headers)

add_executable(TS main.cpp)
target_link_libraries(TS PRIVATE trading_system)

add_executable(client Network/TraderClient.cpp)
target_link_libraries(client PRIVATE trading_system)

add_executable(benchmark_ob Benchmark/OrderBook_Benchmark.cpp)
target_link_libraries(benchmark_ob PRIVATE orderbook)

add_executable(benchmark_sys Benchmark/System_Benchmark.cpp)
target_link_libraries(benchmark_sys PRIVATE orderbook Boost::headers)

if(BUILD_TESTING)
  find_package(GTest)

  if(GTest_FOUND)
    add_executable(run_tests Tests/TradingSystem_Test.cpp)
    target_link_libraries(run_tests PRIVATE orderbook GTest::gtest_main)
    add_test(NAME trading_system_tests COMMAND run_tests)
  else()
    message(WARNING "Google Test not found, skipping unit tests")
  endif()
endif()
