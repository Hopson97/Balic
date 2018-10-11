#pragma once

#include <mutex>
#include <thread>
#include <SFML/Graphics.hpp>

void floodCompress(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height, std::mutex& imgMutex);
void floodCompressConcurrent(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height, std::mutex& imgMutex);
void linearCompress(const sf::Image& originalImage, sf::Image& newImage, unsigned width, unsigned height, std::mutex& imgMutex);