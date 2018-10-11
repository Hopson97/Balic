#include "Compressers.h"

#include "Util.h"
#include <mutex>

//"Compresses" in lines
void linearCompress(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height, std::mutex& imgMutex) {
    sf::Color activeColour;
    bool isNewColourNeeded = true;
    for (unsigned y = 0; y < height - 1; y++) {
        for (unsigned x = 0; x < width - 1; x++) {
            if (isNewColourNeeded) {
                activeColour = originalImage.getPixel(x, y);
                std::lock_guard<std::mutex> lock(imgMutex);
                newImage.setPixel(x, y, activeColour);
                isNewColourNeeded = false;
            }
            if (!isDifferent(activeColour, originalImage.getPixel(x + 1, y))) {
                std::lock_guard<std::mutex> lock(imgMutex);
                newImage.setPixel(x + 1, y, activeColour);
            }
            else {
                isNewColourNeeded = true;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}