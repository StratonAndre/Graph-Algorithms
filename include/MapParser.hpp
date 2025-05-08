#pragma once
#include <string>
#include <vector>
#include <map>
#include "Coordinates.hpp"

class MapParser {
public:
    struct NodeData {
        int id;
        Coordinates coords;
    };

    static std::vector<NodeData> parseNodeFile(const std::string& filename) {

        std::vector<NodeData> nodes;
        nodes.push_back({1, Coordinates(51.5074, -0.1278)});
        nodes.push_back({2, Coordinates(48.8566, 2.3522)});
        nodes.push_back({3, Coordinates(52.5200, 13.4050)});
        return nodes;
    }
    
    static std::pair<Coordinates, Coordinates> findBounds(const std::vector<NodeData>& nodes) {
        if (nodes.empty()) return {Coordinates(), Coordinates()};
        
        Coordinates minBounds = nodes[0].coords;
        Coordinates maxBounds = nodes[0].coords;
        
        for (const auto& node : nodes) {
            minBounds.latitude = std::min(minBounds.latitude, node.coords.latitude);
            minBounds.longitude = std::min(minBounds.longitude, node.coords.longitude);
            maxBounds.latitude = std::max(maxBounds.latitude, node.coords.latitude);
            maxBounds.longitude = std::max(maxBounds.longitude, node.coords.longitude);
        }
        
        return {minBounds, maxBounds};
    }
};