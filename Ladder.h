#ifndef LADDER_H
#define LADDER_H

#include "Square.h"
#include "utilities.h"
#include <SFML/Audio.hpp>

// Definicja klasy Ladder, ktora dziedziczy po klasie Square
class Ladder : public Square {
public:
    // Konstruktor klasy Ladder
    // Parametry:
    // - start: pozycja poczatkowa drabiny
    // - end: pozycja koncowa drabiny
    // - sound: referencja do obiektu dzwieku SFML
    Ladder(int start, int end, sf::Sound& sound);

    // Metoda obslugujaca zdarzenie wejscia na pole z drabina
    // Parametry:
    // - player: referencja do obiektu gracza
    // - diceResult: wynik rzutu kostka
    // - extraRoll: zmienna boolowska okreslajaca, czy gracz ma dodatkowy rzut kostka
    void triggerEvent(Player& player, int& diceResult, bool& extraRoll) override;

    // Metoda rysujaca pole z drabina na oknie
    // Parametry:
    // - window: referencja do okna renderowania SFML
    void draw(sf::RenderWindow& window) const override;

private:
    // Pozycja poczatkowa drabiny
    int mStart;

    // Pozycja koncowa drabiny
    int mEnd;

    // Nazwa pliku z tekstura drabiny
    std::string mTextureFile;

    // Referencja do obiektu dzwieku drabiny
    sf::Sound& ladderKnockSound;

    // Prywatna metoda zwracajaca pozycje na planszy na podstawie indeksu pola
    // Parametry:
    // - squareIndex: indeks pola na planszy
    // Zwraca: wektor 2D z pozycja pola
    sf::Vector2f getPosition(int squareIndex) const;
};

#endif // LADDER_H
