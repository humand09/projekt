#include "utilities.h"

sf::Vector2f adjustPosition(const sf::Vector2f& originalPosition, const sf::Vector2f& target, float offset) {
    sf::Vector2f direction = target - originalPosition; // Obliczenie kierunku ruchu
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y); // Obliczenie d³ugoœci wektora
    direction /= length; // Normalizacja wektora
    return originalPosition + direction * offset; // Zwrócenie przesuniêtej pozycji
}

void drawRectangleWithTextureAndRotation(sf::RenderWindow& window, const sf::Vector2f& topLeft, const sf::Vector2f& bottomRight, const std::string& textureFile) {
    sf::Texture texture;
    if (!texture.loadFromFile(textureFile)) {
        return; // Informacja o b³êdzie ³adowania tekstury
    }

    float length = std::sqrt(std::pow(bottomRight.x - topLeft.x, 2) + std::pow(bottomRight.y - topLeft.y, 2)); // Obliczenie d³ugoœci prostok¹ta
    float width = 50; // Szerokoœæ prostok¹ta

    sf::RectangleShape rectangle(sf::Vector2f(length, width)); // Utworzenie prostok¹ta
    rectangle.setPosition(topLeft.x + 10, topLeft.y + 10); // Ustawienie pozycji prostok¹ta
    rectangle.setTexture(&texture); // Ustawienie tekstury prostok¹ta

    float dx = bottomRight.x - topLeft.x; // Ró¿nica w osi x
    float dy = bottomRight.y - topLeft.y; // Ró¿nica w osi y
    float angle = std::atan2(dy, dx) * 180 / 3.14159f; // Obliczenie k¹ta obrotu

    rectangle.setOrigin(0, width / 2); // Ustawienie punktu pocz¹tkowego prostok¹ta
    rectangle.setRotation(angle); // Ustawienie k¹ta obrotu prostok¹ta

    window.draw(rectangle); // Rysowanie prostok¹ta w oknie
}
