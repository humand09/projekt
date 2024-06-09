#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include "Player.h"
#include "Board.h"
#include "Difficulty.h"

// Definicja klasy Game
class Game {
public:
    // Konstruktor klasy Game
    Game();

    // Metoda uruchamiajaca gre
    void run();

    // Bufory dzwiekowe
    sf::SoundBuffer snakeHissBuffer;
    sf::SoundBuffer bonusBuffer;
    sf::SoundBuffer ladderKnockBuffer;
    sf::SoundBuffer clickInGameBuffer;
    sf::SoundBuffer diceStartBuffer;

    // Dzwieki
    sf::Sound snakeHissSound;
    sf::Sound bonusSound;
    sf::Sound ladderKnockSound;
    sf::Sound clickInGameSound;
    sf::Sound diceStartSound;

    // Tekstury
    sf::Texture bonusTexture;
    sf::Texture logoTexture;
    sf::Sprite logoSprite;

    // Tablica tekstur dla kostki do gry
    sf::Texture diceTextures[6];

    // Tablica nazw plikow z obrazkami kostki
    std::string diceImageFiles[6] = { "assets/1.png", "assets/2.png", "assets/3.png", "assets/4.png", "assets/5.png", "assets/6.png" };

private:
    // Prywatne metody ladowania zasobow
    void loadTextures();
    void loadSounds();

    // Metoda wyswietlajaca menu gry
    // Parametry:
    // - window: referencja do okna renderowania SFML
    // - music: referencja do obiektu muzyki SFML
    void showMenu(sf::RenderWindow& window, sf::Music& music);

    // Metoda rozpoczynajaca gre
    // Parametry:
    // - window: referencja do okna renderowania SFML
    // - difficulty: poziom trudnosci gry
    // - numPlayers: liczba graczy
    // - music: referencja do obiektu muzyki SFML
    // - boardTexture: sciezka do pliku z tekstura planszy
    void startGame(sf::RenderWindow& window, Difficulty difficulty, int numPlayers, sf::Music& music, const std::string& boardTexture);

    // Metoda wyswietlajaca komunikat o wygranej
    // Parametry:
    // - window: referencja do okna renderowania SFML
    // - playerNumber: numer gracza, ktory wygral
    void displayWinMessage(sf::RenderWindow& window, int playerNumber);

    // Metoda wyswietlajaca zasady gry
    // Parametry:
    // - window: referencja do okna renderowania SFML
    // - font: referencja do obiektu czcionki SFML
    // - music: referencja do obiektu muzyki SFML
    void showRules(sf::RenderWindow& window, sf::Font& font, sf::Music& music);
};

// Deklaracje globalnych stalych
extern const int windowWidth;
extern const int windowHeight;
extern const float initialSpeed;
extern const float stopSpeed;
extern const int boardSize;

#endif // GAME_H
