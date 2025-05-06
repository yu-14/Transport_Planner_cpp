# Public Transport Route Planner 🚇

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![License](https://img.shields.io/badge/license-MIT-blue)

Advanced route planning system using Dijkstra's algorithm with real-world transit data.

## Features ✨
- 🗺️ Multi-modal transport (subway, bus, tram, walking)
- ⏱️ Time and distance calculations
- 📍 Real-world coordinates support
- 💾 Automatic data persistence
- 🔍 Interactive station/route management

## Installation 📦
```bash
make install_deps  # Install compiler
make              # Build project
```

## Usage 🖥️
```bash
make run
```

Sample commands:
1. Add station
2. Find route from S1 to S5
3. View all connections

## Data Format 📊
### Stations
```csv
id,name,lat,lon,zone
S1,Central Station,12.3456,78.9012,1
```

### Connections
```csv
from,to,transport_type,weight(mins),distance(km),frequency(mins)
S1,S2,subway,5,2.5,3
```

## Contributing 🤝
Pull requests welcome! For major changes, please open an issue first.

## License 📄
MIT