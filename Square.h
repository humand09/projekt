#ifndef SQUARE_H
#define SQUARE_H

#include <SFML/Graphics.hpp>
#include <string>
#include "Player.h"

// Abstrakcyjna klasa Square
class Square {
public:
    // Czyste wirtualne metody do obslugi zdarzen i rysowania
    virtual void triggerEvent(Player& player, int& diceResult, bool& extraRoll) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;

    // Wirtualny destruktor
    virtual ~Square() {}
};

#endif // SQUARE_H
