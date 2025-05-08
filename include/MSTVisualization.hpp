#pragma once
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <SFML/Graphics.hpp>
#include "MSTTypes.hpp"

class MSTVisualization {
public:
    struct NodeState {
        sf::Color color;
        std::string label;
        bool isHighlighted;
        std::string componentLabel;
        float pulseEffect;
        float scale;

        NodeState() :
            color(MSTColors::UNVISITED),
            isHighlighted(false),
            pulseEffect(0.0f),
            scale(1.0f) {}
    };

    virtual ~MSTVisualization() = default;

    const NodeState& getNodeState(int nodeId) const {
        auto it = nodeStates.find(nodeId);
        return it != nodeStates.end() ? it->second : defaultState;
    }

    const std::string& getCurrentStepDescription() const {
        return currentStep < algorithmSteps.size() ?
               algorithmSteps[currentStep] : emptyDescription;
    }

protected:
    std::map<int, NodeState> nodeStates;
    std::vector<std::string> algorithmSteps;
    size_t currentStep = 0;

    inline static NodeState defaultState{};
    inline static std::string emptyDescription = "";

    void setNodeColor(int nodeId, const sf::Color& color) {
        nodeStates[nodeId].color = color;
    }

    void setNodeLabel(int nodeId, const std::string& label) {
        nodeStates[nodeId].label = label;
    }

    void setNodeHighlight(int nodeId, bool highlighted) {
        nodeStates[nodeId].isHighlighted = highlighted;
    }

    void setComponentLabel(int nodeId, const std::string& label) {
        nodeStates[nodeId].componentLabel = label;
    }

    void setPulseEffect(int nodeId, float value) {
        nodeStates[nodeId].pulseEffect = value;
    }

    void setNodeScale(int nodeId, float scale) {
        nodeStates[nodeId].scale = scale;
    }

    void addAlgorithmStep(const std::string& description) {
        std::stringstream ss;
        ss << "Step " << (currentStep + 1) << ": " << description;
        algorithmSteps.push_back(ss.str());
        currentStep++;
    }

    std::string formatWeight(float weight) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << weight;
        return ss.str();
    }
};