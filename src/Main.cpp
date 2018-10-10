#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <thread>
#include <mutex>

std::mutex mu;

bool isDifferent(sf::Color a, sf::Color b) {
    constexpr uint8_t diff = 30;
    return 
        std::abs(a.r - b.r) > diff ||
        std::abs(a.g - b.g) > diff ||    
        std::abs(a.b - b.b) > diff;
}

void linearCompress(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height) {
    sf::Color activeColour;
    bool isNewColourNeeded = true;
    int newChnages = 0;

    for (unsigned y = 0; y < height - 1; y++) {
        for (unsigned x = 0; x < width - 1; x++) {
            if (isNewColourNeeded) {
                activeColour = originalImage.getPixel(x, y);
                mu.lock();
                newImage.setPixel(x, y, activeColour);
                mu.unlock();
                isNewColourNeeded = false;
            }

            if (!isDifferent(activeColour, originalImage.getPixel(x + 1, y))) {
                mu.lock();
                newImage.setPixel(x + 1, y, activeColour);
                mu.unlock();
                newChnages++;
            }
            else {
                isNewColourNeeded = true;
            }
            //std::cout << "poo\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void floodFillCompress(const sf::Image& originalImage, 
                sf::Image& newImage, 
                sf::Color fillColour, 
                unsigned x, unsigned y, 
                unsigned width, unsigned height,
                std::vector<bool>& visitedpxls) {
    size_t index = y * width + x;
    if (visitedpxls[index]) return;
    if (isDifferent(fillColour, originalImage.getPixel(x, y))) return;
    mu.lock();
    newImage.setPixel(x, y, fillColour);
    mu.unlock();
    visitedpxls[index] = true; 
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));

    if (x == width - 1) return;
    floodFillCompress(originalImage, newImage, fillColour, x + 1, y, width, height, visitedpxls);
    if (x == 0) return;
    floodFillCompress(originalImage, newImage, fillColour, x - 1, y, width, height, visitedpxls);
    if (y == height - 1) return;
    floodFillCompress(originalImage, newImage, fillColour, x, y + 1, width, height, visitedpxls);
    if (y == 0) return;
    floodFillCompress(originalImage, newImage, fillColour, x, y - 1, width, height, visitedpxls);
}

void floodCompress(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height) {
    sf::Color activeColour;
    std::vector<bool> visitedpxls(width * height);
    std::fill(visitedpxls.begin(), visitedpxls.end(), false);

    for (unsigned y = 0; y < height - 1; y++) {
        for (unsigned x = 0; x < width - 1; x++) {
            if (!visitedpxls[y * width + x]) {
                activeColour = originalImage.getPixel(x, y);
                floodFillCompress(originalImage, newImage, activeColour, x, y, width, height, visitedpxls);
            }
        }
    }
}

void visualise(const sf::Image& originalImage, const sf::Image& newImage) {
    mu.lock();
    sf::RenderWindow window({originalImage.getSize().x, originalImage.getSize().y}, "Paint");
    window.setFramerateLimit(60);
    sf::Texture textureA;
    sf::Texture textureB;

    textureA.loadFromImage(originalImage);
    textureB.loadFromImage(newImage);
    float w = originalImage.getSize().x;
    float y = originalImage.getSize().y;

    sf::RectangleShape shapeA({w, y});
    sf::RectangleShape shapeB({w, y});

    shapeA.setTexture(&textureA);
    shapeB.setTexture(&textureB);
    mu.unlock();
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();
        mu.unlock();
        textureB.loadFromImage(newImage);
        mu.lock();
        window.draw(shapeA);
        window.draw(shapeB);



        window.display();
    }

}

int main(int argc, char** argv) {
    std::string imgName;
    if (argc > 1) {
        imgName = argv[1];
    }
    else {
        std::cout << "Please input an image.\n";
        std::cout << "balic <image_name>\n";
        return -1;
    }

    sf::Image originalImage;
    if (!originalImage.loadFromFile(imgName)) {
        return -1;
    }

    unsigned width = originalImage.getSize().x;
    unsigned height = originalImage.getSize().y;

    sf::Image newImage;
    newImage.create(width, height, sf::Color::Transparent);

    std::thread thread ([&]() {
        visualise(originalImage, newImage);
    });
    std::cout << "p\n";

    floodCompress(originalImage, newImage, width, height);
    //linearCompress(originalImage, newImage, width, height);
    newImage.saveToFile("out.jpg"); 

    thread.join();
}