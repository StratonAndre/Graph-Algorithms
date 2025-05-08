#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Theme.hpp"

class BackgroundGrid {
public:
    BackgroundGrid(sf::Vector2u windowSize) :
        windowSize(windowSize),
        baseGridSize(50.f),
        gridSize(50.f),
        gridOpacity(0.1f),
        offset(0.f),
        isAnimated(true)
    {
        createGridLines();
    }

    void draw(sf::RenderTarget& target) {
        const sf::View& view = target.getView();
        sf::Vector2f viewCenter = view.getCenter();
        sf::Vector2f viewSize = view.getSize();

        sf::FloatRect visibleArea(
            viewCenter.x - viewSize.x/2,
            viewCenter.y - viewSize.y/2,
            viewSize.x,
            viewSize.y
        );

        float zoomLevel = viewSize.x / windowSize.x;
        float adjustedGridSize = baseGridSize * std::max(1.0f, zoomLevel);

        while (adjustedGridSize < 30.f) adjustedGridSize *= 2;
        while (adjustedGridSize > 100.f) adjustedGridSize /= 2;

        gridSize = adjustedGridSize;

        float effectiveOffset = std::fmod(offset, gridSize);
        if (effectiveOffset < 0) effectiveOffset += gridSize;

        float startX = std::floor((visibleArea.left - effectiveOffset) / gridSize) * gridSize + effectiveOffset;
        float startY = std::floor((visibleArea.top - effectiveOffset) / gridSize) * gridSize + effectiveOffset;
        float endX = visibleArea.left + visibleArea.width + gridSize;
        float endY = visibleArea.top + visibleArea.height + gridSize;


        for (float x = startX; x <= endX; x += gridSize) {
            sf::RectangleShape line(sf::Vector2f(1.f, visibleArea.height));
            line.setPosition(x, visibleArea.top);
            float alpha = 255.f * gridOpacity / zoomLevel;
            line.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
            target.draw(line);
        }


        for (float y = startY; y <= endY; y += gridSize) {
            sf::RectangleShape line(sf::Vector2f(visibleArea.width, 1.f));
            line.setPosition(visibleArea.left, y);
            float alpha = 255.f * gridOpacity / zoomLevel;
            line.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
            target.draw(line);
        }


        float majorGridSize = gridSize * 5;
        float effectiveMajorOffset = std::fmod(offset, majorGridSize);
        if (effectiveMajorOffset < 0) effectiveMajorOffset += majorGridSize;

        float majorStartX = std::floor((visibleArea.left - effectiveMajorOffset) / majorGridSize) * majorGridSize + effectiveMajorOffset;
        float majorStartY = std::floor((visibleArea.top - effectiveMajorOffset) / majorGridSize) * majorGridSize + effectiveMajorOffset;

        float majorOpacity = std::min(1.0f, gridOpacity * 2.f / zoomLevel);


        for (float x = majorStartX; x <= endX; x += majorGridSize) {
            sf::RectangleShape line(sf::Vector2f(2.f, visibleArea.height));
            line.setPosition(x, visibleArea.top);
            float alpha = 255.f * majorOpacity;
            line.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
            target.draw(line);
        }


        for (float y = majorStartY; y <= endY; y += majorGridSize) {
            sf::RectangleShape line(sf::Vector2f(visibleArea.width, 2.f));
            line.setPosition(visibleArea.left, y);
            float alpha = 255.f * majorOpacity;
            line.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
            target.draw(line);
        }
    }

    void update(float deltaTime) {
        if (isAnimated) {
            offset += deltaTime * 20.f;
            if (offset > 50.f) {
                offset = 0.f;
            }
        }
    }

    void toggleAnimation() {
        isAnimated = !isAnimated;
        if (!isAnimated) {
            offset = 0.f;
        }
    }

    void setOpacity(float opacity) {
        gridOpacity = opacity;
    }

    bool isAnimationEnabled() const {
        return isAnimated;
    }

private:
    sf::Vector2u windowSize;
    float baseGridSize;
    float gridSize;
    float gridOpacity;
    float offset;
    bool isAnimated;
    sf::Color gridColor{255, 255, 255};

    void createGridLines() {}
};