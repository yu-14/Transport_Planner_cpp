#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <queue>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <cctype>
#include <windows.h>

using namespace std;

struct Station {
    string id;
    string name;
    double lat;
    double lon;
};

struct Connection {
    string to;
    double weight;
    string transport_type;
};

class TransportSystem {
    unordered_map<string, Station> stations;
    unordered_map<string, vector<Connection>> adjacency_list;

    bool is_valid_id(const string& id) const {
        if (id.empty() || id.length() > 10) return false;
        return all_of(id.begin(), id.end(), [](char c) {
            return isalnum(c) || c == '_';
        });
    }

public:
    void load_stations(const string& filename);
    void load_connections(const string& filename);
    bool station_exists(const string& id) const;
    string get_station_name(const string& id) const;
    size_t get_station_count() const { return stations.size(); }
    vector<string> find_shortest_path(const string& start, const string& end) const;
    double calculate_path_cost(const vector<string>& path) const;

    void add_station_interactive();
    void add_connection_interactive();
    void print_all_stations() const;
    void print_connections() const;
    void save_to_files() const;
    void delete_station_interactive();
    void delete_connection_interactive();
};

void TransportSystem::load_stations(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) throw runtime_error("Failed to open: " + filename);

    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        stringstream ss(line);
        Station s;
        getline(ss, s.id, ',');
        getline(ss, s.name, ',');
        ss >> s.lat;
        ss.ignore();
        ss >> s.lon;
        stations[s.id] = s;
    }
}

void TransportSystem::load_connections(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) throw runtime_error("Failed to open: " + filename);

    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        stringstream ss(line);
        Connection c;
        string from;
        getline(ss, from, ',');
        getline(ss, c.to, ',');
        getline(ss, c.transport_type, ',');
        ss >> c.weight;
        adjacency_list[from].push_back(c);
    }
}

bool TransportSystem::station_exists(const string& id) const {
    return stations.find(id) != stations.end();
}

string TransportSystem::get_station_name(const string& id) const {
    return stations.at(id).name;
}

vector<string> TransportSystem::find_shortest_path(const string& start, const string& end) const {
    struct QueueNode {
        double cost;
        string id;
        bool operator>(const QueueNode& other) const { return cost > other.cost; }
    };

    priority_queue<QueueNode, vector<QueueNode>, greater<>> pq;
    unordered_map<string, double> distances;
    unordered_map<string, string> previous;

    for (const auto& [id, _] : stations) {
        distances[id] = numeric_limits<double>::infinity();
    }
    distances[start] = 0.0;
    pq.push({0.0, start});

    while (!pq.empty()) {
        auto [current_cost, current_id] = pq.top();
        pq.pop();

        if (current_id == end) break;
        if (current_cost > distances[current_id]) continue;

        for (const auto& conn : adjacency_list.at(current_id)) {
            double new_cost = current_cost + conn.weight;
            if (new_cost < distances[conn.to]) {
                distances[conn.to] = new_cost;
                previous[conn.to] = current_id;
                pq.push({new_cost, conn.to});
            }
        }
    }

    vector<string> path;
    string current = end;
    
    if (previous.find(current) == previous.end() && current != start) {
        return {};
    }
    
    while (!current.empty()) {
        path.push_back(current);
        if (previous.find(current) == previous.end()) break;
        current = previous[current];
    }
    reverse(path.begin(), path.end());

    return path;
}

double TransportSystem::calculate_path_cost(const vector<string>& path) const {
    double total = 0.0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        for (const auto& conn : adjacency_list.at(path[i])) {
            if (conn.to == path[i+1]) {
                total += conn.weight;
                break;
            }
        }
    }
    return total;
}

