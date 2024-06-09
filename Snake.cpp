#include "Snake.h"
#include <iostream> // Dodane, aby uwzglêdniæ std::cout

Snake::Snake(int start, int end, sf::Sound& sound) : mStart(start), mEnd(end), snakeHissSound(sound) {
    int textureIndex = rand() % 3; // Losowanie indeksu tekstury
    mTextureFile = "assets/snake" + std::to_string(textureIndex + 1) + ".png"; // Ustawienie pliku tekstury
}

void Snake::triggerEvent(Player& player, int& diceResult, bool& extraRoll) {
    player.moveToPosition(mEnd); // Przesuniêcie gracza na koniec wê¿a
    snakeHissSound.play(); // Odtworzenie dŸwiêku wê¿a
    std::cout << player.getName() << " landed on a snake! Sliding down to " << mEnd << ".\n"; // Informacja o zdarzeniu
}

void Snake::draw(sf::RenderWindow& window) const {
    sf::Vector2f topLeft = adjustPosition(getPosition(mStart), sf::Vector2f(290, 290), 6); // Obliczenie pozycji pocz¹tkowej wê¿a
    sf::Vector2f bottomRight = adjustPosition(getPosition(mEnd), sf::Vector2f(290, 290), 6); // Obliczenie pozycji koñcowej wê¿a
    drawRectangleWithTextureAndRotation(window, topLeft, bottomRight, mTextureFile); // Rysowanie wê¿a na planszy
}

sf::Vector2f Snake::getPosition(int squareIndex) const {
    int row = squareIndex / 10; // Obliczenie wiersza
    int col = squareIndex % 10; // Obliczenie kolumny
    if (row % 2 == 0) {
        return sf::Vector2f(col * 60, (9 - row) * 60); // Obliczenie pozycji dla parzystych wierszy
    }
    else {
        return sf::Vector2f((9 - col) * 60, (9 - row) * 60); // Obliczenie pozycji dla nieparzystych wierszy
    }
}
