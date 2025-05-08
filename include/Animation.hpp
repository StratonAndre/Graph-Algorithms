#pragma once
#include <functional>
#include <cmath>

class Animation {
public:
    Animation(float duration, std::function<void(float)> updateFunc);
    void update(float deltaTime);
    bool isFinished() const;

private:
    float duration;
    float currentTime;
    bool isComplete;
    std::function<void(float)> updateFunc;
};