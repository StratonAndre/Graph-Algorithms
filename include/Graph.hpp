#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <map>
#include <random>
#include <cmath>
#include "Node.hpp"
#include "Edge.hpp"
#include "Theme.hpp"
#include "ViewportManager.hpp"

class Graph {
public:
    Graph() : isDirectedGraph(false), isOrderedGraph(false), algorithmModeEnabled(false), viewportManager(nullptr) {
        rng.seed(std::random_device()());
    }

    void setViewportManager(ViewportManager* manager) {
        viewportManager = manager;
    }

    void addNode(float x, float y) {
        int id = static_cast<int>(nodes.size());
        nodes.push_back(std::make_unique<Node>(x, y, id));
        nodeMap[id] = nodes.back().get();
    }

    void addNode(float x, float y, int id) {
        nodes.push_back(std::make_unique<Node>(x, y, id));
        nodeMap[id] = nodes.back().get();
    }

    void addEdge(Node* start, Node* end, std::optional<float> weight = std::nullopt) {
        if (!start || !end || start == end) return;

        bool forwardEdgeExists = false;
        bool reverseEdgeExists = false;

        for (const auto& edge : edges) {
            if (edge->getStartNode() == start && edge->getEndNode() == end) {
                forwardEdgeExists = true;
            }
            if (edge->getStartNode() == end && edge->getEndNode() == start) {
                reverseEdgeExists = true;
            }
        }

        if (forwardEdgeExists) return;

        auto newEdge = std::make_unique<Edge>(start, end, isDirectedGraph, weight);
        newEdge->setShowArrow(isOrderedGraph || isDirectedGraph);
        edges.push_back(std::move(newEdge));
    }

    void addEdgeById(int startId, int endId, std::optional<float> weight = std::nullopt) {
        Node* start = nodeMap[startId];
        Node* end = nodeMap[endId];
        if (start && end) {
            addEdge(start, end, weight);
        }
    }

    void setOrdered(bool ordered) {
        isOrderedGraph = ordered;
        for (auto& edge : edges) {
            edge->setShowArrow(isOrderedGraph || isDirectedGraph);
        }
    }

    bool isOrdered() const { return isOrderedGraph; }

    void setAlgorithmMode(bool enabled) {
        algorithmModeEnabled = enabled;
    }

    bool isAlgorithmMode() const {
        return algorithmModeEnabled;
    }

    void setDirected(bool directed) {
        isDirectedGraph = directed;
        updateAllEdges();
    }

    bool isDirected() const { return isDirectedGraph; }

    Node* findNodeAt(sf::Vector2f worldPos) {
        if (!viewportManager) return nullptr;

        float zoomLevel = viewportManager->getZoomLevel();
        float scaledRadius = Theme::NODE_RADIUS * zoomLevel;

        for (auto& node : nodes) {
            sf::Vector2f nodePos = node->getPosition();
            float dx = worldPos.x - nodePos.x;
            float dy = worldPos.y - nodePos.y;
            float distanceSquared = dx * dx + dy * dy;
            if (distanceSquared <= scaledRadius * scaledRadius) {
                return node.get();
            }
        }
        return nullptr;
    }

    void update(float deltaTime) {
        for (auto& node : nodes) {
            node->update(deltaTime);
        }

        for (auto& edge : edges) {
            edge->update();
        }

        applyForceDirectedLayout(deltaTime);
    }

    void draw(sf::RenderTarget& target) {
        for (const auto& edge : edges) {
            edge->draw(target, sf::RenderStates::Default);
        }

        for (const auto& node : nodes) {
            node->draw(target);
        }
    }

    void deleteNode(Node* node) {
        if (!node) return;

        edges.erase(
            std::remove_if(edges.begin(), edges.end(),
                [node](const std::unique_ptr<Edge>& edge) {
                    return edge->isConnectedTo(node);
                }
            ),
            edges.end()
        );

        nodes.erase(
            std::remove_if(nodes.begin(), nodes.end(),
                [node](const std::unique_ptr<Node>& n) {
                    return n.get() == node;
                }
            ),
            nodes.end()
        );

        for (auto it = nodeMap.begin(); it != nodeMap.end(); ) {
            if (it->second == node) {
                it = nodeMap.erase(it);
            } else {
                ++it;
            }
        }
    }

    std::vector<std::unique_ptr<Node>>& getNodes() { return nodes; }
    const std::vector<std::unique_ptr<Node>>& getNodes() const { return nodes; }
    const std::vector<std::unique_ptr<Edge>>& getEdges() const { return edges; }

    std::vector<Node*> getNeighbors(Node* node) const {
        std::vector<Node*> neighbors;
        for (const auto& edge : edges) {
            if (edge->getStartNode() == node) {
                neighbors.push_back(edge->getEndNode());
            } else if (!isDirectedGraph && edge->getEndNode() == node) {
                neighbors.push_back(edge->getStartNode());
            }
        }
        return neighbors;
    }

