#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <iostream> // Dodano nagłówek <iostream>

enum Difficulty { EASY, MEDIUM, HARD };
Difficulty gameDifficulty;

class Player {
public:
    Player(const sf::Color& color, const std::string& name) : mPosition(0), mName(name) {
        mShape.setRadius(10);
        mShape.setFillColor(color);
        mShape.setPosition(0, 0); // Początkowa pozycja
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(mShape);
    }

    void setPosition(int position) {
        mPosition = position;
        int row = position / 10;
        int col = position % 10;
        if (row % 2 == 0) {
            mShape.setPosition(col * 60, (9 - row) * 60);
        }
        else {
            mShape.setPosition((9 - col) * 60, (9 - row) * 60);
        }
    }

    int getPosition() const {
        return mPosition;
    }

    std::string getName() const {
        return mName;
    }

private:
    sf::CircleShape mShape;
    int mPosition;
    std::string mName;
};

class Square {
public:
    virtual void triggerEvent(Player& player) = 0;
    virtual ~Square() {}
};

class Snake : public Square {
public:
    Snake(int start, int end) : mStart(start), mEnd(end) {}
    void triggerEvent(Player& player) override {
        player.setPosition(mEnd);
    }

private:
    int mStart;
    int mEnd;
};

class Ladder : public Square {
public:
    Ladder(int start, int end) : mStart(start), mEnd(end) {}
    void triggerEvent(Player& player) override {
        player.setPosition(mEnd);
    }

private:
    int mStart;
    int mEnd;
};

class Bonus : public Square {
public:
    Bonus(int position) : mPosition(position) {}
    void triggerEvent(Player& player) override {
        player.setPosition(mPosition);
        // Bonus effect: Allow an extra roll
        std::cout << "Bonus! Double roll allowed.\n";
    }

private:
    int mPosition;
};

class Board {
public:
    Board() {
        if (!mBoardTexture.loadFromFile("assets/board.png")) {
            std::cerr << "Error loading board.png" << std::endl;
        }
        mBoardSprite.setTexture(mBoardTexture);
        setupBoard();
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(mBoardSprite);
    }

    void movePlayer(Player& player, int roll) {
        int newPosition = player.getPosition() + roll;
        if (newPosition >= 100) {
            newPosition = 99;
        }
        player.setPosition(newPosition);
        if (mSquares[newPosition]) {
            mSquares[newPosition]->triggerEvent(player);
        }
    }

private:
    void setupBoard() {
        for (int i = 0; i < 100; ++i) {
            mSquares.push_back(nullptr);
        }
        mSquares[16] = new Snake(16, 6);
        mSquares[47] = new Ladder(47, 87);
        mSquares[22] = new Bonus(22); // Example bonus
        if (gameDifficulty == EASY) {
            mSquares[30] = new Ladder(30, 50);
        }
        else if (gameDifficulty == MEDIUM) {
            mSquares[30] = new Ladder(30, 40);
            mSquares[70] = new Snake(70, 50);
        }
        else if (gameDifficulty == HARD) {
            mSquares[10] = new Snake(10, 2);
            mSquares[30] = new Snake(30, 5);
            mSquares[60] = new Ladder(60, 80);
        }
    }

    std::vector<Square*> mSquares;
    sf::Texture mBoardTexture;
    sf::Sprite mBoardSprite;
};

class Game {
public:
    Game()
        : mWindow(sf::VideoMode(800, 600), "Węże i Drabiny"),
        mCurrentPlayer(0), mIsDiceRolled(false), mDiceRoll(0), mIsMenu(true) {
        if (!mFont.loadFromFile("assets/font.ttf")) {
            std::cerr << "Error loading font.ttf" << std::endl;
        }
        mText.setFont(mFont);
        mText.setCharacterSize(24);
        mText.setFillColor(sf::Color::White);
        mText.setPosition(650, 50);

        mMenuText.setFont(mFont);
        mMenuText.setCharacterSize(36);
        mMenuText.setFillColor(sf::Color::White);
        mMenuText.setPosition(150, 100);
        mMenuText.setString("Press 1 for Easy, 2 for Medium, 3 for Hard");

        mRollButton.setSize(sf::Vector2f(100, 50));
        mRollButton.setFillColor(sf::Color::Yellow);
        mRollButton.setPosition(650, 100);

        mPlayers.push_back(Player(sf::Color::Red, "Player 1"));
        mPlayers.push_back(Player(sf::Color::Blue, "Player 2"));
        mPlayers.push_back(Player(sf::Color::Green, "Player 3"));
    }

    void run() {
        while (mWindow.isOpen()) {
            processEvents();
            update();
            render();
        }
    }

private:
    void processEvents() {
        sf::Event event;
        while (mWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                mWindow.close();
            if (mIsMenu && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1) {
                    gameDifficulty = EASY;
                    mIsMenu = false;
                }
                else if (event.key.code == sf::Keyboard::Num2) {
                    gameDifficulty = MEDIUM;
                    mIsMenu = false;
                }
                else if (event.key.code == sf::Keyboard::Num3) {
                    gameDifficulty = HARD;
                    mIsMenu = false;
                }
                if (!mIsMenu) {
                    mBoard = Board();
                }
            }
            if (!mIsMenu && event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(mWindow);
                    if (mRollButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        rollDice();
                    }
                }
            }
        }
    }

    void update() {
        if (mIsDiceRolled) {
            mBoard.movePlayer(mPlayers[mCurrentPlayer], mDiceRoll);
            mCurrentPlayer = (mCurrentPlayer + 1) % mPlayers.size();
            mIsDiceRolled = false;
        }
    }

    void render() {
        mWindow.clear();

        if (mIsMenu) {
            mWindow.draw(mMenuText);
        }
        else {
            mBoard.draw(mWindow);
            for (const auto& player : mPlayers) {
                player.draw(mWindow);
            }
            mWindow.draw(mRollButton);
            mWindow.draw(mText);

            float yPosition = 200.0f;
            for (const auto& player : mPlayers) {
                sf::Text playerText;
                playerText.setFont(mFont);
                playerText.setCharacterSize(20);
                playerText.setFillColor(sf::Color::White);
                playerText.setPosition(650, yPosition);
                playerText.setString(player.getName() + ": " + std::to_string(player.getPosition()));
                mWindow.draw(playerText);
                yPosition += 30.0f;
            }
        }

        mWindow.display();
    }

    void rollDice() {
        mDiceRoll = rand() % 6 + 1;
        mText.setString("Rolled: " + std::to_string(mDiceRoll));
        mIsDiceRolled = true;
    }

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
