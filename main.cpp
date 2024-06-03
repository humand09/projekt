#include <SFML/Graphics.hpp>
#include <iostream>


// Klasa Player
class Player {
public:
    Player(const sf::Color& color, const std::string& name);
    void draw(sf::RenderWindow& window) const;
    void setPosition(int position);
    int getPosition() const;
    std::string getName() const;
private:
    sf::CircleShape mShape;
    int mPosition;
    std::string mName;
};

// Klasa Square
class Square {
public:
    virtual void triggerEvent(Player& player) = 0;
    virtual ~Square() {}
};

// Klasa Snake
class Snake : public Square {
public:
    Snake(int start, int end);
    void triggerEvent(Player& player) override;
private:
    int mStart;
    int mEnd;
};

// Klasa Ladder
class Ladder : public Square {
public:
    Ladder(int start, int end);
    void triggerEvent(Player& player) override;
private:
    int mStart;
    int mEnd;
};

// Klasa Bonus
class Bonus : public Square {
public:
    Bonus(int position);
    void triggerEvent(Player& player) override;
private:
    int mPosition;
};

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
