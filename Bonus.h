#ifndef BONUS_H
#define BONUS_H

#include "Square.h"
#include <SFML/Audio.hpp>

// Definicja klasy Bonus, ktora dziedziczy po klasie Square
class Bonus : public Square {
public:
    // Konstruktor klasy Bonus
    // Parametry:
    // - position: pozycja pola na planszy
    // - sound: referencja do obiektu dzwieku SFML
    // - texture: referencja do obiektu tekstury SFML
    Bonus(int position, sf::Sound& sound, sf::Texture& texture);

    // Metoda obslugujaca zdarzenie wejscia na pole bonusowe
    // Parametry:
    // - player: referencja do obiektu gracza
    // - diceResult: wynik rzutu kostka
    // - extraRoll: zmienna boolowska okreslajaca, czy gracz ma dodatkowy rzut kostka
    void triggerEvent(Player& player, int& diceResult, bool& extraRoll) override;

    // Metoda rysujaca pole bonusowe na oknie
    // Parametry:
    // - window: referencja do okna renderowania SFML
    void draw(sf::RenderWindow& window) const override;

private:
    // Pozycja pola bonusowego na planszy
    int mPosition;

    // Referencja do obiektu dzwieku bonusu
    sf::Sound& bonusSound;

    // Referencja do obiektu tekstury bonusu
    sf::Texture& bonusTexture;
};

#endif // BONUS_H
