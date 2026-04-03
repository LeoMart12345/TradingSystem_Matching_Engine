# Variables
CXX = g++

CXXFLAGS = -std=c++23 -Wall -Wextra -g -O0 -fsanitize=address
# Include directories
INCLUDES = -I. \
           -IMatching_Engine \
           -IMatching_Engine/OrderBook \
           -I"Network Servers" \
           -IUtilityClass \
           -IBenchmark

LDFLAGS = -lboost_system -lpthread -lsfml-window -lsfml-system -fsanitize=address

# Directories
SRC_DIR = .
ME_DIR = Matching_Engine
OB_DIR = $(ME_DIR)/OrderBook
UTIL_DIR = UtilityClass

# Library sources
LIB_SOURCES = $(SRC_DIR)/TradingSystem.cpp \
              $(ME_DIR)/MatchingEngine.cpp \
              $(OB_DIR)/OrderBook.cpp \
              $(OB_DIR)/Order.cpp \
              $(UTIL_DIR)/OrderIdGenerator.cpp

# OrderBook benchmark sources — no TradingSystem, no networking involved
OB_BENCH_SOURCES = Benchmark/OrderBook_Benchmark.cpp \
                   $(ME_DIR)/MatchingEngine.cpp \
                   $(OB_DIR)/OrderBook.cpp \
                   $(OB_DIR)/Order.cpp \
                   $(UTIL_DIR)/OrderIdGenerator.cpp

# Main program sources (main.cpp + library sources)
MAIN_SOURCES = $(SRC_DIR)/main.cpp $(LIB_SOURCES)

MAIN_OBJECTS = $(MAIN_SOURCES:.cpp=.o)

# Executable name
EXEC = TS

# RULES:
all: $(EXEC)

# Link main program
$(EXEC): $(MAIN_OBJECTS)
	@echo "Linking $(EXEC)..."
	$(CXX) $(MAIN_OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete! Run with: ./$(EXEC)"

# Compile .cpp files to .o files
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean up build files
clean:
	@echo "Cleaning up build files"
	rm -f $(MAIN_OBJECTS) $(EXEC) benchmark benchmark_ob client
	@echo "Done."

# Run program
run: $(EXEC)
	@echo "Running $(EXEC)..."
	./$(EXEC)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: clean $(EXEC)
	@echo "Debug build complete."

# Benchmark build - Benchmark.cpp + library sources (no main.cpp)
benchmark: CXXFLAGS = -std=c++23 -Wall -Wextra -O3 -DNDEBUG
benchmark: clean
	@echo "Building benchmark (optimized, no debug prints)..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) Benchmark/Benchmark.cpp $(LIB_SOURCES) -o benchmark $(LDFLAGS)
	@echo "Benchmark ready! Run with: ./benchmark"

# OrderBook microbenchmark — no TradingSystem, no asan, no boost/sfml
benchmark_ob: CXXFLAGS = -std=c++23 -Wall -Wextra -O3 -DNDEBUG
benchmark_ob:
	@echo "Building OrderBook benchmark..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) Benchmark/OrderBook_Benchmark.cpp $(ME_DIR)/MatchingEngine.cpp $(OB_DIR)/OrderBook.cpp $(OB_DIR)/Order.cpp $(UTIL_DIR)/OrderIdGenerator.cpp -o benchmark_ob -lpthread
	@echo "Ready! Run with: ./benchmark_ob"

# Client build
client:
	@echo "Building the client..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) \
		"Network Servers/TraderClient.cpp" \
		$(LIB_SOURCES) \
		-o client $(LDFLAGS)
	@echo "Client ready! Run with: ./client"

# System benchmark - TCP round-trip latency.
benchmark_sys: CXXFLAGS = -std=c++23 -Wall -Wextra -O3 -DNDEBUG -g -fno-omit-frame-pointer
benchmark_sys:
	@echo "Building system benchmark..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) \
		Benchmark/System_Benchmark.cpp \
		$(ME_DIR)/MatchingEngine.cpp \
		$(OB_DIR)/OrderBook.cpp \
		$(OB_DIR)/Order.cpp \
		$(UTIL_DIR)/OrderIdGenerator.cpp \
		-o benchmark_sys -lboost_system -lpthread
	@echo "Ready! Run with: ./benchmark_sys"

# PHONY TARGETS
.PHONY: all clean run debug benchmark benchmark_ob benchmark_sys client
