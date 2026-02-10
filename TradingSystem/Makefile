# Variables

#compiler 
CXX = g++

CXXFLAGS = -std=c++20 -Wall -Wextra -O2

# Include directories
INCLUDES = -I. \
           -IMatching_Engine \
           -IMatching_Engine/OrderBook \
           -I"Network Servers"


LDFLAGS = -lboost_system -lpthread

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

# execcutable name
EXEC = TS

# RULES:
all: $(EXEC)

# Link all object files into executable
$(EXEC): $(OBJECTS)
	@echo "Linking $(EXEC)..."
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete! Run with: ./$(EXEC)"

# Compile .cpp files to .o files
# This rule says: "any .o file depends on its .cpp file"
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean up build files
clean:
	@echo "Cleaning up..."
	rm -f $(OBJECTS) $(EXEC)
	@echo "Done."

# Run the program
run: $(EXEC)
	@echo "Running $(EXEC)..."
	./$(EXEC)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: clean $(EXEC)
	@echo "Debug build complete."



# PHONY TARGETS
.PHONY: all clean run debug