#include "Compressers.h"

#include <unordered_set>
#include <queue>
#include <mutex>
#include <atomic>

#include "FloodFill.h"

struct FloodSection {
    unsigned startX, startY;
    sf::Color color;
};
struct IsColourEqual {
    bool operator()(const sf::Color& colourA, const sf::Color& colourB) const {
        return  colourA.r == colourB.r &&
                colourA.g == colourB.g &&
                colourA.b == colourB.b;
    }
};
struct HashColor {
public:
    size_t operator()(const sf::Color& col) const {
        return (std::hash<int>()(col.r) | std::hash<int>()(col.g) ^ std::hash<uint8_t>()(col.b));
    }
};

void floodCompressConcurrent(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height, std::mutex& imgMutex) {
    std::vector<bool> visitedpxls(width * height);
    std::fill(visitedpxls.begin(), visitedpxls.end(), false);
    std::unordered_set<sf::Color, HashColor, IsColourEqual> cols;
    std::queue<FloodSection> sectionQueue;
    std::atomic<bool> complete = false;
    std::vector<std::thread> threads;
    std::mutex queueAccess;
    for (int i = 0; i < 4; i++) {
        threads.emplace_back([&]() {
            FloodSection sect;
            while (!complete) {
                //std::cout << "doing\n";
                if (!sectionQueue.empty()) {
                    {
                        //std::cout << "Doing lol\n";
                        std::lock_guard<std::mutex> mu(queueAccess);
                        sect = sectionQueue.back();
                        sectionQueue.pop();
                    }
                    floodFill<true>(originalImage, newImage, sect.color, sect.startX, sect.startY, width, height, visitedpxls, imgMutex);
                }
            }
        });
    }
    for (unsigned y = 0; y < height - 1; y++) {
        for (unsigned x = 0; x < width - 1; x++) {
            if (!visitedpxls[y * width + x]) {
                auto c = originalImage.getPixel(x, y);
                if (cols.find(c) == cols.end()) {
                    std::lock_guard<std::mutex> mu(queueAccess);
                    sectionQueue.push({x, y, originalImage.getPixel(x, y)});
                    cols.emplace(c);
                }
                //floodFillCompress(originalImage, newImage, c, x, y, width, height, visitedpxls);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
    complete = true;
    for (auto& thread : threads) {
        thread.join();
    }
}