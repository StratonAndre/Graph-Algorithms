#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include "Theme.hpp"
#include "Animation.hpp"

class Node {
public:
    Node(float x, float y, int id);
    void draw(sf::RenderTarget& target);
    bool contains(sf::Vector2f point) const;
    void setPosition(sf::Vector2f newPos);
    void setSelected(bool selected);
    void setColor(const sf::Color& color);
    void setStatusLabel(const std::string& status);
    void setHighlighted(bool highlighted);
    void update(float deltaTime);
    void setScale(float scale);
    float getScale() const;
    sf::Color getColor() const;
    void setStateColor(const sf::Color& color);

    sf::Vector2f getPosition() const { return shape.getPosition(); }
    int getId() const { return id; }
    float getRadius() const { return shape.getRadius(); }
    bool isSelected() const { return selected; }
    bool isHighlighted() const { return highlighted; }

private:
    sf::CircleShape shape;
    sf::Text label;
    sf::Text statusLabel;
    static sf::Font font;
    static bool fontLoaded;
    int id;
    bool selected;
    bool highlighted;
    std::unique_ptr<Animation> currentAnimation;
    std::unique_ptr<Animation> colorAnimation;
    sf::Color currentColor;
    float currentScale;

    void createSelectionAnimation();
    void createColorAnimation(const sf::Color& newColor);
    void updateStatusLabelPosition();
    void updateLabelPositions();
};


inline void Node::setScale(float scale) {
    currentScale = scale;
    shape.setScale(scale, scale);
    updateLabelPositions();
}

inline float Node::getScale() const {
    return currentScale;
}

inline sf::Color Node::getColor() const {
    return currentColor;
}

inline void Node::setStateColor(const sf::Color& color) {
    currentColor = color;
    if (!colorAnimation) {
        shape.setFillColor(color);
    }
}

inline void Node::updateLabelPositions() {

    sf::Vector2f nodePos = shape.getPosition();
    sf::FloatRect labelBounds = label.getLocalBounds();
    label.setOrigin(labelBounds.width/2, labelBounds.height/2);
    label.setPosition(nodePos);
    label.setScale(currentScale, currentScale);


    if (!statusLabel.getString().isEmpty()) {
        sf::FloatRect statusBounds = statusLabel.getLocalBounds();
        statusLabel.setOrigin(statusBounds.width/2, statusBounds.height + shape.getRadius() * currentScale + 15);
        statusLabel.setPosition(nodePos);
        statusLabel.setScale(currentScale, currentScale);
    }
}