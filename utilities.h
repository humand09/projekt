#ifndef UTILITIES_H
#define UTILITIES_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>

sf::Vector2f adjustPosition(const sf::Vector2f& originalPosition, const sf::Vector2f& target, float offset);
void drawRectangleWithTextureAndRotation(sf::RenderWindow& window, const sf::Vector2f& topLeft, const sf::Vector2f& bottomRight, const std::string& textureFile);

#endif
