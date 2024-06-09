#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <string>

class Player {
public:
    Player(const sf::Color& color, const std::string& name);
    void draw(sf::RenderWindow& window, const sf::Vector2f& offset) const;
    void moveToPosition(int position);
    void update(float deltaTime);
    int getPosition() const;
    std::string getName() const;
    void resetPosition();

private:
    sf::CircleShape mShape;
    sf::Vector2f mTargetPosition;
    bool mMoving;
    int mPosition;
    std::string mName;
};

#endif
