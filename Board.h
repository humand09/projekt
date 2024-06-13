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

// Forward declaration klasy Game, aby unikn�� problem�w z zale�no�ciami cyklicznymi
class Game;

// Definicja klasy Board
class Board {
public:
    // Konstruktor klasy Board
    // Parametry:
    // - difficulty: poziom trudno�ci gry
    // - textureFile: �cie�ka do pliku z tekstur� planszy
    // - game: referencja do obiektu klasy Game
    Board(Difficulty difficulty, const std::string& textureFile, Game& game);

    // Metoda rysuj�ca plansz� na oknie
    // Parametry:
    // - window: referencja do okna renderowania SFML
    void draw(sf::RenderWindow& window) const;

    // Metoda obs�uguj�ca zdarzenia na planszy, takie jak wej�cie na pole specjalne
    // Parametry:
    // - player: referencja do obiektu gracza
    // - diceResult: wynik rzutu kostk�
    // - extraRoll: zmienna boolowska okre�laj�ca, czy gracz ma dodatkowy rzut kostk�
    void triggerEvent(Player& player, int& diceResult, bool& extraRoll) const;

    // Metoda ustawiaj�ca tekstur� planszy
    // Parametry:
    // - textureFile: �cie�ka do pliku z tekstur� planszy
    void setBoardTexture(const std::string& textureFile);

    // Destruktor klasy Board
    ~Board();

private:
    // Tekstura planszy
    sf::Texture mBoardTexture;

    // Sprite reprezentuj�cy plansz�
    sf::Sprite mBoardSprite;

    // Wektor wska�nik�w do obiekt�w klasy Square, reprezentuj�cych pola na planszy
    std::vector<std::unique_ptr<Square>> mSquares;

    // Prywatna metoda zwracaj�ca nazw� pliku z plansz� w zale�no�ci od poziomu trudno�ci
    // Parametry:
    // - difficulty: poziom trudno�ci gry
    // Zwraca: nazw� pliku w postaci stringa
    std::string getFilenameForDifficulty(Difficulty difficulty);

    // Prywatna metoda �aduj�ca plansz� z pliku
    // Parametry:
    // - filename: nazwa pliku z plansz�
    // - game: referencja do obiektu klasy Game
    void loadFromFile(const std::string& filename, Game& game);
};

#endif // BOARD_H
