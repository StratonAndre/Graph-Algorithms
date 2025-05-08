#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <memory>
#include <filesystem>
#include <iostream>
#include "Graph.hpp"
#include "tinyxml2.h"

class LuxembourgMapLoader {
private:
    struct MapNode {
        int id;
        double latitude;
        double longitude;
    };

    struct MapArc {
        int from;
        int to;
        int length;
    };

    static std::pair<double, double> findCoordinateBounds(const std::vector<MapNode>& nodes) {
        if (nodes.empty()) return {0.0, 0.0};

        double minLat = nodes[0].latitude;
        double maxLat = nodes[0].latitude;
        double minLon = nodes[0].longitude;
        double maxLon = nodes[0].longitude;

        for (const auto& node : nodes) {
            minLat = std::min(minLat, node.latitude);
            maxLat = std::max(maxLat, node.latitude);
            minLon = std::min(minLon, node.longitude);
            maxLon = std::max(maxLon, node.longitude);
        }

        return {maxLat - minLat, maxLon - minLon};
    }

    static sf::Vector2f convertToScreenCoordinates(double lat, double lon,
                                                 double minLat, double minLon,
                                                 double latRange, double lonRange,
                                                 float screenWidth, float screenHeight) {

        const float PADDING = 50.0f;
        float usableWidth = screenWidth - 2 * PADDING;
        float usableHeight = screenHeight - 2 * PADDING;

        float x = PADDING + (lon - minLon) / lonRange * usableWidth;
        float y = PADDING + (1.0 - (lat - minLat) / latRange) * usableHeight;

        return sf::Vector2f(x, y);
    }

public:
    static void loadFromXML(Graph& graph, const std::string& filename, sf::Vector2u windowSize) {

        std::vector<std::filesystem::path> possiblePaths = {
            filename,
            "resources/" + filename,
            "../resources/" + filename,
            std::filesystem::current_path() / "resources" / filename
        };

        tinyxml2::XMLDocument doc;
        bool loaded = false;

        for (const auto& path : possiblePaths) {
            std::cout << "Trying to load from: " << path << std::endl;
            if (doc.LoadFile(path.string().c_str()) == tinyxml2::XML_SUCCESS) {
                std::cout << "Successfully loaded from: " << path << std::endl;
                loaded = true;
                break;
            }
        }

        if (!loaded) {
            throw std::runtime_error("Failed to load XML file from any path");
        }

        std::vector<MapNode> nodes;
        std::vector<MapArc> arcs;


        auto* xmlNode = doc.FirstChildElement("map")->FirstChildElement("node");
        while (xmlNode) {
            MapNode node;
            node.id = std::stoi(xmlNode->Attribute("id"));
            node.latitude = std::stod(xmlNode->Attribute("latitude"));
            node.longitude = std::stod(xmlNode->Attribute("longitude"));
            nodes.push_back(node);
            xmlNode = xmlNode->NextSiblingElement("node");
        }


        auto* xmlArc = doc.FirstChildElement("map")->FirstChildElement("arc");
        while (xmlArc) {
            MapArc arc;
            arc.from = std::stoi(xmlArc->Attribute("from"));
            arc.to = std::stoi(xmlArc->Attribute("to"));
            arc.length = std::stoi(xmlArc->Attribute("length"));
            arcs.push_back(arc);
            xmlArc = xmlArc->NextSiblingElement("arc");
        }


        double minLat = nodes[0].latitude;
        double maxLat = nodes[0].latitude;
        double minLon = nodes[0].longitude;
        double maxLon = nodes[0].longitude;

        for (const auto& node : nodes) {
            minLat = std::min(minLat, node.latitude);
            maxLat = std::max(maxLat, node.latitude);
            minLon = std::min(minLon, node.longitude);
            maxLon = std::max(maxLon, node.longitude);
        }

        auto [latRange, lonRange] = findCoordinateBounds(nodes);


        graph.clear();


        std::map<int, Node*> nodeMap;
        for (const auto& node : nodes) {
            sf::Vector2f pos = convertToScreenCoordinates(
                node.latitude, node.longitude,
                minLat, minLon, latRange, lonRange,
                windowSize.x, windowSize.y
            );
            graph.addNode(pos.x, pos.y, node.id);
            nodeMap[node.id] = graph.getNodes().back().get();
        }


        for (const auto& arc : arcs) {
            if (nodeMap.count(arc.from) && nodeMap.count(arc.to)) {
                graph.addEdge(nodeMap[arc.from], nodeMap[arc.to],
                            static_cast<float>(arc.length));
            }
        }
    }
};