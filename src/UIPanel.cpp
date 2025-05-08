#include "UIPanel.hpp"
#include <iostream>
#include <sstream>
#include <set>
#include <algorithm>

UIPanel::UIPanel(float width, float height) :
    selectedNode(nullptr),
    scrollOffset(0.f),
    isOrdered(false),
    fontLoaded(false),
    graph(nullptr),
    panelWidth(width),
    panelTargetX(1920.f - width),
    panelCurrentX(1920.f - width) {

    try {
        panel.setSize(sf::Vector2f(width, height));
        panel.setFillColor(Theme::PANEL_BACKGROUND);
        panel.setPosition(panelCurrentX, 0);

        float buttonX = 10;
        float buttonY = 10;
        float buttonSpacing = 50;
        float buttonWidth = 180;
        float buttonHeight = 40;

        createButton("Directed Mode",
                   sf::Vector2f(buttonX, buttonY),
                   sf::Vector2f(buttonWidth, buttonHeight),
                   [this]() {
                       if (graph) {
                           graph->setDirected(!graph->isDirected());
                           updateButtonText();
                       }
                   });
        buttonY += buttonSpacing;

        createButton("Unordered Graph",
                   sf::Vector2f(buttonX, buttonY),
                   sf::Vector2f(buttonWidth, buttonHeight),
                   [this]() {
                       toggleOrder();
                   });


        if (font.loadFromFile("resources/Roboto-Medium.ttf")) {
            fontLoaded = true;
        } else {
            std::cerr << "Warning: Could not load font" << std::endl;
        }

        if (fontLoaded) {
            title.setFont(font);
            title.setCharacterSize(20);
            title.setFillColor(Theme::TEXT_PRIMARY);
            title.setPosition(panelCurrentX + 10, 20);

            scrollView.setPosition(panelCurrentX + 10, 60);
            scrollView.setSize(sf::Vector2f(width - 20, height - 100));
            scrollView.setFillColor(sf::Color::Transparent);
            scrollView.setOutlineColor(Theme::NODE_OUTLINE);
            scrollView.setOutlineThickness(1.f);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in UIPanel constructor: " << e.what() << std::endl;
    }
}

void UIPanel::setGraph(Graph* g) {
    graph = g;
    updateButtonText();
}

void UIPanel::setSelectedNode(Node* node) {
    selectedNode = node;
    scrollOffset = 0.f;


    if (node) {

        float startX = panelCurrentX;
        slideAnimation = std::make_unique<Animation>(
            Theme::ANIMATION_DURATION,
            [this, startX](float progress) {
                panelCurrentX = startX + (panelTargetX - startX) * progress;
                updatePanelPosition();
            }
        );
    } else {

        float startX = panelCurrentX;
        float endX = 1920.f;
        slideAnimation = std::make_unique<Animation>(
            Theme::ANIMATION_DURATION,
            [this, startX, endX](float progress) {
                panelCurrentX = startX + (endX - startX) * progress;
                updatePanelPosition();
            }
        );
    }
}

const std::vector<std::unique_ptr<Button>>& UIPanel::getButtons() const {
    return buttons;
}

bool UIPanel::contains(sf::Vector2f point) const {
    return panel.getGlobalBounds().contains(point);
}

void UIPanel::createButton(const std::string& text, const sf::Vector2f& position,
                         const sf::Vector2f& size, std::function<void()> callback) {
    buttons.push_back(std::make_unique<Button>(text, position, size, callback));
}

float UIPanel::drawDirectedNeighbors(sf::RenderWindow& window, float y, float viewHeight, float itemHeight) {
    if (!selectedNode || !graph) return y;

    auto outNeighbors = graph->getOutgoingNeighbors(selectedNode);
    auto inNeighbors = graph->getIncomingNeighbors(selectedNode);

    std::map<Node*, std::string> connections;

    for (auto* node : outNeighbors) {
        connections[node] = "->";
    }

    for (auto* node : inNeighbors) {
        if (connections[node] == "->") {
            connections[node] = "<->";
        } else {
            connections[node] = "<-";
        }
    }

    std::vector<std::pair<Node*, std::string>> sortedConnections(connections.begin(), connections.end());
    if (isOrdered) {
        std::sort(sortedConnections.begin(), sortedConnections.end(),
            [](const auto& a, const auto& b) { return a.first->getId() < b.first->getId(); });
    }

    for (const auto& [node, type] : sortedConnections) {
        if (y + itemHeight >= scrollView.getPosition().y &&
            y <= scrollView.getPosition().y + viewHeight) {
            sf::Text nodeText;
            nodeText.setFont(font);

            std::string connectionText = std::to_string(selectedNode->getId()) + " " +
                                       type + " " + std::to_string(node->getId());
            nodeText.setString(connectionText);
            nodeText.setCharacterSize(16);
            nodeText.setFillColor(Theme::TEXT_PRIMARY);
            nodeText.setPosition(scrollView.getPosition().x + 20, y + 5);
            window.draw(nodeText);
        }
        y += itemHeight;
    }

    return y;
}

float UIPanel::drawUndirectedNeighbors(sf::RenderWindow& window, float y, float viewHeight, float itemHeight) {
    if (!selectedNode || !graph) return y;

    auto neighbors = graph->getNeighbors(selectedNode);
    if (isOrdered) {
        std::sort(neighbors.begin(), neighbors.end(),
            [](const Node* a, const Node* b) { return a->getId() < b->getId(); });
    }

    std::string connectionText = "Connected to: ";
    for (size_t i = 0; i < neighbors.size(); ++i) {
        connectionText += std::to_string(neighbors[i]->getId());
        if (i < neighbors.size() - 1) {
            connectionText += ", ";
        }
    }

    sf::Text nodeText;
    nodeText.setFont(font);
    nodeText.setString(connectionText);
    nodeText.setCharacterSize(16);
    nodeText.setFillColor(Theme::TEXT_PRIMARY);
    nodeText.setPosition(scrollView.getPosition().x + 20, y + 5);
    window.draw(nodeText);

    return y + itemHeight;
}

float UIPanel::getMaxScroll() const {
    if (!selectedNode || !graph) return 0.f;
    const float ITEM_HEIGHT = 30.f;
    float contentHeight;

    if (graph->isDirected()) {
        auto outNeighbors = graph->getOutgoingNeighbors(selectedNode);
        auto inNeighbors = graph->getIncomingNeighbors(selectedNode);
        std::set<Node*> uniqueNeighbors;
        for (auto* node : outNeighbors) uniqueNeighbors.insert(node);
        for (auto* node : inNeighbors) uniqueNeighbors.insert(node);
        contentHeight = uniqueNeighbors.size() * ITEM_HEIGHT;
    } else {
        auto neighbors = graph->getNeighbors(selectedNode);
        contentHeight = 2 * ITEM_HEIGHT;
    }

    float viewHeight = scrollView.getSize().y;
    return std::max(0.f, contentHeight - viewHeight);
}

void UIPanel::toggleOrder() {
    isOrdered = !isOrdered;
    if (graph) {
        graph->setOrdered(isOrdered);
    }

    auto orderBtn = std::find_if(buttons.begin(), buttons.end(),
        [](const auto& btn) {
            return btn->getText().find("Graph") != std::string::npos;
        });

    if (orderBtn != buttons.end()) {
        (*orderBtn)->setText(isOrdered ? "Ordered Graph" : "Unordered Graph");
    }
}

void UIPanel::updateButtonText() {
    if (!graph) return;

    auto directedBtn = std::find_if(buttons.begin(), buttons.end(),
        [](const auto& btn) {
            return btn->getText().find("Mode") != std::string::npos;
        });

    if (directedBtn != buttons.end()) {
        (*directedBtn)->setText(graph->isDirected() ? "Undirected Mode" : "Directed Mode");
    }
}

void UIPanel::handleEvent(sf::Event& event) {
    try {
        if (event.type == sf::Event::MouseWheelScrolled && selectedNode) {
            float maxScroll = getMaxScroll();
            scrollOffset = std::max(0.f,
                         std::min(scrollOffset - event.mouseWheelScroll.delta * 30.f,
                                 maxScroll));
        }

        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
            for (auto& button : buttons) {
                button->setHovered(button->contains(mousePos));
            }
        }

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            for (auto& button : buttons) {
                if (button->contains(mousePos)) {
                    button->handleClick();
                    break;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in UIPanel::handleEvent: " << e.what() << std::endl;
    }
}

void UIPanel::update(float deltaTime) {
    if (slideAnimation) {
        slideAnimation->update(deltaTime);
        if (slideAnimation->isFinished()) {
            slideAnimation.reset();
        }
    }
    updatePanelPosition();
}

void UIPanel::updatePanelPosition() {
    panel.setPosition(panelCurrentX, 0);
    title.setPosition(panelCurrentX + 10, 20);
    scrollView.setPosition(panelCurrentX + 10, 60);
}

void UIPanel::draw(sf::RenderWindow& window) {
    try {

        for (const auto& button : buttons) {
            button->draw(window);
        }


        window.draw(panel);

        if (selectedNode && fontLoaded && graph) {

            std::string titleText = "Node " + std::to_string(selectedNode->getId());
            if (graph->isDirected()) {
                titleText += " Connections (Directed)";
            } else {
                titleText += " Connections" + std::string(isOrdered ? " (Ordered)" : "");
            }
            title.setString(titleText);
            window.draw(title);

            window.draw(scrollView);

            float y = scrollView.getPosition().y - scrollOffset;
            const float ITEM_HEIGHT = 30.f;
            const float VIEW_HEIGHT = scrollView.getSize().y;

            if (graph->isDirected()) {
                y = drawDirectedNeighbors(window, y, VIEW_HEIGHT, ITEM_HEIGHT);
            } else {
                y = drawUndirectedNeighbors(window, y, VIEW_HEIGHT, ITEM_HEIGHT);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in UIPanel::draw: " << e.what() << std::endl;
    }
}