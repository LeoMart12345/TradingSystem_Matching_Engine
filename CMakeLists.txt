cmake_minimum_required(VERSION 3.20)
project(TradingSystemMatchingEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Put every executable in build/bin so the run commands stay the same.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(TS_ENABLE_ASAN "Build with AddressSanitizer" OFF)
option(TS_DEBUG_PRINTS "Enable DEBUG_PRINT output" OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra)

  if(TS_ENABLE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
  endif()

  if(TS_DEBUG_PRINTS)
    add_compile_options(-DDEBUG)
  endif()
endif()

include(CTest)

add_subdirectory(TradingSystem)
