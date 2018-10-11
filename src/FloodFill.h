#pragma once

#include <SFML/Graphics.hpp>
#include <mutex>

#include "Util.h"

//"compresses" using flood fill
template <bool Pause>
void floodFill(const sf::Image& originalImage, 
                sf::Image& newImage, 
                sf::Color fillColour, 
                unsigned x, unsigned y, 
                unsigned width, unsigned height,
                std::vector<bool>& visitedpxls, std::mutex& imgMutex) {
    size_t index = y * width + x;
    if (visitedpxls[index]) {
        return;
    }
    if (isDifferent(fillColour, originalImage.getPixel(x, y))) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(imgMutex);
        newImage.setPixel(x, y, fillColour);
        visitedpxls[index] = true; 
    }

    if constexpr (Pause) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    if (x == width - 1) return;
    floodFill<Pause>(originalImage, newImage, fillColour, x + 1, y, width, height, visitedpxls, imgMutex);
    if (x == 0) return;
    floodFill<Pause>(originalImage, newImage, fillColour, x - 1, y, width, height, visitedpxls, imgMutex);
    if (y == height - 1) return;
    floodFill<Pause>(originalImage, newImage, fillColour, x, y + 1, width, height, visitedpxls, imgMutex);
    if (y == 0) return;
    floodFill<Pause>(originalImage, newImage, fillColour, x, y - 1, width, height, visitedpxls, imgMutex);
}