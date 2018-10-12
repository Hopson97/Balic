#include "Util.h"

bool isDifferent(sf::Color a, sf::Color b) {
    uint8_t difference = 35;
    return 
        std::abs(a.r - b.r) > difference ||
        std::abs(a.g - b.g) > difference ||    
        std::abs(a.b - b.b) > difference;
}