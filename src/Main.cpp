#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <unordered_set>

#include "Compressers.h"

namespace {
    //Mutex for locking access to the image while it is being modified.
    std::mutex mutex;

    //Allows visualisation of the image manipulation process
    void visualise(const sf::Image& originalImage, const sf::Image& newImage, std::mutex& imgMutex) {
        mutex.lock();
        sf::RenderWindow window({originalImage.getSize().x * 1.5, originalImage.getSize().y * 1.5}, "Paint");
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
                std::lock_guard<std::mutex> lock(imgMutex);
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
    if (argc > 1) {
        imgName = argv[1];
        if (argc > 2) {
            outputName = argv[2];
        } else {
            outputName = "balic_output.jpg";
        }
    }
    else {
        std::cout << "Please input an image.\n";
        std::cout << "balic <image_name> <output (optional)>\n";
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

    std::mutex mutex;
    sf::Image newImage;
    newImage.create(width, height, sf::Color::Transparent);
    std::thread thread ([&]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //linearCompress(originalImage, newImage, width, height, mutex);
        //floodCompress(originalImage, newImage, width, height, mutex);
        floodCompressConcurrent(originalImage, newImage, width, height, mutex);
        std::cout << "Finished!";
        std::cout << "Saving image...\n";
        newImage.saveToFile(outputName); 
        std::cout << "Image saved, program complete. Please close window to exit program\n";
    });

    visualise(originalImage, newImage, mutex);
    thread.join();
}