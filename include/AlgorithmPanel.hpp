#pragma once
#include <SFML/Graphics.hpp>
#include "Button.hpp"
#include "Graph.hpp"
#include "Theme.hpp"
#include "algorithms/MSTAlgorithm.hpp"
#include <memory>
#include <string>
#include <functional>
#include <sstream>
#include <map>
#include <iostream>
#include "algorithms/KruskalMST.hpp"
#include "algorithms/BoruvkaMST.hpp"
#include "GraphIO.hpp"

class AlgorithmPanel {
public:
    AlgorithmPanel(float x, float y, float width, float height) {
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
                    [this]() { if (currentAlgorithm && !isAnimating) step(); });

        buttonY += buttonSpacing;
        createButton("Reset", sf::Vector2f(x + 10, buttonY),
                    sf::Vector2f(buttonWidth, buttonHeight),
                    [this]() { if (currentAlgorithm) reset(); });

        buttonY += buttonSpacing;
        createButton("Run Animation", sf::Vector2f(x + 10, buttonY),
                    sf::Vector2f(buttonWidth, buttonHeight),
                    [this]() { if (currentAlgorithm && !currentAlgorithm->isFinished()) toggleAnimation(); });

        buttonY += buttonSpacing;
        createButton("Kruskal's Algorithm", sf::Vector2f(x + 10, buttonY),
                    sf::Vector2f(buttonWidth, buttonHeight),
                    [this]() { selectAlgorithm("Kruskal"); });

        buttonY += buttonSpacing;
        createButton("Boruvka's Algorithm", sf::Vector2f(x + 10, buttonY),
                    sf::Vector2f(buttonWidth, buttonHeight),
                    [this]() { selectAlgorithm("Boruvka"); });

