#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include <vector>
#include <algorithm>
#include <map>
#include "Button.hpp"
#include "Graph.hpp"
#include "Theme.hpp"

class UIPanel {
public:
    UIPanel(float width, float height);
    void setGraph(Graph* g);
    void setSelectedNode(Node* node);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    void handleEvent(sf::Event& event);
    bool contains(sf::Vector2f point) const;
    const std::vector<std::unique_ptr<Button>>& getButtons() const;

private:
    sf::RectangleShape panel;
    sf::RectangleShape scrollView;
    sf::Font font;
    sf::Text title;
    std::vector<std::unique_ptr<Button>> buttons;
    Node* selectedNode;
    float scrollOffset;
    bool isOrdered;
    bool fontLoaded;
    Graph* graph;
    float panelWidth;
    float panelTargetX;
    float panelCurrentX;
    std::unique_ptr<Animation> slideAnimation;

    void createButton(const std::string& text, const sf::Vector2f& position,
                     const sf::Vector2f& size, std::function<void()> callback);
    float drawUndirectedNeighbors(sf::RenderWindow& window, float y, float viewHeight, float itemHeight);
    float drawDirectedNeighbors(sf::RenderWindow& window, float y, float viewHeight, float itemHeight);
    void toggleOrder();
    void updateButtonText();
    float getMaxScroll() const;
    void updatePanelPosition();
};