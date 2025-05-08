#include "Application.hpp"

Application::Application() : selectedNode(nullptr), draggedNode(nullptr),
    isDragging(false), showAlgorithmPanel(false), gridOffset(0.f) {

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(desktop, "Graph Visualizer", sf::Style::Fullscreen,
                 sf::ContextSettings(0, 0, 8));

    float panelWidth = 300.f;
    float panelHeight = static_cast<float>(desktop.height);

    uiPanel = std::make_unique<UIPanel>(panelWidth, panelHeight);
    grid = std::make_unique<BackgroundGrid>(window.getSize());
    algorithmPanel = std::make_unique<AlgorithmPanel>(
        desktop.width - panelWidth - 10, 10, panelWidth, panelHeight);

    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    try {
        viewportManager = std::make_unique<ViewportManager>(window);

        sf::Vector2u screenSize = window.getSize();
        float padding = 500.f;
        sf::FloatRect initialBounds(-padding, -padding,
                                  screenSize.x + 2 * padding,
                                  screenSize.y + 2 * padding);
        viewportManager->setBounds(initialBounds);

        graph.setViewportManager(viewportManager.get());
        uiPanel->setGraph(&graph);
        algorithmPanel->setGraph(&graph);

        loadInitialGraph();
    } catch (const std::exception& e) {
        std::cerr << "Error during application initialization: " << e.what() << std::endl;
    }
}

void Application::run() {
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        handleEvents();
        update(deltaTime);
        render();
    }
}

void Application::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        viewportManager->handleEvent(event);
        uiPanel->handleEvent(event);

        if (showAlgorithmPanel) {
            algorithmPanel->handleEvent(event);
        }

        switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:
                handleKeyPress(event.key);
                break;

            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    handleMousePress(event.mouseButton);
                }
                break;

            case sf::Event::MouseButtonReleased:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    handleMouseRelease(event.mouseButton);
                }
                break;

            case sf::Event::MouseMoved:
                handleMouseMove(event);
                break;
        }
    }
}

void Application::update(float deltaTime) {
    graph.update(deltaTime);
    uiPanel->update(deltaTime);
    if (showAlgorithmPanel) {
        algorithmPanel->update(deltaTime);
    }
    grid->update(deltaTime);
}

void Application::render() {
    window.clear(Theme::BACKGROUND);
    sf::View uiView = window.getDefaultView();
    viewportManager->applyView();

    grid->draw(window);
    graph.draw(window);

    window.setView(uiView);
    uiPanel->draw(window);

    if (showAlgorithmPanel) {
        algorithmPanel->draw(window);
    }

    window.display();
}

void Application::handleKeyPress(const sf::Event::KeyEvent& key) {
    if (key.code == sf::Keyboard::Escape) {
        window.close();
        return;
    }

    if (key.code == sf::Keyboard::R) {
        resetViewportToFitGraph();
        return;
    }

    if (key.code == sf::Keyboard::G) {
        grid->toggleAnimation();
        return;
    }

    if (key.control) {
        switch (key.code) {
            case sf::Keyboard::S:
                saveGraph("output.txt");
                break;

            case sf::Keyboard::L:
                loadGraph("input.txt");
                break;

            case sf::Keyboard::A:
                toggleAlgorithmPanel();
                break;
        }
    }

    if (key.code == sf::Keyboard::Delete && selectedNode) {
        graph.deleteNode(selectedNode);
        selectedNode = nullptr;
        draggedNode = nullptr;
        uiPanel->setSelectedNode(nullptr);
        resetViewportToFitGraph();
    }
}

void Application::handleMousePress(const sf::Event::MouseButtonEvent& mouseButton) {
    sf::Vector2i screenPos(mouseButton.x, mouseButton.y);

    if (uiPanel->contains(sf::Vector2f(mouseButton.x, mouseButton.y)) ||
        (showAlgorithmPanel && algorithmPanel->contains(sf::Vector2f(mouseButton.x, mouseButton.y)))) {
        return;
    }

    sf::Vector2f worldPos = viewportManager->screenToWorld(screenPos);
    Node* clickedNode = graph.findNodeAt(worldPos);

    if (clickedNode) {
        if (selectedNode && selectedNode != clickedNode) {
            graph.addEdge(selectedNode, clickedNode);
            selectedNode->setSelected(false);
            selectedNode = nullptr;
            uiPanel->setSelectedNode(nullptr);
        } else {
            if (selectedNode) {
                selectedNode->setSelected(false);
            }
            selectedNode = clickedNode;
            selectedNode->setSelected(true);
            draggedNode = clickedNode;
            isDragging = true;
            uiPanel->setSelectedNode(clickedNode);
        }
    } else if (!isDragging) {
        graph.addNode(worldPos.x, worldPos.y);
        updateViewportBounds(worldPos);

        if (selectedNode) {
            selectedNode->setSelected(false);
            selectedNode = nullptr;
            uiPanel->setSelectedNode(nullptr);
        }
    }
}

