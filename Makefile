# Makefile für Pipey 1.0

# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall
LDFLAGS = -lcurl

# Ziel
TARGET = pipey

# Quelldateien
SRCS = main.cpp

# Build
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

# Clean
clean:
	rm -f $(TARGET)
