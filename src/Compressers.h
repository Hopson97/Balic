#pragma once

#include <SFML/Graphics.hpp>

void floodCompress(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height);
void floodCompressConcurrent(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height);
void linearCompress(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height);