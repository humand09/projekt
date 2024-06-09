#include "Player.h"
#include <cmath>

Player::Player(const sf::Color& color, const std::string& name)
    : mPosition(0), mTargetPosition(0, 0), mMoving(false), mName(name) {
    mShape.setRadius(10); // Ustawienie promienia kszta�tu gracza
    mShape.setFillColor(color); // Ustawienie koloru kszta�tu gracza
    mShape.setPosition(10, 10); // Ustawienie pocz�tkowej pozycji gracza
}

void Player::draw(sf::RenderWindow& window, const sf::Vector2f& offset) const {
    sf::CircleShape shape = mShape; // Skopiowanie kszta�tu gracza
    shape.move(offset); // Przesuni�cie kszta�tu o offset
    window.draw(shape); // Rysowanie kszta�tu gracza w oknie
}

void Player::moveToPosition(int position) {
    mPosition = position; // Aktualizacja pozycji gracza
    if (mPosition >= 100) {
        mPosition = 99; // Ograniczenie pozycji do rozmiaru planszy
    }
    int row = mPosition / 10; // Obliczenie wiersza na planszy
    int col = mPosition % 10; // Obliczenie kolumny na planszy
    if (row % 2 == 0) {
        mTargetPosition = sf::Vector2f(col * 60 + 10, (9 - row) * 60 + 10); // Ustawienie celu dla parzystych wierszy
    }
    else {
        mTargetPosition = sf::Vector2f((9 - col) * 60 + 10, (9 - row) * 60 + 10); // Ustawienie celu dla nieparzystych wierszy
    }
    mMoving = true; // Ustawienie flagi ruchu na prawda
}

void Player::update(float deltaTime) {
    if (mMoving) {
        sf::Vector2f currentPosition = mShape.getPosition(); // Pobranie aktualnej pozycji gracza
        sf::Vector2f direction = mTargetPosition - currentPosition; // Obliczenie kierunku ruchu
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y); // Obliczenie odleg�o�ci do celu

        if (distance < 5.0f) {
            mShape.setPosition(mTargetPosition); // Ustawienie pozycji gracza na celu
            mMoving = false; // Zatrzymanie ruchu
        }
        else {
            direction /= distance; // Normalizacja kierunku
            mShape.move(direction * 100.0f * deltaTime); // Ruch gracza w kierunku celu
        }
    }
}

int Player::getPosition() const {
    return mPosition; // Zwr�cenie pozycji gracza
}

std::string Player::getName() const {
    return mName; // Zwr�cenie nazwy gracza
}

void Player::resetPosition() {
    mPosition = 0; // Reset pozycji gracza do pocz�tkowej
    mShape.setPosition(34, 540); // Ustawienie pocz�tkowej pozycji kszta�tu gracza
    mTargetPosition = sf::Vector2f(33, 540); // Ustawienie pocz�tkowego celu gracza
}