        if (font.loadFromFile("resources/Roboto-Medium.ttf")) {
            status.setFont(font);
            status.setCharacterSize(14);
            status.setFillColor(Theme::TEXT_PRIMARY);
            status.setPosition(x + 10, buttonY + buttonSpacing);

            description.setFont(font);
            description.setCharacterSize(14);
            description.setFillColor(Theme::TEXT_PRIMARY);
            description.setPosition(x + 10, buttonY + buttonSpacing * 2);
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


        if (isTransitioning) {
            transitionTimer += deltaTime;
            if (transitionTimer >= TRANSITION_DURATION) {
                isTransitioning = false;
                transitionTimer = 0;
            }
            updateGraphVisualization();
        }


        if (isAnimating && currentAlgorithm && !currentAlgorithm->isFinished()) {
            animationTimer += deltaTime;
            if (animationTimer >= ANIMATION_STEP_DURATION) {
                animationTimer = 0;
                step();
                updateAnimationButtonText();

                if (currentAlgorithm->isFinished()) {
                    stopAnimation();
                }
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(panel);
        for (const auto& button : buttons) {
            button->draw(window);
        }
        window.draw(status);
        window.draw(description);
    }

private:
    sf::RectangleShape panel;
    sf::FloatRect panelBounds;
    std::vector<std::unique_ptr<Button>> buttons;
    Graph* graph = nullptr;
    std::unique_ptr<MSTAlgorithm> currentAlgorithm;
    sf::Text status;
    sf::Text description;
    sf::Font font;
    bool isAnimating = false;
    bool isTransitioning = false;
    float animationTimer = 0;
    float transitionTimer = 0;
    std::map<int, MSTVisualization::NodeState> nodeStates;
    const float ANIMATION_STEP_DURATION = 1.0f;
    const float TRANSITION_DURATION = 0.5f;

    void createButton(const std::string& text, const sf::Vector2f& position,
                     const sf::Vector2f& size, std::function<void()> callback) {
        buttons.push_back(std::make_unique<Button>(text, position, size, callback));
    }

    void loadAlgorithmGraph(const std::string& algorithmName) {
        if (!graph) return;


        graph->clear();
        graph->setAlgorithmMode(false);


        std::string filename = algorithmName + "_input.txt";
        std::cout << "Attempting to load algorithm input file: " << filename << std::endl;

        try {

            GraphIO::loadFromFile(*graph, filename);


            if (algorithmName == "Kruskal") {
                std::cout << "Creating Kruskal's algorithm instance" << std::endl;
                currentAlgorithm = std::make_unique<KruskalMST>();
            } else if (algorithmName == "Boruvka") {
                std::cout << "Creating Boruvka's algorithm instance" << std::endl;
                currentAlgorithm = std::make_unique<BoruvkaMST>();
            }

            if (currentAlgorithm) {
                std::cout << "Setting algorithm mode and preparing algorithm" << std::endl;
                graph->setAlgorithmMode(true);
                prepareAlgorithm();
            }
        } catch (const std::exception& e) {
            std::cerr << "Error loading algorithm graph: " << e.what() << std::endl;
        }
    }

    void selectAlgorithm(const std::string& name) {
        if (!graph) return;
        stopAnimation();
        loadAlgorithmGraph(name);
    }

    void prepareAlgorithm() {
        if (!currentAlgorithm || !graph) return;

        currentAlgorithm->reset();
        nodeStates.clear();


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
        isTransitioning = true;
        transitionTimer = 0;
        updateGraphVisualization();


        status.setString("Algorithm initialized");
        description.setString(currentAlgorithm->getDescription());
    }

    void toggleAnimation() {
        isAnimating = !isAnimating;
        animationTimer = 0;
        updateAnimationButtonText();
    }

    void stopAnimation() {
        isAnimating = false;
        animationTimer = 0;
        updateAnimationButtonText();
    }

    void updateAnimationButtonText() {
        auto animButton = std::find_if(buttons.begin(), buttons.end(),
            [](const auto& btn) { return btn->getText().find("Animation") != std::string::npos; });
        if (animButton != buttons.end()) {
            (*animButton)->setText(isAnimating ? "Pause Animation" : "Run Animation");
        }
    }

    void step() {
        if (!currentAlgorithm || !graph) return;

        if (!currentAlgorithm->isFinished()) {
            if (currentAlgorithm->step()) {
                isTransitioning = true;
                transitionTimer = 0;
                updateGraphVisualization();
            } else {
                status.setString("Algorithm finished!");
                stopAnimation();
            }
        }
    }

    void reset() {
        if (!currentAlgorithm || !graph) return;

        stopAnimation();


        for (const auto& node : graph->getNodes()) {
            node->setColor(Theme::NODE_FILL);
            node->setStatusLabel("");
            node->setHighlighted(false);
            node->setScale(1.0f);
        }


        for (const auto& edge : graph->getEdges()) {
            edge->setHighlighted(false);
        }

        graph->setAlgorithmMode(false);
        prepareAlgorithm();
        status.setString("Algorithm reset");
    }

    void updateGraphVisualization() {
        if (!currentAlgorithm || !graph) return;

        for (const auto& node : graph->getNodes()) {
            const auto& nodeState = currentAlgorithm->getNodeState(node->getId());

            if (isTransitioning) {
                float progress = transitionTimer / TRANSITION_DURATION;
                sf::Color currentColor = node->getColor();
                sf::Color targetColor = nodeState.color;

                sf::Color interpolatedColor(
                    currentColor.r + (targetColor.r - currentColor.r) * progress,
                    currentColor.g + (targetColor.g - currentColor.g) * progress,
                    currentColor.b + (targetColor.b - currentColor.b) * progress
                );
                node->setColor(interpolatedColor);
            } else {
                node->setColor(nodeState.color);
            }

            node->setStatusLabel(nodeState.componentLabel);
            node->setHighlighted(nodeState.isHighlighted);

            if (nodeState.pulseEffect > 0) {
                node->setScale(1.0f + 0.2f * nodeState.pulseEffect);
            }
        }

        updateGraphHighlights();

        std::string stepDescription = currentAlgorithm->getCurrentStepDescription();
        if (!stepDescription.empty()) {
            status.setString(stepDescription);
            wrapText(status, panel.getSize().x - 20);
        }
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

    void wrapText(sf::Text& text, float maxWidth) {
        std::string str = text.getString();
        std::string wrappedText;
        std::stringstream ss(str);
        std::string word;
        float lineWidth = 0;

        while (ss >> word) {
            sf::Text tempText;
            tempText.setFont(*text.getFont());
            tempText.setString(word + " ");
            tempText.setCharacterSize(text.getCharacterSize());
            float wordWidth = tempText.getLocalBounds().width;

            if (lineWidth + wordWidth > maxWidth) {
                wrappedText += "\n" + word + " ";
                lineWidth = wordWidth;
            } else {
                wrappedText += word + " ";
                lineWidth += wordWidth;
            }
        }

        text.setString(wrappedText);
    }
};