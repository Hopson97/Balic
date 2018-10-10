#include <iostream>
#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window({1280, 720}, "SFML", sf::Style::Default);
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            switch(e.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                default:
                    break;
            }
        }
        window.clear();

        window.display();
    }
}