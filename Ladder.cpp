#include "Ladder.h"
#include <iostream>

Ladder::Ladder(int start, int end, sf::Sound& sound) : mStart(start), mEnd(end), ladderKnockSound(sound) {
    int textureIndex = rand() % 3; // Losowanie indeksu tekstury
    mTextureFile = "assets/ladder" + std::to_string(textureIndex + 1) + ".png"; // Ustawienie pliku tekstury
}

void Ladder::triggerEvent(Player& player, int& diceResult, bool& extraRoll) {
    player.moveToPosition(mEnd); // Przesuni?cie gracza na koniec drabiny
    ladderKnockSound.play(); // Odtworzenie d?wi?ku drabiny
    std::cout << player.getName() << " climbed a ladder! Moving up to " << mEnd << ".\n"; // Informacja o zdarzeniu
}

void Ladder::draw(sf::RenderWindow& window) const {
    sf::Vector2f topLeft = adjustPosition(getPosition(mStart), sf::Vector2f(290, 290), 14); // Obliczenie pozycji pocz?tkowej drabiny
    sf::Vector2f bottomRight = adjustPosition(getPosition(mEnd), sf::Vector2f(290, 290), 14); // Obliczenie pozycji ko?cowej drabiny
    drawRectangleWithTextureAndRotation(window, topLeft, bottomRight, mTextureFile); // Rysowanie drabiny na planszy
}

sf::Vector2f Ladder::getPosition(int squareIndex) const {
    int row = squareIndex / 10; // Obliczenie wiersza
    int col = squareIndex % 10; // Obliczenie kolumny
    if (row % 2 == 0) {
        return sf::Vector2f(col * 60, (9 - row) * 60); // Obliczenie pozycji dla parzystych wierszy
    }
    else {
        return sf::Vector2f((9 - col) * 60, (9 - row) * 60); // Obliczenie pozycji dla nieparzystych wierszy
    }
}