#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include <filesystem>
#include <iostream>
#include <optional>
#include "Graph.hpp"

class GraphIO {
public:
    static void loadFromFile(Graph& graph, const std::string& filename) {

        std::filesystem::path currentPath = std::filesystem::current_path();
        std::cout << "Current working directory: " << currentPath << std::endl;


        std::filesystem::path resourcesPath = currentPath / "resources";
        std::cout << "Looking for resources directory at: " << resourcesPath << std::endl;

        if (!std::filesystem::exists(resourcesPath)) {
            std::cerr << "Resources directory not found at: " << resourcesPath << std::endl;


            resourcesPath = currentPath.parent_path() / "resources";
            std::cout << "Trying parent directory resources: " << resourcesPath << std::endl;

            if (!std::filesystem::exists(resourcesPath)) {
                std::cerr << "Resources directory not found in parent directory either" << std::endl;
                throw std::runtime_error("Resources directory not found");
            }
        }


        std::filesystem::path filePath = resourcesPath / filename;
        std::cout << "Looking for file at: " << filePath << std::endl;

        if (!std::filesystem::exists(filePath)) {
            std::cerr << "Input file not found: " << filePath << std::endl;
            throw std::runtime_error("Input file not found: " + filePath.string());
        }

        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filePath.string());
        }


        graph.clear();


        int numNodes;
        file >> numNodes;
        std::cout << "Loading graph with " << numNodes << " nodes" << std::endl;


        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> xDist(200.0f, 800.0f);
        std::uniform_real_distribution<float> yDist(200.0f, 600.0f);

        for (int i = 0; i < numNodes; i++) {
            float x = xDist(gen);
            float y = yDist(gen);
            graph.addNode(x, y, i);
        }


        std::string line;
        std::getline(file, line);


        while (std::getline(file, line)) {
            std::istringstream iss(line);
            int sourceId;
            iss >> sourceId;

            std::string target;
            while (iss >> target) {

                int targetId;
                std::optional<float> weight;

                size_t colonPos = target.find(':');
                if (colonPos != std::string::npos) {

                    targetId = std::stoi(target.substr(0, colonPos));
                    weight = std::stof(target.substr(colonPos + 1));
                } else {

                    targetId = std::stoi(target);
                }

                graph.addEdgeById(sourceId, targetId, weight);
                std::cout << "Added edge: " << sourceId << " -> " << targetId;
                if (weight) {
                    std::cout << " with weight " << weight.value();
                }
                std::cout << std::endl;
            }
        }

        std::cout << "Successfully loaded graph from: " << filePath << std::endl;
    }

    static void saveToFile(const Graph& graph, const std::string& filename) {
        std::filesystem::path filePath = std::filesystem::current_path() / "resources" / filename;
        std::cout << "Saving to file at: " << filePath << std::endl;

        std::ofstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file for writing: " + filePath.string());
        }

        file << graph.getNodes().size() << "\n";

        for (const auto& node : graph.getNodes()) {
            file << node->getId();
            for (const auto& edge : graph.getEdges()) {
                if (edge->getStartNode() == node.get()) {
                    file << " " << edge->getEndNode()->getId();
                    if (edge->getWeight()) {
                        file << ":" << edge->getWeight().value();
                    }
                }
            }
            file << "\n";
        }
    }
};