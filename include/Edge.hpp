#pragma once
#include <SFML/Graphics.hpp>
#include "Node.hpp"
#include "Theme.hpp"
#include <math.h>
#include <optional>
#include <string>
#include <iomanip>
#include <sstream>

class Edge {
public:
    Edge(Node* start, Node* end, bool isDirected = false, std::optional<float> weight = std::nullopt)
        : startNode(start), endNode(end), directed(isDirected), showArrow(isDirected),
          weight(weight), isHighlighted(false) {

        line.setPrimitiveType(sf::Quads);
        line.resize(4);

        glow.setPrimitiveType(sf::Quads);
        glow.resize(4);

        arrowHead.setPrimitiveType(sf::Triangles);
        arrowHead.resize(3);

        if (weight.has_value() && !fontLoaded) {
            if (font.loadFromFile("resources/Roboto-Medium.ttf")) {
                fontLoaded = true;
            }
        }

        if (weight.has_value() && fontLoaded) {
            weightText.setFont(font);
            weightText.setCharacterSize(14);
            weightText.setFillColor(sf::Color::White);
            updateWeightText();
        }

        updateVertices();
    }

    void draw(sf::RenderTarget& target, const sf::RenderStates& states) {
        // Draw glow effect first
        target.draw(glow, states);
        // Then draw the main line
        target.draw(line, states);
        if (showArrow) {
            target.draw(arrowHead, states);
        }
        if (weight.has_value() && fontLoaded) {
            target.draw(weightText, states);
        }
    }

    void setDirected(bool isDirected) {
        directed = isDirected;
        showArrow = isDirected;
        updateVertices();
    }

    void setShowArrow(bool show) {
        showArrow = show;
        updateVertices();
    }

    void setHighlighted(bool highlight) {
        isHighlighted = highlight;
        updateVertices();
    }

    void setWeight(float newWeight) {
        weight = newWeight;
        updateWeightText();
    }

    void update() {
        if (startNode && endNode) {
            updateVertices();
            if (weight.has_value()) {
                updateWeightText();
            }
        }
    }

    std::optional<float> getWeight() const { return weight; }
    bool isConnectedTo(Node* node) const { return startNode == node || endNode == node; }
    Node* getStartNode() const { return startNode; }
    Node* getEndNode() const { return endNode; }
    bool isDirected() const { return directed; }
    bool isShowingArrow() const { return showArrow; }
    bool isHighlightedEdge() const { return isHighlighted; }

private:
    Node* startNode;
    Node* endNode;
    bool directed;
    bool showArrow;
    bool isHighlighted;
    std::optional<float> weight;
    sf::VertexArray line;
    sf::VertexArray glow;
    sf::VertexArray arrowHead;
    sf::Text weightText;
    inline static sf::Font font;
    inline static bool fontLoaded = false;
    const float LINE_THICKNESS = 3.0f;
    const float GLOW_THICKNESS = 6.0f;
    const float ARROW_SIZE = 15.0f;

    void updateWeightText() {
        if (!weight.has_value() || !fontLoaded) return;

        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << weight.value();
        weightText.setString(ss.str());

        sf::Vector2f start = startNode->getPosition();
        sf::Vector2f end = endNode->getPosition();
        sf::Vector2f mid = (start + end) / 2.f;

        sf::Vector2f dir = end - start;
        sf::Vector2f normal(-dir.y, dir.x);
        float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
        if (length > 0) {
            normal = normal / length * 20.f;
        }

        sf::FloatRect bounds = weightText.getLocalBounds();
        weightText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        weightText.setPosition(mid + normal);

        weightText.setOutlineThickness(1.0f);
        weightText.setOutlineColor(sf::Color::Black);
    }

    void updateVertices() {
        sf::Vector2f start = startNode->getPosition();
        sf::Vector2f end = endNode->getPosition();

        float nodeRadius = startNode->getRadius();
        sf::Vector2f direction = end - start;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f normalizedDir = direction / length;

        sf::Vector2f adjustedStart = start + normalizedDir * nodeRadius;
        sf::Vector2f adjustedEnd = end - normalizedDir * (nodeRadius + (showArrow ? ARROW_SIZE : 0));

        sf::Vector2f normal(-direction.y / length, direction.x / length);

        sf::Color edgeColor = isHighlighted ? Theme::EDGE_HIGHLIGHT : Theme::EDGE_COLOR;
        sf::Color glowColor = isHighlighted ?
            sf::Color(Theme::EDGE_HIGHLIGHT.r, Theme::EDGE_HIGHLIGHT.g, Theme::EDGE_HIGHLIGHT.b, 100) :
            sf::Color(Theme::EDGE_COLOR.r, Theme::EDGE_COLOR.g, Theme::EDGE_COLOR.b, 40);

        updateQuad(line, adjustedStart, adjustedEnd, normal, LINE_THICKNESS, edgeColor);
        updateQuad(glow, adjustedStart, adjustedEnd, normal, GLOW_THICKNESS, glowColor);

        if (showArrow) {
            updateArrowHead(adjustedEnd, -normalizedDir, edgeColor);
        }
    }

    void updateQuad(sf::VertexArray& vertices, const sf::Vector2f& start,
                   const sf::Vector2f& end, const sf::Vector2f& normal,
                   float thickness, const sf::Color& color) {
        sf::Vector2f offset = normal * (thickness / 2.f);

        vertices[0].position = start - offset;
        vertices[1].position = end - offset;
        vertices[2].position = end + offset;
        vertices[3].position = start + offset;

        vertices[0].color = color;
        vertices[1].color = color;
        vertices[2].color = color;
        vertices[3].color = color;
    }

    void updateArrowHead(const sf::Vector2f& tip, const sf::Vector2f& dir, const sf::Color& color) {
        sf::Vector2f normal(-dir.y, dir.x);
        float arrowSize = Theme::NODE_RADIUS * 0.8f;

        arrowHead[0].position = tip;
        arrowHead[1].position = tip + (dir + normal * 0.5f) * arrowSize;
        arrowHead[2].position = tip + (dir - normal * 0.5f) * arrowSize;

        arrowHead[0].color = color;
        arrowHead[1].color = color;
        arrowHead[2].color = color;
    }
};