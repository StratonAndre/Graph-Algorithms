#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class ViewportManager {
public:
    ViewportManager(sf::RenderWindow& window) :
        window(window),
        view(window.getDefaultView()),
        zoomLevel(1.0f),
        minZoom(0.1f),
        maxZoom(10.0f),
        isDragging(false),
        dragSensitivity(1.0f)
    {

        sf::Vector2u screenSize = window.getSize();
        bounds = sf::FloatRect(0, 0, screenSize.x, screenSize.y);


        view = window.getDefaultView();
        window.setView(view);

        std::cout << "ViewportManager initialized with screen size: "
                  << screenSize.x << "x" << screenSize.y << std::endl;
    }

    void handleEvent(const sf::Event& event) {
        try {
            switch (event.type) {
                case sf::Event::MouseWheelScrolled:
                    if (event.mouseWheelScroll.delta != 0) {
                        handleZoom(event.mouseWheelScroll.x, event.mouseWheelScroll.y,
                                 event.mouseWheelScroll.delta);
                    }
                    break;

                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Middle) {
                        isDragging = true;
                        lastMousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                    }
                    break;

                case sf::Event::MouseButtonReleased:
                    if (event.mouseButton.button == sf::Mouse::Middle) {
                        isDragging = false;
                    }
                    break;

                case sf::Event::MouseMoved:
                    if (isDragging) {
                        handleDrag(event.mouseMove.x, event.mouseMove.y);
                    }
                    break;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in ViewportManager event handling: " << e.what() << std::endl;
        }
    }

    void setBounds(const sf::FloatRect& newBounds) {
        bounds = newBounds;
        constrainView();
        std::cout << "ViewportManager bounds set to: "
                  << bounds.left << "," << bounds.top << ","
                  << bounds.width << "," << bounds.height << std::endl;
    }

    const sf::FloatRect& getBounds() const {
        return bounds;
    }

    sf::Vector2f screenToWorld(const sf::Vector2i& screenPos) const {
        try {
            return window.mapPixelToCoords(screenPos, view);
        } catch (const std::exception& e) {
            std::cerr << "Error in screenToWorld conversion: " << e.what() << std::endl;
            return sf::Vector2f(0, 0);
        }
    }

    sf::Vector2i worldToScreen(const sf::Vector2f& worldPos) const {
        try {
            return window.mapCoordsToPixel(worldPos, view);
        } catch (const std::exception& e) {
            std::cerr << "Error in worldToScreen conversion: " << e.what() << std::endl;
            return sf::Vector2i(0, 0);
        }
    }

    float getZoomLevel() const { return zoomLevel; }

    const sf::View& getView() const { return view; }

    void applyView() {
        try {
            window.setView(view);
        } catch (const std::exception& e) {
            std::cerr << "Error applying view: " << e.what() << std::endl;
        }
    }

    void reset() {
        try {
            zoomLevel = 1.0f;
            view = window.getDefaultView();
            window.setView(view);
            std::cout << "ViewportManager reset to default view" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error resetting view: " << e.what() << std::endl;
        }
    }

private:
    sf::RenderWindow& window;
    sf::View view;
    float zoomLevel;
    float minZoom;
    float maxZoom;
    bool isDragging;
    sf::Vector2f lastMousePos;
    sf::FloatRect bounds;
    float dragSensitivity;

    void handleZoom(float mouseX, float mouseY, float delta) {
        try {

            sf::Vector2f mousePosBeforeZoom = window.mapPixelToCoords(
                {static_cast<int>(mouseX), static_cast<int>(mouseY)}, view);

            float zoomFactor = (delta > 0) ? 0.9f : 1.1f;
            float newZoom = zoomLevel * zoomFactor;


            if (newZoom >= minZoom && newZoom <= maxZoom) {
                zoomLevel = newZoom;

                sf::Vector2f viewSize = window.getDefaultView().getSize();
                view.setSize(viewSize * zoomLevel);

                sf::Vector2f mousePosAfterZoom = window.mapPixelToCoords(
                    {static_cast<int>(mouseX), static_cast<int>(mouseY)}, view);

                view.move(mousePosBeforeZoom - mousePosAfterZoom);

                constrainView();
                window.setView(view);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in zoom handling: " << e.what() << std::endl;
        }
    }

    void handleDrag(float mouseX, float mouseY) {
        try {
            sf::Vector2f currentMousePos(mouseX, mouseY);
            sf::Vector2f delta = (lastMousePos - currentMousePos) * zoomLevel * dragSensitivity;

            view.move(delta);
            constrainView();
            window.setView(view);

            lastMousePos = currentMousePos;
        } catch (const std::exception& e) {
            std::cerr << "Error in drag handling: " << e.what() << std::endl;
        }
    }

    void constrainView() {
        try {
            sf::Vector2f viewSize = view.getSize();
            sf::Vector2f center = view.getCenter();

            float minX = bounds.left + viewSize.x / 2;
            float maxX = bounds.left + bounds.width - viewSize.x / 2;
            float minY = bounds.top + viewSize.y / 2;
            float maxY = bounds.top + bounds.height - viewSize.y / 2;

            if (maxX < minX) {
                float centerX = bounds.left + bounds.width / 2;
                minX = centerX;
                maxX = centerX;
            }
            if (maxY < minY) {
                float centerY = bounds.top + bounds.height / 2;
                minY = centerY;
                maxY = centerY;
            }

            center.x = std::max(minX, std::min(center.x, maxX));
            center.y = std::max(minY, std::min(center.y, maxY));

            view.setCenter(center);
        } catch (const std::exception& e) {
            std::cerr << "Error in view constraint: " << e.what() << std::endl;
        }
    }
};