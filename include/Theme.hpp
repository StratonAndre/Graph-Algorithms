#pragma once
#include <SFML/Graphics.hpp>

namespace Theme {
    // Base colors
    inline const sf::Color BACKGROUND(34, 40, 49);       // Dark blue-grey background
    inline const sf::Color NODE_FILL(238, 238, 238);     // Light grey nodes
    inline const sf::Color NODE_OUTLINE(0, 173, 181);    // Teal outline
    inline const sf::Color NODE_SELECTED(252, 92, 101);  // Coral red for selected nodes

    // Updated edge colors for better visibility
    inline const sf::Color EDGE_COLOR(150, 150, 150, 255);         // Brighter base edge color
    inline const sf::Color EDGE_HIGHLIGHT(50, 255, 50, 255);       // Bright green for MST edges
    inline const sf::Color EDGE_CONSIDERING(255, 215, 0, 255);     // Gold for edges being considered
    inline const sf::Color EDGE_DISABLED(100, 100, 100, 128);      // Dimmed edges

    inline const sf::Color TEXT_PRIMARY(238, 238, 238);  // Light grey text
    inline const sf::Color TEXT_SECONDARY(0, 173, 181);  // Teal text
    inline const sf::Color PANEL_BACKGROUND(47, 54, 64); // Darker panel background

    // Algorithm visualization colors
    inline const sf::Color NODE_NEW(150, 150, 255);        // Light blue for new nodes
    inline const sf::Color NODE_PROCESSING(255, 200, 100); // Orange for nodes being processed
    inline const sf::Color NODE_COMPLETED(100, 255, 100);  // Green for completed nodes
    inline const sf::Color NODE_HIGHLIGHT(255, 255, 0);    // Yellow for highlighted nodes

    // Constants
    inline const float NODE_RADIUS = 35.f;
    inline const float NODE_OUTLINE_THICKNESS = 3.f;
    inline const float EDGE_THICKNESS = 3.f;              // Increased edge thickness
    inline const float EDGE_GLOW_THICKNESS = 6.f;         // Added glow thickness
    inline const float ANIMATION_DURATION = 0.3f;
}