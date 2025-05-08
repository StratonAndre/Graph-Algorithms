#pragma once
#include <vector>
#include <memory>
#include "Node.hpp"

class GraphAlgorithm {
public:
    virtual ~GraphAlgorithm() = default;


    virtual void execute(std::vector<std::unique_ptr<Node>>& nodes) = 0;


    virtual std::string getName() const = 0;

    virtual std::string getDescription() const = 0;


    virtual void reset() = 0;

    virtual bool isFinished() const = 0;

    virtual bool step() = 0;
};

class MinimumSpanningTree : public GraphAlgorithm {
public:
    std::string getName() const override { return "Minimum Spanning Tree"; }

    std::string getDescription() const override {
        return "Finds the minimum spanning tree using Kruskal's algorithm";
    }

    void execute(std::vector<std::unique_ptr<Node>>& nodes) override {
        nodes_ptr = &nodes;
        mst_edges.clear();
        visited.clear();
        initialized = false;
        finished = false;
        current_edge = 0;
    }

    void reset() override {
        if (nodes_ptr) {
            mst_edges.clear();
            visited.clear();
            initialized = false;
            finished = false;
            current_edge = 0;
        }
    }

    bool isFinished() const override {
        return finished;
    }

    bool step() override {
        if (!initialized) {
            initializeAlgorithm();
            return true;
        }
        if (finished) return false;

        return stepAlgorithm();
    }

private:
    bool finished = false;
    bool initialized = false;
    std::vector<std::unique_ptr<Node>>* nodes_ptr = nullptr;
    std::vector<std::pair<Node*, Node*>> mst_edges;
    std::vector<bool> visited;
    size_t current_edge = 0;

    void initializeAlgorithm() {
        if (!nodes_ptr || nodes_ptr->empty()) return;
        visited.resize(nodes_ptr->size(), false);
        initialized = true;
    }

    bool stepAlgorithm() {

        if (current_edge >= nodes_ptr->size() - 1) {
            finished = true;
            return false;
        }
        current_edge++;
        return true;
    }
};