#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Square.h"
#include "Snake.h"
#include "Ladder.h"
#include "Bonus.h"
#include "Game.h"
#include "Difficulty.h"

// Forward declaration klasy Game, aby unikn¹æ problemów z zale¿noœciami cyklicznymi
class Game;

// Definicja klasy Board
class Board {
public:
    // Konstruktor klasy Board
    // Parametry:
    // - difficulty: poziom trudnoœci gry
    // - textureFile: œcie¿ka do pliku z tekstur¹ planszy
    // - game: referencja do obiektu klasy Game
    Board(Difficulty difficulty, const std::string& textureFile, Game& game);

    // Metoda rysuj¹ca planszê na oknie
    // Parametry:
    // - window: referencja do okna renderowania SFML
    void draw(sf::RenderWindow& window) const;

    // Metoda obs³uguj¹ca zdarzenia na planszy, takie jak wejœcie na pole specjalne
    // Parametry:
    // - player: referencja do obiektu gracza
    // - diceResult: wynik rzutu kostk¹
    // - extraRoll: zmienna boolowska okreœlaj¹ca, czy gracz ma dodatkowy rzut kostk¹
    void triggerEvent(Player& player, int& diceResult, bool& extraRoll) const;

    // Metoda ustawiaj¹ca teksturê planszy
    // Parametry:
    // - textureFile: œcie¿ka do pliku z tekstur¹ planszy
    void setBoardTexture(const std::string& textureFile);

    // Destruktor klasy Board
    ~Board();

private:
    // Tekstura planszy
    sf::Texture mBoardTexture;

    // Sprite reprezentuj¹cy planszê
    sf::Sprite mBoardSprite;

    // Wektor wskaŸników do obiektów klasy Square, reprezentuj¹cych pola na planszy
    std::vector<std::unique_ptr<Square>> mSquares;

    // Prywatna metoda zwracaj¹ca nazwê pliku z plansz¹ w zale¿noœci od poziomu trudnoœci
    // Parametry:
    // - difficulty: poziom trudnoœci gry
    // Zwraca: nazwê pliku w postaci stringa
    std::string getFilenameForDifficulty(Difficulty difficulty);

    // Prywatna metoda ³aduj¹ca planszê z pliku
    // Parametry:
    // - filename: nazwa pliku z plansz¹
    // - game: referencja do obiektu klasy Game
    void loadFromFile(const std::string& filename, Game& game);
};

#endif // BOARD_H
