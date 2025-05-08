#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <memory>
#include <iostream>
#include "Animation.hpp"
#include "Theme.hpp"

class Button {
public:
    Button(const std::string& text, const sf::Vector2f& position,
           const sf::Vector2f& size, std::function<void()> callback);

    void setText(const std::string& newText);
    std::string getText() const;
    void draw(sf::RenderWindow& window);
    bool contains(sf::Vector2f point) const;
    void setHovered(bool hovered);
    void handleClick();
    void update(float deltaTime);

private:
    sf::RectangleShape shape;
    sf::Text label;
    bool isHovered;
    std::function<void()> onClick;
    std::unique_ptr<Animation> hoverAnimation;

    void centerText() {
        sf::FloatRect textBounds = label.getLocalBounds();
        label.setOrigin(textBounds.width/2, textBounds.height/2);
        sf::Vector2f buttonCenter = shape.getPosition() + shape.getSize() / 2.f;
        label.setPosition(buttonCenter);
    }
};