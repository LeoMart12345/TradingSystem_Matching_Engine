# Variables

#compiler 
CXX = g++

# SEPARATE flags for different builds
CXXFLAGS = -std=c++20 -Wall -Wextra -O2  # Normal build (no debug prints)

# Include directories
INCLUDES = -I. \
           -IMatching_Engine \
           -IMatching_Engine/OrderBook \
           -I"Network Servers"

LDFLAGS = -lboost_system -lpthread -lsfml-window -lsfml-system

# Directories
SRC_DIR = .
ME_DIR = Matching_Engine
OB_DIR = $(ME_DIR)/OrderBook

# Source files
SOURCES = $(SRC_DIR)/TradingSystem.cpp \
          $(ME_DIR)/MatchingEngine.cpp \
          $(OB_DIR)/OrderBook.cpp \
          $(OB_DIR)/Order.cpp

OBJECTS = $(SOURCES:.cpp=.o)

# Executable name
EXEC = TS

# RULES:
all: $(EXEC)

# Link all object files into executable
$(EXEC): $(OBJECTS)
	@echo "Linking $(EXEC)..."
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete! Run with: ./$(EXEC)"

# Compile .cpp files to .o files
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean up build files
clean:
	@echo "Cleaning up build files"
	rm -f $(OBJECTS) $(EXEC) benchmark
	@echo "Done."

# Run program
run: $(EXEC)
	@echo "Running $(EXEC)..."
	./$(EXEC)

# Debug build - ADDS -g and -DDEBUG flags
debug: CXXFLAGS += -g -DDEBUG
debug: clean $(EXEC)
	@echo "Debug build complete."

# Benchmark build - Max optimization, NO debug prints
benchmark: CXXFLAGS = -std=c++20 -Wall -Wextra -O3 -DNDEBUG
benchmark: clean
	@echo "Building benchmark (optimized, no debug prints)..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) benchmark.cpp $(SOURCES) -o benchmark $(LDFLAGS)
	@echo "Benchmark ready! Run with: ./benchmark"

# PHONY TARGETS
.PHONY: all clean run debug benchmark