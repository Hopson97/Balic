#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <unordered_set>

namespace {
    //Mutex for locking access to the image while it is being modified.
    std::mutex mutex;

    //Return true if 2 colours have a big difference
    bool isDifferent(sf::Color a, sf::Color b) {
        uint8_t difference = 50;
        return 
            std::abs(a.r - b.r) > difference ||
            std::abs(a.g - b.g) > difference ||    
            std::abs(a.b - b.b) > difference;
    }

    //"Compresses" in lines
    void linearCompress(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height) {
        sf::Color activeColour;
        bool isNewColourNeeded = true;

        for (unsigned y = 0; y < height - 1; y++) {
            for (unsigned x = 0; x < width - 1; x++) {
                if (isNewColourNeeded) {
                    activeColour = originalImage.getPixel(x, y);
                    std::lock_guard<std::mutex> lock(mutex);
                    newImage.setPixel(x, y, activeColour);
                    isNewColourNeeded = false;
                }

                if (!isDifferent(activeColour, originalImage.getPixel(x + 1, y))) {
                    std::lock_guard<std::mutex> lock(mutex);
                    newImage.setPixel(x + 1, y, activeColour);
                }
                else {
                    isNewColourNeeded = true;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    //"compresses" using flood fill
    template <bool Pause>
    void floodFillCompress(const sf::Image& originalImage, 
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

    void floodCompressConcurrent(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height) {
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
                        floodFillCompress<true>(originalImage, newImage, sect.color, sect.startX, sect.startY, width, height, visitedpxls);
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

    void floodCompress(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height) {
        std::vector<bool> visitedpxls(width * height);
        std::fill(visitedpxls.begin(), visitedpxls.end(), false);

        for (unsigned y = 0; y < height - 1; y++) {
            for (unsigned x = 0; x < width - 1; x++) {
                if (!visitedpxls[y * width + x]) {
                    floodFillCompress<false>(originalImage, newImage, originalImage.getPixel(x, y), x, y, width, height, visitedpxls);
                    std::this_thread::sleep_for(std::chrono::milliseconds(3));
                }
            }
        }
    }

    //Allows visualisation of the image manipulation process
    void visualise(const sf::Image& originalImage, const sf::Image& newImage) {
        mutex.lock();
        sf::RenderWindow window({originalImage.getSize().x * 2, originalImage.getSize().y * 2}, "Paint");
        window.setFramerateLimit(60);
        sf::Texture textureA;
        sf::Texture textureB;

        textureA.loadFromImage(originalImage);
        textureB.loadFromImage(newImage);
        float w = window.getSize().x;
        float y = window.getSize().y;

        sf::RectangleShape shapeA({w, y});
        sf::RectangleShape shapeB({w, y});

        shapeA.setTexture(&textureA);
        shapeB.setTexture(&textureB);
        mutex.unlock();
        while (window.isOpen()) {
            sf::Event e;
            while (window.pollEvent(e)) {
                if (e.type == sf::Event::Closed) {
                    window.close();
                }
            }
            window.clear();
            {
                std::lock_guard<std::mutex> lock(mutex);
                textureB.loadFromImage(newImage);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
                window.draw(shapeA);
            }
            window.draw(shapeB);

            window.display();
        }
    }
}

int main(int argc, char** argv) {
    std::string imgName;
    std::string outputName;
    if (argc > 2) {
        imgName = argv[1];
        outputName = argv[2];
    }
    else {
        std::cout << "Please input an image.\n";
        std::cout << "balic <image_name> <output>\n";
        std::cout << "EG: balic yosemite.jpg new_yostemite.jpg\n";
        return -1;
    }

    std::cout << "Press V to view original image\n";

    sf::Image originalImage;
    if (!originalImage.loadFromFile(imgName)) {
        return -1;
    }

    unsigned width = originalImage.getSize().x;
    unsigned height = originalImage.getSize().y;

    sf::Image newImage;
    newImage.create(width, height, sf::Color::Transparent);
    std::thread thread ([&]() {
        //linearCompress(originalImage, newImage, width, height);
        floodCompress(originalImage, newImage, width, height);
        std::cout << "Finished!";
        std::cout << "Saving image...\n";
        newImage.saveToFile(outputName); 
        std::cout << "Image saved, program complete. Please close window to exit program\n";
    });

    visualise(originalImage, newImage);
    thread.join();
}