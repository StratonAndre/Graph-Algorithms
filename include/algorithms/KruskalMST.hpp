#pragma once
#include "MSTAlgorithm.hpp"
#include <queue>
#include <sstream>

class KruskalMST : public MSTAlgorithm {
private:
    std::priority_queue<WeightedEdge, std::vector<WeightedEdge>, std::greater<WeightedEdge>> edgeQueue;
    std::vector<WeightedEdge> allEdges;
    WeightedEdge currentEdge;
    bool initialized;
    bool hasCurrentEdge;
    std::stringstream summary;
    float totalWeight;

    enum class StepPhase {
        INIT,
        SORTING,
        PROCESSING,
        FINALIZING,
        COMPLETE
    } currentStepPhase;

public:
    KruskalMST() : initialized(false), hasCurrentEdge(false), totalWeight(0) {}

    std::string getName() const override {
        return "Kruskal's MST";
    }

    std::string getDescription() const override {
        return "Finds MST by repeatedly selecting the minimum weight edge that doesn't create a cycle";
    }

    void execute(std::vector<std::unique_ptr<Node>>& nodes) override {
        parent.resize(nodes.size());
        rank.resize(nodes.size());
        mstEdges.clear();
        nodeStates.clear();
        algorithmSteps.clear();
        currentStep = 0;
        allEdges.clear();
        hasCurrentEdge = false;
        totalWeight = 0;
        summary.str("");
        summary.clear();

        for (size_t i = 0; i < nodes.size(); i++) {
            makeSet(i);
            setNodeColor(i, MSTColors::UNVISITED);
            setNodeHighlight(i, false);
            setPulseEffect(i, 0.0f);
        }

        while (!edgeQueue.empty()) edgeQueue.pop();
        for (const auto& edge : edges) {
            edgeQueue.push(edge);
            allEdges.push_back(edge);
        }

        currentStepPhase = StepPhase::INIT;
        initialized = false;
        finished = false;

        addAlgorithmStep("Initialized with " + std::to_string(nodes.size()) + " nodes and " +
                        std::to_string(edges.size()) + " edges");
    }

    bool step() override {
        if (finished) {
            resetForNextIteration();
            return true;
        }

        switch (currentStepPhase) {
            case StepPhase::INIT:
                handleInitPhase();
                break;
            case StepPhase::SORTING:
                handleSortingPhase();
                break;
            case StepPhase::PROCESSING:
                handleProcessingPhase();
                break;
            case StepPhase::FINALIZING:
                handleFinalizingPhase();
                break;
            case StepPhase::COMPLETE:
                finished = true;
                break;
        }

        return true;
    }

    void reset() override {
        MSTAlgorithm::reset();
        while (!edgeQueue.empty()) edgeQueue.pop();
        allEdges.clear();
        hasCurrentEdge = false;
        totalWeight = 0;
        summary.str("");
        summary.clear();
        currentStepPhase = StepPhase::INIT;
        initialized = false;
        finished = false;
    }

private:
    void handleInitPhase() {
        if (!initialized) {
            initialized = true;
            currentStepPhase = StepPhase::SORTING;
            addAlgorithmStep("Sorting edges by weight...");
        }
    }

    void handleSortingPhase() {
        if (!edgeQueue.empty()) {
            if (hasCurrentEdge) {
                updateNodeColors(currentEdge, false);
            }
            currentEdge = edgeQueue.top();
            edgeQueue.pop();
            hasCurrentEdge = true;

            updateNodeColors(currentEdge, true);
            addAlgorithmStep("Considering edge " + currentEdge.description);
            currentStepPhase = StepPhase::PROCESSING;
        } else {
            currentStepPhase = StepPhase::FINALIZING;
        }
    }

    void handleProcessingPhase() {
        if (hasCurrentEdge) {
            int set1 = findSet(currentEdge.src);
            int set2 = findSet(currentEdge.dest);

            if (set1 != set2) {
                unionSets(set1, set2);
                mstEdges.push_back(currentEdge);
                totalWeight += currentEdge.weight;

                setNodeColor(currentEdge.src, MSTColors::IN_MST);
                setNodeColor(currentEdge.dest, MSTColors::IN_MST);
                setPulseEffect(currentEdge.src, 1.0f);
                setPulseEffect(currentEdge.dest, 1.0f);

                std::string edgeInfo = formatEdgeInfo(currentEdge);
                addAlgorithmStep("Added " + edgeInfo);
            } else {
                setNodeColor(currentEdge.src, MSTColors::REJECTED);
                setNodeColor(currentEdge.dest, MSTColors::REJECTED);
                addAlgorithmStep("Skipped edge " + currentEdge.description + " (would create cycle)");
            }

            hasCurrentEdge = false;
            currentStepPhase = StepPhase::SORTING;
        }
    }

    void handleFinalizingPhase() {
        for (const auto& edge : mstEdges) {
            setNodeColor(edge.src, MSTColors::IN_MST);
            setNodeColor(edge.dest, MSTColors::IN_MST);
            setPulseEffect(edge.src, 0.0f);
            setPulseEffect(edge.dest, 0.0f);
        }

        summary.str("");
        summary << "MST Path (" << mstEdges.size() << " edges):\\n";
        for (const auto& edge : mstEdges) {
            summary << formatEdgeInfo(edge) << "\\n";
        }
        summary << "Total weight: " << formatWeight(totalWeight);

        addAlgorithmStep(summary.str());
        currentStepPhase = StepPhase::COMPLETE;
    }

    std::string formatEdgeInfo(const WeightedEdge& edge) {
        return "Edge " + std::to_string(edge.src) + " -> " +
               std::to_string(edge.dest) + " (weight: " +
               formatWeight(edge.weight) + ")";
    }

    void updateNodeColors(const WeightedEdge& edge, bool highlight) {
        if (highlight) {
            setNodeColor(edge.src, MSTColors::CONSIDERING);
            setNodeColor(edge.dest, MSTColors::CONSIDERING);
            setNodeHighlight(edge.src, true);
            setNodeHighlight(edge.dest, true);
        } else {
            bool srcInMST = isNodeInMST(edge.src);
            bool destInMST = isNodeInMST(edge.dest);

            setNodeColor(edge.src, srcInMST ? MSTColors::IN_MST : MSTColors::UNVISITED);
            setNodeColor(edge.dest, destInMST ? MSTColors::IN_MST : MSTColors::UNVISITED);
                        setNodeHighlight(edge.src, false);
                        setNodeHighlight(edge.dest, false);

                        if (srcInMST || destInMST) {
                            setPulseEffect(edge.src, 1.0f);
                            setPulseEffect(edge.dest, 1.0f);
                        }
                    }
    }

    bool isNodeInMST(int nodeId) {
        return std::any_of(mstEdges.begin(), mstEdges.end(),
            [nodeId](const WeightedEdge& e) {
                return e.src == nodeId || e.dest == nodeId;
            });
    }

    void resetForNextIteration() {
        for (size_t i = 0; i < parent.size(); i++) {
            makeSet(i);
            setNodeColor(i, MSTColors::UNVISITED);
            setNodeHighlight(i, false);
            setPulseEffect(i, 0.0f);
        }

        mstEdges.clear();
        while (!edgeQueue.empty()) edgeQueue.pop();
        for (const auto& edge : allEdges) {
            edgeQueue.push(edge);
        }

        hasCurrentEdge = false;
        totalWeight = 0;
        summary.str("");
        summary.clear();
        currentStepPhase = StepPhase::INIT;
        initialized = false;
        finished = false;

        addAlgorithmStep("Starting new iteration");
    }
};