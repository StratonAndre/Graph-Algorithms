#include "Button.hpp"
#include <iostream>

Button::Button(const std::string& text, const sf::Vector2f& position,
               const sf::Vector2f& size, std::function<void()> callback)
    : isHovered(false), onClick(callback) {

    shape.setPosition(position);
    shape.setSize(size);
    shape.setFillColor(Theme::PANEL_BACKGROUND);
    shape.setOutlineColor(Theme::NODE_OUTLINE);
    shape.setOutlineThickness(1.f);

    static sf::Font* sharedFont = nullptr;
    if (!sharedFont) {
        sharedFont = new sf::Font();
        bool fontLoaded = false;

        std::vector<std::string> fontPaths = {
            "resources/Roboto-Medium.ttf",
            "../resources/Roboto-Medium.ttf",
            "Roboto-Medium.ttf"
        };

        for (const auto& path : fontPaths) {
            if (sharedFont->loadFromFile(path)) {
                std::cout << "Successfully loaded font from: " << path << std::endl;
                fontLoaded = true;
                break;
            }
        }

        if (!fontLoaded) {
            std::cerr << "Warning: Could not load font, using default system font" << std::endl;
            sharedFont->loadFromFile("C:\\Windows\\Fonts\\arial.ttf");
        }
    }

    label.setFont(*sharedFont);
    setText(text);
}

void Button::setText(const std::string& newText) {
    label.setString(newText);
    label.setCharacterSize(16);
    label.setFillColor(Theme::TEXT_PRIMARY);
    centerText();
}

std::string Button::getText() const {
    return label.getString();
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(label);
}

bool Button::contains(sf::Vector2f point) const {
    return shape.getGlobalBounds().contains(point);
}

void Button::setHovered(bool hovered) {
    if (isHovered != hovered) {
        isHovered = hovered;
        float targetAlpha = hovered ? 1.0f : 0.0f;

        hoverAnimation = std::make_unique<Animation>(
            Theme::ANIMATION_DURATION,
            [this, targetAlpha](float progress) {
                sf::Color fillColor = shape.getFillColor();
                fillColor.a = static_cast<sf::Uint8>(255 * (0.8f + 0.2f * progress));
                shape.setFillColor(fillColor);
            }
        );
    }
}

void Button::handleClick() {
    if (onClick) onClick();
}

void Button::update(float deltaTime) {
    if (hoverAnimation) {
        hoverAnimation->update(deltaTime);
        if (hoverAnimation->isFinished()) {
            hoverAnimation.reset();
        }
    }
}