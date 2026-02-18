# Variables
CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -O2

# Include directories
INCLUDES = -I. \
           -IMatching_Engine \
           -IMatching_Engine/OrderBook \
           -I"Network Servers" \
           -IUtilityClass \
           -IBenchmark

LDFLAGS = -lboost_system -lpthread -lsfml-window -lsfml-system

# Directories
SRC_DIR = .
ME_DIR = Matching_Engine
OB_DIR = $(ME_DIR)/OrderBook
UTIL_DIR = UtilityClass

# Library sources (includes TradingSystem.cpp but NO main function)
LIB_SOURCES = $(SRC_DIR)/TradingSystem.cpp \
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
	rm -f $(MAIN_OBJECTS) $(EXEC) benchmark client
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
benchmark: CXXFLAGS = -std=c++20 -Wall -Wextra -O3 -DNDEBUG
benchmark: clean
	@echo "Building benchmark (optimized, no debug prints)..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) Benchmark/Benchmark.cpp $(LIB_SOURCES) -o benchmark $(LDFLAGS)
	@echo "Benchmark ready! Run with: ./benchmark"

# Client build
client:
	@echo "Building the client..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) \
		"Network Servers/Client_Test.cpp" \
		$(LIB_SOURCES) \
		-o client $(LDFLAGS)
	@echo "Client ready! Run with: ./client"

# PHONY TARGETS
.PHONY: all clean run debug benchmark client