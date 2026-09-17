CXX := c++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Iinclude

BUILD_DIR := build

SOURCES :=  src/main.cpp src/memory.cpp src/cpu.cpp src/fetch.cpp src/decode.cpp src/execute.cpp
TARGET := $(BUILD_DIR)/app

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET)
