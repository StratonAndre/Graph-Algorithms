#pragma once
#include "MSTAlgorithm.hpp"
#include <optional>
#include <limits>

class BoruvkaMST : public MSTAlgorithm {
private:
    std::vector<std::optional<WeightedEdge>> cheapest;
    int remainingComponents;
    int currentPhase;
    size_t currentComponentIndex;
    enum class StepPhase {
        INIT,
        FINDING_EDGES,
        MERGING_COMPONENTS,
        PHASE_COMPLETE
    } currentStepPhase;

public:
    std::string getName() const override {
        return "Boruvka's MST";
    }

    std::string getDescription() const override {
        return "Finds MST by simultaneously growing all components using their cheapest edges";
    }

    void execute(std::vector<std::unique_ptr<Node>>& nodes) override {
        parent.resize(nodes.size());
        rank.resize(nodes.size());
        mstEdges.clear();
        nodeStates.clear();
        algorithmSteps.clear();
        currentStep = 0;

        for (size_t i = 0; i < nodes.size(); i++) {
            makeSet(i);
            setNodeColor(i, MSTColors::UNVISITED);
            setComponentLabel(i, "Component " + std::to_string(i));
        }

        cheapest.resize(nodes.size());
        remainingComponents = nodes.size();
        currentPhase = 1;
        currentComponentIndex = 0;
        currentStepPhase = StepPhase::INIT;
        finished = false;

        addAlgorithmStep("Phase " + std::to_string(currentPhase) +
                        " initialized with " + std::to_string(remainingComponents) +
                        " components");
    }

    bool step() override {
        if (finished) return false;

        switch (currentStepPhase) {
            case StepPhase::INIT:
                initializePhase();
                break;
            case StepPhase::FINDING_EDGES:
                findCheapestEdges();
                break;
            case StepPhase::MERGING_COMPONENTS:
                mergeComponents();
                break;
            case StepPhase::PHASE_COMPLETE:
                completePhase();
                break;
        }

        return !finished;
    }

private:
    void initializePhase() {
        std::fill(cheapest.begin(), cheapest.end(), std::nullopt);
        for (auto& [nodeId, state] : nodeStates) {
            state.isHighlighted = false;
            setNodeColor(nodeId, MSTColors::PROCESSING);
        }

        currentStepPhase = StepPhase::FINDING_EDGES;
        currentComponentIndex = 0;

        addAlgorithmStep("Finding cheapest edges for each component...");
    }

    void findCheapestEdges() {
        if (currentComponentIndex >= parent.size()) {
            currentStepPhase = StepPhase::MERGING_COMPONENTS;
            currentComponentIndex = 0;
            return;
        }

        int currentSet = findSet(currentComponentIndex);


        for (size_t i = 0; i < parent.size(); i++) {
            if (findSet(i) == currentSet) {
                setNodeColor(i, MSTColors::CURRENT);
                setNodeHighlight(i, true);
            }
        }


        float minWeight = std::numeric_limits<float>::max();
        WeightedEdge* bestEdge = nullptr;

        for (auto& edge : edges) {
            int set1 = findSet(edge.src);
            int set2 = findSet(edge.dest);

            if (set1 == currentSet && set2 != currentSet) {
                if (!bestEdge || edge.weight < minWeight) {
                    bestEdge = &edge;
                    minWeight = edge.weight;
                }
            }
        }

        if (bestEdge) {
            cheapest[currentSet] = *bestEdge;
            setNodeColor(bestEdge->dest, MSTColors::CONSIDERING);

            addAlgorithmStep("Found cheapest edge for component " +
                           std::to_string(currentSet) + ": " + bestEdge->description);
        }

        currentComponentIndex++;
    }

    void mergeComponents() {
        bool addedEdge = false;

        for (size_t i = 0; i < cheapest.size(); i++) {
            if (cheapest[i]) {
                int set1 = findSet(cheapest[i]->src);
                int set2 = findSet(cheapest[i]->dest);

                if (set1 != set2) {
                    unionSets(set1, set2);
                    mstEdges.push_back(*cheapest[i]);
                    remainingComponents--;
                    addedEdge = true;


                    int newSet = findSet(set1);
                    for (size_t j = 0; j < parent.size(); j++) {
                        if (findSet(j) == newSet) {
                            setNodeColor(j, MSTColors::IN_MST);
                            setComponentLabel(j, "Component " + std::to_string(newSet));
                            setPulseEffect(j, 1.0f);
                        }
                    }

                    addAlgorithmStep("Merged components " + std::to_string(set1) +
                                   " and " + std::to_string(set2) +
                                   " using edge " + cheapest[i]->description);
                }
            }
        }

        currentStepPhase = StepPhase::PHASE_COMPLETE;
    }

    void completePhase() {
        if (remainingComponents <= 1 || !hasUnconnectedComponents()) {
            finished = true;
            addAlgorithmStep("Algorithm completed! Final MST has " +
                           std::to_string(mstEdges.size()) + " edges in " +
                           std::to_string(remainingComponents) + " component(s)");
        } else {
            currentPhase++;
            currentStepPhase = StepPhase::INIT;
            addAlgorithmStep("Starting Phase " + std::to_string(currentPhase) +
                           " with " + std::to_string(remainingComponents) +
                           " components remaining");
        }
    }

    bool hasUnconnectedComponents() {
        if (parent.empty()) return false;
        int firstSet = findSet(0);
        for (size_t i = 1; i < parent.size(); i++) {
            if (findSet(i) != firstSet) return true;
        }
        return false;
    }
};