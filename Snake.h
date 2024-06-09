#ifndef SNAKE_H
#define SNAKE_H

#include "Square.h"
#include "utilities.h"
#include <SFML/Audio.hpp>

class Snake : public Square {
public:
    Snake(int start, int end, sf::Sound& sound);
    void triggerEvent(Player& player, int& diceResult, bool& extraRoll) override;
    void draw(sf::RenderWindow& window) const override;

private:
    int mStart;
    int mEnd;
    std::string mTextureFile;
    sf::Sound& snakeHissSound;
    sf::Vector2f getPosition(int squareIndex) const;
};

#endif
