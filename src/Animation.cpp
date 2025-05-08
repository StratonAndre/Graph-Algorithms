#include "Animation.hpp"

Animation::Animation(float duration, std::function<void(float)> updateFunc)
    : duration(duration), currentTime(0), isComplete(false), updateFunc(updateFunc) {
}

void Animation::update(float deltaTime) {
    if (isComplete) return;

    currentTime += deltaTime;
    float progress = currentTime / duration;

    if (progress >= 1.0f) {
        progress = 1.0f;
        isComplete = true;
    }

    updateFunc(progress);
}

bool Animation::isFinished() const {
    return isComplete;
}