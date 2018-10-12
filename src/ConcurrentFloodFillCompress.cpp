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

struct ConcurrentFloodUtilities {
    ConcurrentFloodUtilities(const sf::Image& originalImage, std::mutex& queueAccessMutex) 
    :   originalImage       (originalImage)
    ,   visitedPixels       (originalImage.getSize().x * originalImage.getSize().y)
    ,   queueAccessMutex    (queueAccessMutex)
    { 
        std::fill(visitedPixels.begin(), visitedPixels.end(), false);
    }

    const sf::Image&            originalImage;
    std::vector<bool>           visitedPixels;
    std::queue<FloodSection>    sectionQueue;
    std::mutex&                 queueAccessMutex;
};

void addQueueThread(std::vector<std::thread>& queueUpdaterThreads, ConcurrentFloodUtilities& util, 
    unsigned xBegin, unsigned yBegin, unsigned xEnd, unsigned yEnd) 
{
    queueUpdaterThreads.emplace_back([=, &util]() {
        auto width = util.originalImage.getSize().x;
        for (unsigned y = yBegin ; y < yEnd; y++) {
            for (unsigned x = xBegin; x < xEnd; x++) {
                if (!util.visitedPixels[y * width + x]) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    std::lock_guard<std::mutex> mu(util.queueAccessMutex);
                    util.sectionQueue.push({x, y, util.originalImage.getPixel(x, y)});
                }
            }
        }
    });
}

void floodCompressConcurrent(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height, std::mutex& imgMutex) {
    std::mutex queueAccess;
    ConcurrentFloodUtilities util(originalImage, queueAccess);
    std::vector<std::thread> threads;
    std::vector<std::thread> queueUpdaterThreads;

    std::atomic<bool> complete = false;
    
    //Init threads
    for (int i = 0; i < 4; i++) {
        threads.emplace_back([&]() {
            FloodSection sect;
            while (!complete) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                if (!util.sectionQueue.empty()) {  
                    std::lock_guard<std::mutex> mu(queueAccess);
                    sect = util.sectionQueue.back();
                    util.sectionQueue.pop();
                }
                floodFill<true>(
                    originalImage, newImage, sect.color, 
                    sect.startX,  sect.startY, width, height, 
                    util.visitedPixels, imgMutex);
            }
        });
    }

    auto w = originalImage.getSize().x;
    auto h = originalImage.getSize().y;
    addQueueThread(queueUpdaterThreads, util, 0,        0,      w / 2,  h / 2);
    addQueueThread(queueUpdaterThreads, util, w / 2,    h / 2,  w,      h);
    addQueueThread(queueUpdaterThreads, util, w / 2,    0,      w,      h / 2);
    addQueueThread(queueUpdaterThreads, util, 0,        h / 2,  w / 2,  h);

    for (auto& thread : queueUpdaterThreads) {
        thread.join();
    }
    std::cout << "DONE\n";
    complete = true;

    for (auto& thread : threads) {
        thread.join();
    }
}