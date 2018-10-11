#pragma once

#include <SFML/Graphics.hpp>

//"compresses" using flood fill
template <bool Pause>
void floodFill(const sf::Image& originalImage, 
                sf::Image& newImage, 
                sf::Color fillColour, 
                unsigned x, unsigned y, 
                unsigned width, unsigned height,
                std::vector<bool>& visitedpxls) {
    size_t index = y * width + x;
    if (visitedpxls[index]) return;
    if (isDifferent(fillColour, originalImage.getPixel(x, y))) return;
    {
        std::lock_guard<std::mutex> lock(mutex);
        newImage.setPixel(x, y, fillColour);
        visitedpxls[index] = true; 
    }
    if constexpr (Pause) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    if (x == width - 1) return;
    floodFillCompress<Pause>(originalImage, newImage, fillColour, x + 1, y, width, height, visitedpxls);
    if (x == 0) return;
    floodFillCompress<Pause>(originalImage, newImage, fillColour, x - 1, y, width, height, visitedpxls);
    if (y == height - 1) return;
    floodFillCompress<Pause>(originalImage, newImage, fillColour, x, y + 1, width, height, visitedpxls);
    if (y == 0) return;
    floodFillCompress<Pause>(originalImage, newImage, fillColour, x, y - 1, width, height, visitedpxls);
}