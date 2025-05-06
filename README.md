# Transport Route Planner

[![C++ Version](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/)
[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)

A C++ implementation of Dijkstra's algorithm for public transit route planning.

## Features
- Shortest path calculation between stations
- Interactive station/connection management
- CSV data persistence
- Multi-modal transport support

## Build Instructions
```bash
# Clone repository
git clone https://github.com/yu-14/Transport_Planner_cpp.git
cd Transport_Planner_cpp

# Compile
g++ -std=c++17 -Wall -Wextra -O3 src/main.cpp -o bin/planner

# Run
./bin/planner