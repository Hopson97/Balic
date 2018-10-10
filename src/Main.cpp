#include <iostream>
#include <SFML/Graphics.hpp>

bool isDifferent(sf::Color a, sf::Color b) {
    return 
        std::abs(a.r - b.r) > 50 ||
        std::abs(a.g - b.g) > 50 ||    
        std::abs(a.b - b.b) > 50;
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

    linearCompress(originalImage, newImage, width, height);

    newImage.saveToFile("out.jpg");
}