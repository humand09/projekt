#include "Bonus.h"
#include <iostream> // Dodane, aby uwzglêdniæ std::cout

Bonus::Bonus(int position, sf::Sound& sound, sf::Texture& texture) : mPosition(position), bonusSound(sound), bonusTexture(texture) {}

void Bonus::triggerEvent(Player& player, int& diceResult, bool& extraRoll) {
    bonusSound.play(); // Odtworzenie dŸwiêku bonusu
    std::cout << player.getName() << " landed on a bonus! Double roll allowed.\n"; // Informacja o zdarzeniu
    extraRoll = true; // Ustawienie flagi dodatkowego rzutu
}

void Bonus::draw(sf::RenderWindow& window) const {
    sf::Sprite bonusSprite(bonusTexture); // Utworzenie sprite'a bonusu
    bonusSprite.setScale(0.04f, 0.04f); // Skalowanie sprite'a
    int row = mPosition / 10; // Obliczenie wiersza
    int col = mPosition % 10; // Obliczenie kolumny
    if (row % 2 == 0) {
        bonusSprite.setPosition(col * 60 + 10, (9 - row) * 60 + 10); // Ustawienie pozycji dla parzystych wierszy
    }
    else {
        bonusSprite.setPosition((9 - col) * 60 + 10, (9 - row) * 60 + 10); // Ustawienie pozycji dla nieparzystych wierszy
    }
    sf::Vector2f targetPos(290, 290); // Pozycja docelowa
    sf::Vector2f currentPos = bonusSprite.getPosition(); // Aktualna pozycja
    sf::Vector2f direction = targetPos - currentPos; // Obliczenie kierunku ruchu
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y); // Obliczenie d³ugoœci wektora

    if (length > 10) {
        direction.x /= length; // Normalizacja wektora
        direction.y /= length; // Normalizacja wektora
        sf::Vector2f moveVector(direction.x * 10, direction.y * 10); // Wektor ruchu
        bonusSprite.move(moveVector); // Przesuniêcie sprite'a
    }
    window.draw(bonusSprite); // Rysowanie sprite'a bonusu w oknie
}
