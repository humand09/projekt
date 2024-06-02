#include <SFML/Graphics.hpp>
#include <iostream>

// Klasa Board
class Board {
public:
    Board();
    void draw(sf::RenderWindow& window) const;
    void movePlayer(Player& player, int roll);
private:
    void setupBoard();
    std::vector<Square*> mSquares;
    sf::Texture mBoardTexture;
    sf::Sprite mBoardSprite;
};

// Klasa Game
class Game {
public:
    Game();
    void run();
private:
    void processEvents();
    void update();
    void render();
    void rollDice();

    sf::RenderWindow mWindow;
    Board mBoard;
    std::vector<Player> mPlayers;
    int mCurrentPlayer;
    sf::Font mFont;
    sf::Text mText;
    sf::Text mMenuText;
    sf::RectangleShape mRollButton;
    bool mIsDiceRolled;
    bool mIsMenu;
    int mDiceRoll;
};

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    Game game;
    game.run();
    return 0;
}