#include "Compressers.h"

#include <unordered_set>
#include <queue>
#include <mutex>
#include <atomic>
#include <iostream>

#include "FloodFill.h"

struct FloodSection {
    unsigned startX, startY;
    sf::Color color;
};

void floodCompressConcurrent(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height, std::mutex& imgMutex) {
    std::vector<bool> visitedpxls(width * height);
    std::fill(visitedpxls.begin(), visitedpxls.end(), false);
    std::queue<FloodSection> sectionQueue;
    std::atomic<bool> complete = false;
    std::vector<std::thread> threads;
    std::mutex queueAccess;
    for (int i = 0; i < 4; i++) {
        threads.emplace_back([&]() {
            FloodSection sect;
            while (!complete) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                if (!sectionQueue.empty()) {  
                    std::lock_guard<std::mutex> mu(queueAccess);
                    sect = sectionQueue.back();
                    sectionQueue.pop();
                }
                floodFill<true>(originalImage, newImage, sect.color, sect.startX, sect.startY, width, height, visitedpxls, imgMutex);
            }
        });
    }
    for (unsigned y = 0; y < height - 1; y++) {
        for (unsigned x = 0; x < width - 1; x++) {
            if (!visitedpxls[y * width + x]) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                std::lock_guard<std::mutex> mu(queueAccess);
                sectionQueue.push({x, y, originalImage.getPixel(x, y)});
            }
        }
    }
    complete = true;
    for (auto& thread : threads) {
        thread.join();
    }
}