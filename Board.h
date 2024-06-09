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

class Game;

class Board {
public:
    Board(Difficulty difficulty, const std::string& textureFile, Game& game);
    void draw(sf::RenderWindow& window) const;
    void triggerEvent(Player& player, int& diceResult, bool& extraRoll) const;
    void setBoardTexture(const std::string& textureFile);
    ~Board();

private:
    sf::Texture mBoardTexture;
    sf::Sprite mBoardSprite;
    std::vector<std::unique_ptr<Square>> mSquares;

    std::string getFilenameForDifficulty(Difficulty difficulty);
    void loadFromFile(const std::string& filename, Game& game);
};

#endif
