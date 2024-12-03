#include "json.hpp"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <stack>
#include <queue>
#include <algorithm>

using json = nlohmann::json;
using namespace std;

// DFS, Mark visited Nodes                   
void dfs(const string& node, unordered_map<string, unordered_set<string>>& adj_list, unordered_set<string>& visited) {
    stack<string> to_visit;
    to_visit.push(node);

    while (!to_visit.empty()) {
        string current = to_visit.top();
        to_visit.pop();

        if (visited.count(current) == 0) {
            visited.insert(current);

            // Add Neighbors to stack
            for (const auto& neighbor : adj_list[current]) {
                if (visited.count(neighbor) == 0) {
                    to_visit.push(neighbor);
                }
            }
        }
    }
}

// BFS Function
int shortest_path(const string& actor_a, const string& actor_b, unordered_map<string, unordered_set<string>>& adj_list) {
    if (adj_list.find(actor_a) == adj_list.end() || adj_list.find(actor_b) == adj_list.end()) {
        return -1; // One or both actors are not in the graph
    }

    if (actor_a == actor_b) {
        return 0; // Same actor
    }

    unordered_set<string> visited;
    queue<pair<string, int>> to_visit; 
    to_visit.push({actor_a, 0});

    while (!to_visit.empty()) {
        auto [current, distance] = to_visit.front();
        to_visit.pop();

        if (current == actor_b) {
            return distance;
        }

        visited.insert(current);

        for (const auto& neighbor : adj_list[current]) {
            if (visited.count(neighbor) == 0) {
                to_visit.push({neighbor, distance + 1});
            }
        }
    }

    return -1; // No path found
}

// BFS with path tracking
vector<string> shortest_path_2(const string& actor_a, const string& actor_b, unordered_map<string, unordered_set<string>>& adj_list) {
    if (adj_list.find(actor_a) == adj_list.end() || adj_list.find(actor_b) == adj_list.end()) {
        return {"No path"}; // One or both actors are not in the graph
    }

    if (actor_a == actor_b) {
        return {actor_a}; // Same actor
    }

    unordered_set<string> visited;
    queue<pair<string, vector<string>>> to_visit; 
    to_visit.push({actor_a, {actor_a}});

    while (!to_visit.empty()) {
        auto [current, path] = to_visit.front();
        to_visit.pop();

        if (current == actor_b) {
            return path;
        }

        visited.insert(current);

        for (const auto& neighbor : adj_list[current]) {
            if (visited.count(neighbor) == 0) {
                vector<string> new_path = path;
                new_path.push_back(neighbor);
                to_visit.push({neighbor, new_path});
            }
        }
    }

    return {"No path"}; // No path was found
}

