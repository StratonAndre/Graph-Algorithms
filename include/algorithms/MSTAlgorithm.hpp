#pragma once
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <iomanip>
#include "MSTVisualization.hpp"
#include "Node.hpp"

class MSTAlgorithm : public MSTVisualization {
protected:
    std::vector<WeightedEdge> edges;
    std::vector<WeightedEdge> mstEdges;
    std::vector<int> parent;
    std::vector<int> rank;
    bool finished = false;

    void makeSet(int v) {
        parent[v] = v;
        rank[v] = 0;
        setNodeColor(v, MSTColors::UNVISITED);
        setComponentLabel(v, "Component " + std::to_string(v));
    }

    int findSet(int v) {
        if (v != parent[v])
            parent[v] = findSet(parent[v]);
        return parent[v];
    }

    void unionSets(int a, int b) {
        a = findSet(a);
        b = findSet(b);
        if (a != b) {
            if (rank[a] < rank[b])
                std::swap(a, b);
            parent[b] = a;
            if (rank[a] == rank[b])
                rank[a]++;


            for (size_t i = 0; i < parent.size(); i++) {
                if (findSet(i) == a) {
                    setComponentLabel(i, "Component " + std::to_string(a));
                }
            }
        }
    }

public:
    virtual ~MSTAlgorithm() = default;

    virtual void execute(std::vector<std::unique_ptr<Node>>& nodes) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool step() = 0;

    virtual void reset() {
        edges.clear();
        mstEdges.clear();
        parent.clear();
        rank.clear();
        nodeStates.clear();
        algorithmSteps.clear();
        currentStep = 0;
        finished = false;
    }

    bool isFinished() const { return finished; }

    void addEdge(int src, int dest, float weight) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1);
        ss << src << " -> " << dest << " (weight: " << weight << ")";
        edges.push_back({src, dest, weight, ss.str()});
    }

    const std::vector<WeightedEdge>& getMSTEdges() const {
        return mstEdges;
    }
};