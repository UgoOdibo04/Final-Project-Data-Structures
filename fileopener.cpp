#include "json.hpp"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

using json = nlohmann::json;
using namespace std;

int main() {
    // Load JSON data from file
    ifstream file("tmdb_5000_credits.json");
    if (!file.is_open()) {
        cerr << "Failed to open file. Please check the file path and name." << endl;
        return 1;
    }

    json data;
    file >> data;  // Parse the JSON content from the file
    file.close();

    // Adjacency list to store the actor graph
    unordered_map<string, unordered_set<string>> adj_list;

    // Iterate through each movie entry
    for (const auto& entry : data) {
        cout << "Processing movie: " << entry["title"] << endl;

        if (entry.contains("cast") && entry["cast"].is_string()) {
            // Parse the 'cast' field as JSON
            json cast_data = json::parse(entry["cast"].get<string>());

            vector<string> actors;
            for (const auto& actor : cast_data) {
                if (actor.contains("name")) {
                    actors.push_back(actor["name"]);
                }
            }

            cout << "Found " << actors.size() << " actors in movie: " << entry["title"] << endl;

            // Add edges between each pair of actors
            for (size_t i = 0; i < actors.size(); ++i) {
                for (size_t j = i + 1; j < actors.size(); ++j) {
                    adj_list[actors[i]].insert(actors[j]);
                    adj_list[actors[j]].insert(actors[i]);
                }
            }
        } else {
            cout << "No valid cast data for movie: " << entry["title"] << endl;
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

    return 0;
}