    std::vector<Node*> getOutgoingNeighbors(Node* node) const {
        std::vector<Node*> outNeighbors;
        for (const auto& edge : edges) {
            if (edge->getStartNode() == node) {
                outNeighbors.push_back(edge->getEndNode());
            }
        }
        return outNeighbors;
    }

    std::vector<Node*> getIncomingNeighbors(Node* node) const {
        std::vector<Node*> inNeighbors;
        for (const auto& edge : edges) {
            if (edge->getEndNode() == node) {
                inNeighbors.push_back(edge->getStartNode());
            }
        }
        return inNeighbors;
    }

    void clear() {
        nodes.clear();
        edges.clear();
        nodeMap.clear();
    }

private:
    std::vector<std::unique_ptr<Node>> nodes;
    std::vector<std::unique_ptr<Edge>> edges;
    std::map<int, Node*> nodeMap;
    bool isDirectedGraph;
    bool isOrderedGraph;
    bool algorithmModeEnabled;
    ViewportManager* viewportManager;
    std::mt19937 rng;

    void updateAllEdges() {
        for (auto& edge : edges) {
            edge->setDirected(isDirectedGraph);
            edge->setShowArrow(isOrderedGraph || isDirectedGraph);
        }
    }

    void applyForceDirectedLayout(float deltaTime) {
        if (nodes.empty()) return;

        const float springConstant = 50.0f;
        const float nodeRadius = nodes[0]->getRadius();

        const float baseRepulsion = algorithmModeEnabled ?
            (1000000.0f * (nodeRadius * 5.0f)) :
            100000.0f;
        const float damping = 0.8f;


        std::vector<sf::Vector2f> forces(nodes.size(), sf::Vector2f(0, 0));


        for (size_t i = 0; i < nodes.size(); ++i) {
            for (size_t j = i + 1; j < nodes.size(); ++j) {
                sf::Vector2f pos1 = nodes[i]->getPosition();
                sf::Vector2f pos2 = nodes[j]->getPosition();
                sf::Vector2f delta = pos1 - pos2;
                float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);

                if (distance > 0) {
                    float nodeRadius = nodes[i]->getRadius();
                    float repulsionFactor;

                    if (distance < nodeRadius) {
                        repulsionFactor = 2.0f;
                    } else if (distance < 2.0f * nodeRadius) {
                        float t = (distance - nodeRadius) / nodeRadius;
                        repulsionFactor = 2.0f * (1.0f - t * t);
                    } else {
                        repulsionFactor = 0.0f;
                    }

                    if (repulsionFactor > 0) {
                        sf::Vector2f normalizedDelta = delta / distance;
                        sf::Vector2f force = normalizedDelta * (baseRepulsion * repulsionFactor / (distance * distance));

                        if (distance < nodeRadius) {
                            force *= 2.0f;
                        }

                        forces[i] += force;
                        forces[j] -= force;
                    }
                }
            }
        }

        for (const auto& edge : edges) {
            Node* start = edge->getStartNode();
            Node* end = edge->getEndNode();

            auto startIt = std::find_if(nodes.begin(), nodes.end(),
                [start](const auto& n) { return n.get() == start; });
            auto endIt = std::find_if(nodes.begin(), nodes.end(),
                [end](const auto& n) { return n.get() == end; });

            if (startIt != nodes.end() && endIt != nodes.end()) {
                size_t startIndex = std::distance(nodes.begin(), startIt);
                size_t endIndex = std::distance(nodes.begin(), endIt);

                sf::Vector2f pos1 = start->getPosition();
                sf::Vector2f pos2 = end->getPosition();
                sf::Vector2f delta = pos1 - pos2;
                float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);

                if (distance > 0) {
                    float idealLength = algorithmModeEnabled ?
                        nodes[0]->getRadius() * 8.0f :
                        nodes[0]->getRadius() * 4.0f;

                    if (edge->getWeight()) {
                        idealLength *= (1.0f + edge->getWeight().value() * 0.1f);
                    }

                    sf::Vector2f force = delta * ((distance - idealLength) / distance);
                    forces[startIndex] -= force * springConstant * deltaTime;
                    forces[endIndex] += force * springConstant * deltaTime;
                }
            }
        }

        for (size_t i = 0; i < nodes.size(); ++i) {
            float maxForce = nodes[i]->getRadius() * 5.0f;
            sf::Vector2f& force = forces[i];
            float forceMagnitude = std::sqrt(force.x * force.x + force.y * force.y);

            if (forceMagnitude > maxForce) {
                force *= maxForce / forceMagnitude;
            }

            sf::Vector2f newPos = nodes[i]->getPosition() + force * damping * deltaTime;
            nodes[i]->setPosition(newPos);
        }
    }
};