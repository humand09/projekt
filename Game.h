#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include "Player.h"
#include "Board.h"
#include "Difficulty.h"

class Game {
public:
    Game();
    void run();

    sf::SoundBuffer snakeHissBuffer;
    sf::SoundBuffer bonusBuffer;
    sf::SoundBuffer ladderKnockBuffer;
    sf::SoundBuffer clickInGameBuffer;
    sf::SoundBuffer diceStartBuffer;

    sf::Sound snakeHissSound;
    sf::Sound bonusSound;
    sf::Sound ladderKnockSound;
    sf::Sound clickInGameSound;
    sf::Sound diceStartSound;

    sf::Texture bonusTexture;
    sf::Texture logoTexture;
    sf::Sprite logoSprite;

    sf::Texture diceTextures[6];
    std::string diceImageFiles[6] = { "assets/1.png", "assets/2.png", "assets/3.png", "assets/4.png", "assets/5.png", "assets/6.png" };

private:
    void loadTextures();
    void loadSounds();
    void showMenu(sf::RenderWindow& window, sf::Music& music);
    void startGame(sf::RenderWindow& window, Difficulty difficulty, int numPlayers, sf::Music& music, const std::string& boardTexture);
    void displayWinMessage(sf::RenderWindow& window, int playerNumber);
    void showRules(sf::RenderWindow& window, sf::Font& font, sf::Music& music);
};

extern const int windowWidth;
extern const int windowHeight;
extern const float initialSpeed;
extern const float stopSpeed;
extern const int boardSize;

#endif
