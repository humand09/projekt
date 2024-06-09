#ifndef BONUS_H
#define BONUS_H

#include "Square.h"
#include <SFML/Audio.hpp>

class Bonus : public Square {
public:
    Bonus(int position, sf::Sound& sound, sf::Texture& texture);
    void triggerEvent(Player& player, int& diceResult, bool& extraRoll) override;
    void draw(sf::RenderWindow& window) const override;

private:
    int mPosition;
    sf::Sound& bonusSound;
    sf::Texture& bonusTexture;
};

#endif
