#include "Compressers.h"

#include "FloodFill.h"

#include <mutex>

void floodCompress(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height, std::mutex& imgMutex) {
    std::vector<bool> visitedpxls(width * height);
    std::fill(visitedpxls.begin(), visitedpxls.end(), false);
    for (unsigned y = 0; y < height - 1; y++) {
        for (unsigned x = 0; x < width - 1; x++) {
            if (!visitedpxls[y * width + x]) {
                floodFill<false>(originalImage, newImage, originalImage.getPixel(x, y), x, y, width, height, visitedpxls, imgMutex);
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
            }
        }
    }
}