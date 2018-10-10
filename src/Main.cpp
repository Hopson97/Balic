#include <iostream>
#include <SFML/Graphics.hpp>


void linearCompress(const sf::Image& originalImage, sf::Image& newImage) {

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

    linearCompress(originalImage, newImage);
}