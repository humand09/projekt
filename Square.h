#ifndef SQUARE_H
#define SQUARE_H

#include <SFML/Graphics.hpp>
#include <string>
#include "Player.h"

class Square {
public:
    virtual void triggerEvent(Player& player, int& diceResult, bool& extraRoll) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual ~Square() {}
};

#endif