void TransportSystem::add_station_interactive() {
    Station s;
    cout << "\n=== Add New Station ===\n";
    
    while (true) {
        cout << "Enter Station ID (alphanumeric, max 10 chars): ";
        cin >> s.id;
        
        if (!is_valid_id(s.id)) {
            cout << "Invalid ID format! Use only letters/numbers/underscores.\n";
            continue;
        }
        
        if (station_exists(s.id)) {
            cout << "Station ID already exists!\n";
            continue;
        }
        break;
    }

    cin.ignore();
    cout << "Enter Station Name: ";
    getline(cin, s.name);
    
    cout << "Enter Latitude (-90 to 90): ";
    while (!(cin >> s.lat) || s.lat < -90 || s.lat > 90) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid! Enter between -90 and 90: ";
    }
    
    cout << "Enter Longitude (-180 to 180): ";
    while (!(cin >> s.lon) || s.lon < -180 || s.lon > 180) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid! Enter between -180 and 180: ";
    }

    stations[s.id] = s;
    cout << "Station '" << s.id << "' added successfully!\n";
}

void TransportSystem::add_connection_interactive() {
    cout << "\n=== Add New Connection ===\n";
    
    string from, to, transport;
    double weight;
    
    while (true) {
        cout << "Enter FROM Station ID: ";
        cin >> from;
        if (!station_exists(from)) {
            cout << "Station doesn't exist! Available stations:\n";
            print_all_stations();
            continue;
        }
        break;
    }
    
    while (true) {
        cout << "Enter TO Station ID: ";
        cin >> to;
        if (!station_exists(to)) {
            cout << "Station doesn't exist! Available stations:\n";
            print_all_stations();
            continue;
        }
        break;
    }
    
    cout << "Enter Transport Type: ";
    cin >> transport;
    
    cout << "Enter Travel Weight (positive number): ";
    while (!(cin >> weight) || weight <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid! Enter positive number: ";
    }

    adjacency_list[from].push_back({to, weight, transport});
    cout << "Connection added successfully!\n";
}

void TransportSystem::print_all_stations() const {
    cout << "\n=== All Stations (" << stations.size() << ") ===\n";
    cout << left << setw(10) << "ID" << setw(25) << "Name" 
         << "Coordinates\n" << string(60, '-') << endl;
    
    for (const auto& [id, station] : stations) {
        cout << setw(10) << id << setw(25) 
             << (station.name.length() > 24 ? station.name.substr(0, 24) + "..." : station.name)
             << fixed << setprecision(6) << "(" << station.lat << ", " << station.lon << ")\n";
    }
}

void TransportSystem::print_connections() const {
    cout << "\n=== All Connections ===\n";
    for (const auto& [from, connections] : adjacency_list) {
        for (const auto& conn : connections) {
            cout << from << " (" << get_station_name(from) << ") → "
                 << conn.to << " (" << get_station_name(conn.to) << ") via "
                 << conn.transport_type << " (" << conn.weight << ")\n";
        }
    }
}

void TransportSystem::save_to_files() const {
    ofstream station_file("data/stations.csv");
    if (!station_file) {
        cerr << "Error: Couldn't create stations file!\n";
        return;
    }
    station_file << "id,name,lat,lon\n";
    for (const auto& [id, station] : stations) {
        station_file << id << "," << station.name << ","
                    << fixed << setprecision(6) << station.lat << ","
                    << station.lon << "\n";
    }

    ofstream connection_file("data/connections.csv");
    if (!connection_file) {
        cerr << "Error: Couldn't create connections file!\n";
        return;
    }
    connection_file << "from,to,transport_type,weight\n";
    for (const auto& [from, connections] : adjacency_list) {
        for (const auto& conn : connections) {
            connection_file << from << "," << conn.to << ","
                          << conn.transport_type << "," << conn.weight << "\n";
        }
    }

    cout << "\nData saved to files successfully!\n";
}

