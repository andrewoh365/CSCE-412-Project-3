# Define compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -g3

# Define source files and output binary
SOURCES = main.cpp loadbalancer.cpp webserver.cpp
OBJECTS = $(SOURCES:.cpp=.o)
OUTPUT = output/main

# Default target
all: $(OUTPUT)

# Compile the program
$(OUTPUT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(OUTPUT) $(OBJECTS)

# Compile .cpp files to .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up the build
clean:
	rm -f $(OBJECTS) $(OUTPUT)
