#pragma once
#include <cmath>

struct Coordinates {
    double latitude;
    double longitude;
    
    Coordinates(double lat = 0, double lon = 0) : latitude(lat), longitude(lon) {}
    

    sf::Vector2f toScreenPosition(sf::Vector2u windowSize, Coordinates minBounds, Coordinates maxBounds) const {
        double x = (longitude - minBounds.longitude) / (maxBounds.longitude - minBounds.longitude);
        double y = (latitude - minBounds.latitude) / (maxBounds.latitude - minBounds.latitude);


        const float PADDING = 50.f;
        float screenX = PADDING + x * (windowSize.x - 2 * PADDING);
        float screenY = PADDING + (1 - y) * (windowSize.y - 2 * PADDING);
        return sf::Vector2f(screenX, screenY);
    }


    double distanceTo(const Coordinates& other) const {

        double dx = longitude - other.longitude;
        double dy = latitude - other.latitude;
        return std::sqrt(dx * dx + dy * dy);
    }
};