void TransportSystem::delete_station_interactive() {
    print_all_stations();
    string id;
    cout << "\nEnter Station ID to delete (or 'cancel'): ";
    cin >> id;
    
    if (id == "cancel") return;
    if (!station_exists(id)) {
        cout << "Station doesn't exist!\n";
        return;
    }
    
    cout << "Confirm delete " << id << " (" << get_station_name(id) << ")? (y/n): ";
    char choice;
    cin >> choice;
    if (tolower(choice) != 'y') return;
    
    stations.erase(id);
    adjacency_list.erase(id);
    for (auto& [_, connections] : adjacency_list) {
        connections.erase(
            remove_if(connections.begin(), connections.end(),
                [&id](const Connection& c) { return c.to == id; }),
            connections.end()
        );
    }
    cout << "Station deleted!\n";
}

void TransportSystem::delete_connection_interactive() {
    print_connections();
    string from, to;
    cout << "\nEnter FROM Station ID: ";
    cin >> from;
    cout << "Enter TO Station ID: ";
    cin >> to;
    
    if (!station_exists(from) || !station_exists(to)) {
        cout << "Invalid station IDs!\n";
        return;
    }
    
    auto& connections = adjacency_list[from];
    size_t before = connections.size();
    connections.erase(
        remove_if(connections.begin(), connections.end(),
            [&to](const Connection& c) { return c.to == to; }),
        connections.end()
    );
    
    cout << (connections.size() < before ? "Connection deleted!\n" : "No connection found!\n");
}

void display_menu() {
    cout << "\n===== Transport Route Planner =====\n"
         << "1. Add Station\n2. Add Connection\n3. View Stations\n"
         << "4. View Connections\n5. Find Path\n6. Delete Station\n"
         << "7. Delete Connection\n8. Save Data\n0. Exit\n"
         << "Choice: ";
}

int main() {
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, 800, 600, TRUE);

    cout << "=== TRANSPORT ROUTE PLANNER ===\n";
    cout << "Initializing system...\n\n";

    TransportSystem ts;  // Changed variable name from 'system' to 'ts'
    
    try {
        cout << "Loading station data... ";
        ts.load_stations("data/stations.csv");
        cout << "Done!\n";

        cout << "Loading connection data... ";
        ts.load_connections("data/connections.csv");
        cout << "Done!\n\n";

        cout << "System ready! Found " << ts.get_station_count() << " stations.\n";
    } catch (const exception& e) {
        cerr << "\nFATAL ERROR: " << e.what() << endl;
        cerr << "Check that data files exist in the data/ folder\n";
        system("pause");
        return 1;
    }

    // Main menu loop
    int choice = -1;
    while (choice != 0) {
        cout << "\n===== MAIN MENU =====\n"
             << "1. Add Station\n2. Add Connection\n3. View Stations\n"
             << "4. View Connections\n5. Find Path\n6. Delete Station\n"
             << "7. Delete Connection\n8. Save Data\n0. Exit\n"
             << "Enter choice: ";
        
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1: ts.add_station_interactive(); break;
            case 2: ts.add_connection_interactive(); break;
            case 3: ts.print_all_stations(); break;
            case 4: ts.print_connections(); break;
            case 5: {
                string start, end;
                cout << "Start Station ID: ";
                cin >> start;
                cout << "End Station ID: ";
                cin >> end;
                
                if (!ts.station_exists(start) || !ts.station_exists(end)) {
                    cout << "Invalid stations!\n";
                    break;
                }
                
                auto path = ts.find_shortest_path(start, end);
                if (path.empty()) {
                    cout << "No path exists!\n";
                } else {
                    cout << "\nOptimal Route:\n";
                    for (size_t i = 0; i < path.size(); ++i) {
                        cout << (i > 0 ? " → " : "") << path[i] 
                             << " (" << ts.get_station_name(path[i]) << ")";
                    }
                    cout << "\nTotal cost: " << ts.calculate_path_cost(path) << "\n";
                }
                break;
            }
            case 6: ts.delete_station_interactive(); break;
            case 7: ts.delete_connection_interactive(); break;
            case 8: ts.save_to_files(); break;
            case 0: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice!\n";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "\nProgram completed. Press Enter to exit...";
    cin.ignore();
    cin.get();
    return 0;
}