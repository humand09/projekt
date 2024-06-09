#ifndef SNAKE_H
#define SNAKE_H

#include "Square.h"
#include "utilities.h"
#include <SFML/Audio.hpp>

// Klasa Snake
class Snake : public Square {
public:
    // Konstruktor
    Snake(int start, int end, sf::Sound& sound);

    // Metody obslugi zdarzen i rysowania
    void triggerEvent(Player& player, int& diceResult, bool& extraRoll) override;
    void draw(sf::RenderWindow& window) const override;

private:
    // Atrybuty weza
    int mStart;
    int mEnd;
    std::string mTextureFile;
    sf::Sound& snakeHissSound;

    // Metoda pomocnicza do uzyskania pozycji na planszy
    sf::Vector2f getPosition(int squareIndex) const;
};

#endif
