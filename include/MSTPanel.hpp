#pragma once
#include <SFML/Graphics.hpp>
#include "Button.hpp"
#include "Graph.hpp"
#include "Theme.hpp"
#include "algorithms/MSTAlgorithm.hpp"
#include <memory>
#include <string>
#include <functional>
#include "algorithms/KruskalMST.hpp"
#include "algorithms/BoruvkaMST.hpp"
class MSTPanel {
public:
    MSTPanel(float x, float y, float width, float height) {
        panel.setPosition(x, y);
        panel.setSize(sf::Vector2f(width, height));
        panel.setFillColor(Theme::PANEL_BACKGROUND);
        panelBounds = panel.getGlobalBounds();

        float buttonY = y + 10;
        float buttonSpacing = 50;
        float buttonWidth = width - 20;
        float buttonHeight = 40;

        createButton("Step", sf::Vector2f(x + 10, buttonY),
                    sf::Vector2f(buttonWidth, buttonHeight),
                    [this]() { if (currentAlgorithm) step(); });

        buttonY += buttonSpacing;
        createButton("Reset", sf::Vector2f(x + 10, buttonY),
                    sf::Vector2f(buttonWidth, buttonHeight),
                    [this]() { if (currentAlgorithm) reset(); });

        buttonY += buttonSpacing;
        createButton("Kruskal", sf::Vector2f(x + 10, buttonY),
                    sf::Vector2f(buttonWidth, buttonHeight),
                    [this]() { selectAlgorithm("Kruskal"); });

        buttonY += buttonSpacing;
        createButton("Boruvka", sf::Vector2f(x + 10, buttonY),
                    sf::Vector2f(buttonWidth, buttonHeight),
                    [this]() { selectAlgorithm("Boruvka"); });

        if (font.loadFromFile("resources/Roboto-Medium.ttf")) {
            status.setFont(font);
            status.setCharacterSize(14);
            status.setFillColor(Theme::TEXT_PRIMARY);
            status.setPosition(x + 10, buttonY + buttonSpacing);
        }
    }

    void setGraph(Graph* g) {
        graph = g;
        if (currentAlgorithm) {
            reset();
        }
    }

    bool contains(sf::Vector2f point) const {
        return panelBounds.contains(point);
    }

    void handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            for (auto& button : buttons) {
                if (button->contains(mousePos)) {
                    button->handleClick();
                    break;
                }
            }
        } else if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
            for (auto& button : buttons) {
                button->setHovered(button->contains(mousePos));
            }
        }
    }

    void update(float deltaTime) {
        for (auto& button : buttons) {
            button->update(deltaTime);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(panel);
        for (const auto& button : buttons) {
            button->draw(window);
        }
        if (!status.getString().isEmpty()) {
            window.draw(status);
        }
    }

private:
    sf::RectangleShape panel;
    sf::FloatRect panelBounds;
    std::vector<std::unique_ptr<Button>> buttons;
    Graph* graph = nullptr;
    std::unique_ptr<MSTAlgorithm> currentAlgorithm;
    sf::Text status;
    sf::Font font;

    void createButton(const std::string& text, const sf::Vector2f& position,
                     const sf::Vector2f& size, std::function<void()> callback) {
        buttons.push_back(std::make_unique<Button>(text, position, size, callback));
    }

    void selectAlgorithm(const std::string& name) {
        if (!graph) return;

        if (name == "Kruskal") {
            currentAlgorithm = std::make_unique<KruskalMST>();
        } else if (name == "Boruvka") {
            currentAlgorithm = std::make_unique<BoruvkaMST>();
        }

        if (currentAlgorithm) {
            prepareAlgorithm();
        }
    }

    void prepareAlgorithm() {
        if (!currentAlgorithm || !graph) return;

        currentAlgorithm->reset();

        for (const auto& edge : graph->getEdges()) {
            if (edge->getWeight()) {
                currentAlgorithm->addEdge(
                    edge->getStartNode()->getId(),
                    edge->getEndNode()->getId(),
                    edge->getWeight().value()
                );
            }
        }

        currentAlgorithm->execute(graph->getNodes());
        updateGraphHighlights();
        status.setString("Algorithm initialized");
    }

    void step() {
        if (!currentAlgorithm || !graph) return;

        if (!currentAlgorithm->isFinished()) {
            if (currentAlgorithm->step()) {
                status.setString("Step completed");
                updateGraphHighlights();
            } else {
                status.setString("Algorithm finished!");
            }
        }
    }

    void reset() {
        if (!currentAlgorithm || !graph) return;

        for (const auto& edge : graph->getEdges()) {
            edge->setHighlighted(false);
        }

        prepareAlgorithm();
        status.setString("Algorithm reset");
    }

    void updateGraphHighlights() {
        for (const auto& edge : graph->getEdges()) {
            edge->setHighlighted(false);
        }

        const auto& mstEdges = currentAlgorithm->getMSTEdges();
        for (const auto& mstEdge : mstEdges) {
            for (const auto& edge : graph->getEdges()) {
                if (edge->getStartNode()->getId() == mstEdge.src &&
                    edge->getEndNode()->getId() == mstEdge.dest) {
                    edge->setHighlighted(true);
                    break;
                }
            }
        }
    }
};