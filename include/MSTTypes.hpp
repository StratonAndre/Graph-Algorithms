#pragma once
#include <SFML/Graphics.hpp>
#include <string>

namespace MSTColors {
    const sf::Color UNVISITED(150, 150, 255);    // Light blue
    const sf::Color PROCESSING(255, 200, 100);    // Orange
    const sf::Color CURRENT(255, 165, 0);         // Bright orange
    const sf::Color IN_MST(100, 255, 100);       // Green
    const sf::Color REJECTED(255, 100, 100);      // Red
    const sf::Color CONSIDERING(255, 255, 100);   // Yellow
}

struct WeightedEdge {
    int src;
    int dest;
    float weight;
    std::string description;

    bool operator>(const WeightedEdge& other) const {
        return weight > other.weight;
    }
};