int main() {
    // Load JSON 
    ifstream file("tmdb_5000_credits.json");
    if (!file.is_open()) {
        cerr << "Failed to open file." << endl;
        return 1;
    }

    json data;
    file >> data;  
    file.close();

    
    unordered_map<string, unordered_set<string>> adj_list;

    // Iterate through each movie
    for (const auto& entry : data) {
        cout << "Processing movie: " << entry["title"] << endl;

        if (entry.contains("cast") && entry["cast"].is_string()) {
            json cast_data = json::parse(entry["cast"].get<string>());

            vector<string> actors;
            for (const auto& actor : cast_data) {
                if (actor.contains("name")) {
                    actors.push_back(actor["name"]);
                }
            }

            cout << "Found " << actors.size() << " actors in movie: " << entry["title"] << endl;

            // Add edges 
            for (size_t i = 0; i < actors.size(); ++i) {
                for (size_t j = i + 1; j < actors.size(); ++j) {
                    adj_list[actors[i]].insert(actors[j]);
                    adj_list[actors[j]].insert(actors[i]);
                }
            }
        } else {
            cout << "No cast for the movie: " << entry["title"] << endl;
        }
    }

    // Save the adjacency list to a file
    ofstream output_file("adjacency_list.txt");
    if (!output_file.is_open()) {
        cerr << "Failed to open output file." << endl;
        return 1;
    }

    for (const auto& pair : adj_list) {
        output_file << pair.first << ": ";
        for (const auto& neighbor : pair.second) {
            output_file << neighbor << " ";
        }
        output_file << endl;
    }

    output_file.close();

    cout << "Adjacency list saved to adjacency_list.txt" << endl;

    // Calculate degree centrality 
    vector<pair<string, size_t>> actor_degrees;
    for (const auto& pair : adj_list) {
        actor_degrees.emplace_back(pair.first, pair.second.size());
    }

    // Sort actors by degree centrality in descending order
    sort(actor_degrees.begin(), actor_degrees.end(),
         [](const pair<string, size_t>& a, const pair<string, size_t>& b) {
             return b.second < a.second; 
         });

    // Output top 5 actors
    cout << "Top 5 actors with highest degree centrality:" << endl;
    for (size_t i = 0; i < 5 && i < actor_degrees.size(); ++i) {
        cout << actor_degrees[i].first << " - Degree: " << actor_degrees[i].second << endl;
    }

    // Save the top 5 actors to file
    ofstream top_actors_file("top_5_actors.txt");
    if (!top_actors_file.is_open()) {
        cerr << "Failed to open top actors file." << endl;
        return 1;
    }

    for (size_t i = 0; i < 5 && i < actor_degrees.size(); ++i) {
        top_actors_file << actor_degrees[i].first << " - Degree: " << actor_degrees[i].second << endl;
    }

    top_actors_file.close();
    
    // Determine is graph is connected
    unordered_set<string> visited;
    int connected_components = 0;

    for (const auto& pair : adj_list) {
        if (visited.count(pair.first) == 0) {
            // Found a new connected component, perform DFS
            dfs(pair.first, adj_list, visited);
            connected_components++;
        }
    }

    // Output result
    ofstream report_file("graph_report.txt");
    if (!report_file.is_open()) {
        cerr << "Failed to open report file." << endl;
        return 1;
    }

    if (connected_components == 1) {
        cout << "The graph is connected." << endl;
        report_file << "The graph is connected." << endl;
    } else {
        cout << "The graph is not connected. Number of connected components: " << connected_components << endl;
        report_file << "The graph is not connected. Number of connected components: " << connected_components << endl;
    }

    report_file.close();
    
    // Shortest Degree of Separation
    vector<pair<string, string>> example_pairs = {
        {"Sam Worthington", "Sean Patrick Murphy"}, 
        {"Jennifer Connelly", "Kierstin Koppel"},
        {"Samm Levine", "Nick Frost"}
    };

    ofstream report1_file("shortest_paths_report.txt");
    if (!report1_file.is_open()) {
        cerr << "Failed to open report file." << endl;
        return 1;
    }

    for (const auto& pair : example_pairs) {
        int distance = shortest_path(pair.first, pair.second, adj_list);
        if (distance == -1) {
            cout << "No path between " << pair.first << " and " << pair.second << endl;
            report1_file << "No path between " << pair.first << " and " << pair.second << endl;
        } else {
            cout << "Shortest path between " << pair.first << " and " << pair.second << " is " << distance << endl;
            report1_file << "Shortest path between " << pair.first << " and " << pair.second << " is " << distance << endl;
        }
    }

    report1_file.close();
    
    // Shortest Path with Path Tracked
    vector<pair<string, string>> example_pairs_2 = {
        {"Sam Worthington", "Sean Patrick Murphy"}, 
        {"Jennifer Connelly", "Kierstin Koppel"},
        {"Samm Levine", "Nick Frost"}
    };

    ofstream report2_file("shortest_paths_report_2.txt");
    if (!report2_file.is_open()) {
        cerr << "Failed to open report file." << endl;
        return 1;
    }
    //Output Path
    for (const auto& pair : example_pairs_2) {
        vector<string> path = shortest_path_2(pair.first, pair.second, adj_list);
        if (path.size() == 1 && path[0] == "No path") {
            cout << "No path between " << pair.first << " and " << pair.second << endl;
            report2_file << "No path between " << pair.first << " and " << pair.second << endl;
        } else {
            cout << "Shortest path between " << pair.first << " and " << pair.second << ": ";
            for (const auto& actor : path) {
                cout << actor << (actor == pair.second ? "\n" : " -> ");
                report2_file << actor << (actor == pair.second ? "\n" : " -> ");
            }
        }
    }

    report2_file.close();
    
    

    return 0;
}
