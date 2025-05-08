#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <filesystem>
#include "Graph.hpp"
#include "Theme.hpp"
#include "GraphIO.hpp"
#include "UIPanel.hpp"
#include "MSTPanel.hpp"
#include "AlgorithmPanel.hpp"
#include "BackgroundGrid.hpp"
#include "ViewportManager.hpp"

class Application {
public:
    Application();
    void run();

private:
    sf::RenderWindow window;
    Graph graph;
    std::unique_ptr<UIPanel> uiPanel;
    std::unique_ptr<MSTPanel> mstPanel;
    std::unique_ptr<AlgorithmPanel> algorithmPanel;
    std::unique_ptr<BackgroundGrid> grid;
    std::unique_ptr<ViewportManager> viewportManager;
    Node* selectedNode;
    Node* draggedNode;
    bool isDragging;
    bool showAlgorithmPanel;
    sf::Clock clock;
    float gridOffset;

    void handleEvents();
    void update(float deltaTime);
    void render();
    void handleKeyPress(const sf::Event::KeyEvent& key);
    void handleMousePress(const sf::Event::MouseButtonEvent& mouseButton);
    void handleMouseRelease(const sf::Event::MouseButtonEvent& mouseButton);
    void handleMouseMove(const sf::Event& event);
    void loadInitialGraph();
    void saveGraph(const std::string& filename);
    void loadGraph(const std::string& filename);
    void toggleAlgorithmPanel();
    void updateViewportBounds(const sf::Vector2f& newPoint);
    void resetViewportToFitGraph();
};