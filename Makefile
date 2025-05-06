CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3
TARGET = bin\planner.exe
SRC = source\main.cpp
DATA_DIR = data

all: $(TARGET)

$(TARGET): $(SRC)
	@if not exist bin mkdir bin
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

install_deps:
	@echo Installing dependencies...
	choco install mingw -y
	@echo Please add MinGW to your PATH

clean:
	@if exist $(TARGET) del /Q $(TARGET)

run: $(TARGET)
	@$(TARGET)

.PHONY: all clean run install_deps