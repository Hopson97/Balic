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
    unsigned xBegin, unsigned yBegin, unsigned xEnd, unsigned yEnd,
    bool goRight, bool goDown) 
{
    queueUpdaterThreads.emplace_back([=, &util]() {
        auto width = util.originalImage.getSize().x;
        for (unsigned y = yBegin; goDown ? (y < yEnd) : (y >= yEnd); goDown ? (y++) : (y--)) {
            for (unsigned x = xBegin; goRight ? (x < xEnd) : (x >= xEnd); goRight? (x++) : (x--)) {
                if (!util.visitedPixels[y * width + x]) {
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                    std::lock_guard<std::mutex> mu(util.queueAccessMutex);
                    util.sectionQueue.push({x, y, util.originalImage.getPixel(x, y)});
                }
            }
        }
    });
}

void joinThreadPool(std::vector<std::thread>& threads) {
    for (auto& thread : threads) {
        thread.join();
    }
}

void floodCompressConcurrent(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height, std::mutex& imgMutex) {
    std::mutex queueAccess;
    ConcurrentFloodUtilities util(originalImage, queueAccess);
    std::vector<std::thread> workers;
    std::vector<std::thread> queueUpdaterThreads;

    std::atomic<bool> complete = false;
    sf::Clock clock;
    //Init threads
    for (int i = 0; i < 64; i++) {
        workers.emplace_back([&]() {
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
    auto h = originalImage.getSize().y;//     X-BEGIN   Y-BEGIN     X-END       Y-END
    addQueueThread(queueUpdaterThreads, util, 0,        0,          w / 2,      h / 2,  true,   true);
    addQueueThread(queueUpdaterThreads, util, w - 1,    h - 1,      w / 2,      h / 2,  false,  false);
    addQueueThread(queueUpdaterThreads, util, 0,        h - 1,      w / 2,      h / 2,  true,   false);
    addQueueThread(queueUpdaterThreads, util, w,        0,          w / 2,      h / 2,  false,  true);

    joinThreadPool(queueUpdaterThreads);
    std::cout << "\nDone. Time taken: " << clock.getElapsedTime().asSeconds() << "secs.\n\n\n";
    complete = true;
    joinThreadPool(workers);
}