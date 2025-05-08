#include "Node.hpp"
#include <iostream>

sf::Font Node::font;
bool Node::fontLoaded = false;

Node::Node(float x, float y, int id)
    : id(id), selected(false), highlighted(false), currentScale(1.0f),
      currentColor(Theme::NODE_FILL) {

    shape.setRadius(Theme::NODE_RADIUS);
    shape.setOrigin(Theme::NODE_RADIUS, Theme::NODE_RADIUS);
    shape.setPosition(x, y);
    shape.setFillColor(Theme::NODE_FILL);
    shape.setOutlineThickness(Theme::NODE_OUTLINE_THICKNESS);
    shape.setOutlineColor(Theme::NODE_OUTLINE);

    if (!fontLoaded) {
        if (font.loadFromFile("resources/Roboto-Medium.ttf")) {
            fontLoaded = true;
        } else {
            std::cerr << "Could not load font" << std::endl;
        }
    }


    label.setFont(font);
    label.setString(std::to_string(id));
    label.setCharacterSize(24);
    label.setFillColor(sf::Color::Black);
    label.setStyle(sf::Text::Bold);


    statusLabel.setFont(font);
    statusLabel.setCharacterSize(14);
    statusLabel.setFillColor(sf::Color::White);

    updateLabelPositions();
}

void Node::draw(sf::RenderTarget& target) {

    if (highlighted) {
        sf::CircleShape glow = shape;
        glow.setRadius(Theme::NODE_RADIUS + 5.f);
        glow.setOrigin(Theme::NODE_RADIUS + 5.f, Theme::NODE_RADIUS + 5.f);
        glow.setPosition(shape.getPosition());
        glow.setFillColor(sf::Color::Transparent);
        glow.setOutlineColor(sf::Color(255, 255, 0, 100));
        glow.setOutlineThickness(8.f);
        target.draw(glow);
    }


    if (selected) {
        sf::CircleShape selectionGlow = shape;
        selectionGlow.setRadius(Theme::NODE_RADIUS + 5.f);
        selectionGlow.setOrigin(Theme::NODE_RADIUS + 5.f, Theme::NODE_RADIUS + 5.f);
        selectionGlow.setPosition(shape.getPosition());
        selectionGlow.setFillColor(sf::Color::Transparent);
        selectionGlow.setOutlineColor(sf::Color(Theme::NODE_SELECTED.r,
                                     Theme::NODE_SELECTED.g,
                                     Theme::NODE_SELECTED.b,
                                     100));
        selectionGlow.setOutlineThickness(8.f);
        target.draw(selectionGlow);
    }

    target.draw(shape);
    target.draw(label);

    if (!statusLabel.getString().isEmpty()) {

        sf::FloatRect bounds = statusLabel.getLocalBounds();
        sf::RectangleShape background;
        background.setSize(sf::Vector2f(bounds.width + 10, bounds.height + 10));
        background.setOrigin(bounds.width/2 + 5, bounds.height + 5);
        background.setPosition(statusLabel.getPosition());
        background.setFillColor(sf::Color(0, 0, 0, 150));
        target.draw(background);
        target.draw(statusLabel);
    }
}

bool Node::contains(sf::Vector2f point) const {
    sf::Vector2f center = shape.getPosition();
    float dx = point.x - center.x;
    float dy = point.y - center.y;
    float distanceSquared = dx * dx + dy * dy;
    float scaledRadius = shape.getRadius() * currentScale;
    return distanceSquared <= scaledRadius * scaledRadius;
}

void Node::setPosition(sf::Vector2f newPos) {
    shape.setPosition(newPos);
    updateLabelPositions();
}

void Node::setSelected(bool isSelected) {
    if (selected != isSelected) {
        selected = isSelected;
        createSelectionAnimation();
    }
}

void Node::setHighlighted(bool isHighlighted) {
    highlighted = isHighlighted;
}

void Node::setColor(const sf::Color& color) {
    if (currentColor != color) {
        createColorAnimation(color);
        currentColor = color;
    }
}

void Node::setStatusLabel(const std::string& status) {
    statusLabel.setString(status);
    updateLabelPositions();
}

void Node::update(float deltaTime) {
    if (currentAnimation) {
        currentAnimation->update(deltaTime);
        if (currentAnimation->isFinished()) {
            currentAnimation.reset();
        }
    }

    if (colorAnimation) {
        colorAnimation->update(deltaTime);
        if (colorAnimation->isFinished()) {
            colorAnimation.reset();
        }
    }
}

void Node::createSelectionAnimation() {
    float startScale = currentScale;
    float targetScale = selected ? 1.1f : 1.0f;

    currentAnimation = std::make_unique<Animation>(
        Theme::ANIMATION_DURATION,
        [this, startScale, targetScale](float progress) {
            float scale = startScale + (targetScale - startScale) * progress;
            setScale(scale);

            if (selected) {
                shape.setOutlineColor(Theme::NODE_SELECTED);
            } else {
                sf::Color color = Theme::NODE_OUTLINE;
                color.a = static_cast<sf::Uint8>(255 * (1.0f - progress));
                shape.setOutlineColor(color);
            }
        }
    );
}

void Node::createColorAnimation(const sf::Color& newColor) {
    sf::Color startColor = shape.getFillColor();

    colorAnimation = std::make_unique<Animation>(
        Theme::ANIMATION_DURATION,
        [this, startColor, newColor](float progress) {
            sf::Color currentColor;
            currentColor.r = static_cast<sf::Uint8>(startColor.r + (newColor.r - startColor.r) * progress);
            currentColor.g = static_cast<sf::Uint8>(startColor.g + (newColor.g - startColor.g) * progress);
            currentColor.b = static_cast<sf::Uint8>(startColor.b + (newColor.b - startColor.b) * progress);
            currentColor.a = static_cast<sf::Uint8>(startColor.a + (newColor.a - startColor.a) * progress);
            shape.setFillColor(currentColor);
        }
    );
}