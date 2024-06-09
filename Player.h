#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <string>

// Klasa Player
class Player {
public:
    // Konstruktor
    Player(const sf::Color& color, const std::string& name);

    // Metody rysowania, ruchu i aktualizacji gracza
    void draw(sf::RenderWindow& window, const sf::Vector2f& offset) const;
    void moveToPosition(int position);
    void update(float deltaTime);
    int getPosition() const;
    std::string getName() const;
    void resetPosition();

private:
    // Atrybuty gracza
    sf::CircleShape mShape;
    sf::Vector2f mTargetPosition;
    bool mMoving;
    int mPosition;
    std::string mName;
};

#endif
