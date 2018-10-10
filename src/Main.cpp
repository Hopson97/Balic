#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

bool isDifferent(sf::Color a, sf::Color b) {
    constexpr uint8_t diff = 60;
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
                newImage.setPixel(x, y, activeColour);
                isNewColourNeeded = false;
            }

            if (!isDifferent(activeColour, originalImage.getPixel(x + 1, y))) {
                newImage.setPixel(x + 1, y, activeColour);
                newChnages++;
            }
            else {
                isNewColourNeeded = true;
            }
            
        }
    }
    std::cout << "New compress poop: " << newChnages << "\n";
}

void floodFillCompress(const sf::Image& originalImage, 
                sf::Image& newImage, 
                sf::Color fillColour, 
                unsigned x, unsigned y, 
                unsigned width, unsigned height,
                std::vector<bool>& visitedpxls) {
    if (visitedpxls[y * width + x]) return;
    if (isDifferent(fillColour, originalImage.getPixel(x, y))) return;
    
    newImage.setPixel(x, y, fillColour);
    visitedpxls[y * width + x] = true; 

    if (x + 1 > width - 1) return;
    floodFillCompress(originalImage, newImage, fillColour, x + 1, y, width, height, visitedpxls);
    if (x == 0) return;
    floodFillCompress(originalImage, newImage, fillColour, x - 1, y, width, height, visitedpxls);
    if (y + 1 > height - 1) return;
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
    newImage.create(width, height);

    floodCompress(originalImage, newImage, width, height);

    newImage.saveToFile("out.jpg");
}