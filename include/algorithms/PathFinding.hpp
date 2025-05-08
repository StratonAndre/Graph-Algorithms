#pragma once
#include "../Node.hpp"
#include <vector>
#include <memory>

class PathFindingAlgorithm {
public:
    virtual ~PathFindingAlgorithm() = default;
    virtual void reset() = 0;
    virtual bool step() = 0;
    virtual bool isFinished() const = 0;
};

class Dijkstra : public PathFindingAlgorithm {
public:
    void reset() override {}
    bool step() override { return true; }
    bool isFinished() const override { return true; }
};