void Application::handleMouseRelease(const sf::Event::MouseButtonEvent& mouseButton) {
    if (isDragging && draggedNode) {
        updateViewportBounds(draggedNode->getPosition());
    }
    isDragging = false;
    draggedNode = nullptr;
}

void Application::handleMouseMove(const sf::Event& event) {
    if (isDragging && draggedNode) {
        sf::Vector2i screenPos(event.mouseMove.x, event.mouseMove.y);
        sf::Vector2f worldPos = viewportManager->screenToWorld(screenPos);
        draggedNode->setPosition(worldPos);
        updateViewportBounds(worldPos);
    }
}

void Application::loadInitialGraph() {
    try {
        GraphIO::loadFromFile(graph, "input.txt");
        resetViewportToFitGraph();
    } catch (const std::exception& e) {
        std::cerr << "Could not load initial graph: " << e.what() << std::endl;
    }
}

void Application::saveGraph(const std::string& filename) {
    try {
        GraphIO::saveToFile(graph, filename);
        std::cout << "Graph saved to " << filename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Could not save graph: " << e.what() << std::endl;
    }
}

void Application::loadGraph(const std::string& filename) {
    try {
        GraphIO::loadFromFile(graph, filename);
        selectedNode = nullptr;
        draggedNode = nullptr;
        uiPanel->setSelectedNode(nullptr);
        resetViewportToFitGraph();
    } catch (const std::exception& e) {
        std::cerr << "Could not load graph: " << e.what() << std::endl;
    }
}

void Application::toggleAlgorithmPanel() {
    showAlgorithmPanel = !showAlgorithmPanel;
    if (showAlgorithmPanel) {
        algorithmPanel->setGraph(&graph);
    }
}

void Application::updateViewportBounds(const sf::Vector2f& newPoint) {
    sf::FloatRect currentBounds = viewportManager->getBounds();
    float padding = 200.0f;
    bool boundsChanged = false;

    if (newPoint.x < currentBounds.left + padding) {
        currentBounds.width += currentBounds.left - (newPoint.x - padding);
        currentBounds.left = newPoint.x - padding;
        boundsChanged = true;
    }
    if (newPoint.x > currentBounds.left + currentBounds.width - padding) {
        currentBounds.width = newPoint.x - currentBounds.left + padding;
        boundsChanged = true;
    }
    if (newPoint.y < currentBounds.top + padding) {
        currentBounds.height += currentBounds.top - (newPoint.y - padding);
        currentBounds.top = newPoint.y - padding;
        boundsChanged = true;
    }
    if (newPoint.y > currentBounds.top + currentBounds.height - padding) {
        currentBounds.height = newPoint.y - currentBounds.top + padding;
        boundsChanged = true;
    }

    if (boundsChanged) {
        viewportManager->setBounds(currentBounds);
    }
}

void Application::resetViewportToFitGraph() {
    const auto& nodes = graph.getNodes();
    if (nodes.empty()) {
        sf::Vector2u screenSize = window.getSize();
        float padding = 500.f;
        viewportManager->setBounds(sf::FloatRect(-padding, -padding,
                                               screenSize.x + 2 * padding,
                                               screenSize.y + 2 * padding));
        return;
    }

    sf::Vector2f minPos = nodes[0]->getPosition();
    sf::Vector2f maxPos = minPos;

    for (const auto& node : nodes) {
        sf::Vector2f pos = node->getPosition();
        minPos.x = std::min(minPos.x, pos.x);
        minPos.y = std::min(minPos.y, pos.y);
        maxPos.x = std::max(maxPos.x, pos.x);
        maxPos.y = std::max(maxPos.y, pos.y);
    }

    float padding = 500.0f;
    sf::FloatRect newBounds(
        minPos.x - padding,
        minPos.y - padding,
        maxPos.x - minPos.x + 2 * padding,
        maxPos.y - minPos.y + 2 * padding
    );

    viewportManager->setBounds(newBounds);
    viewportManager->reset();
}