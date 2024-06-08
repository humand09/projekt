#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <string>

enum Difficulty { EASY, MEDIUM, HARD };
Difficulty gameDifficulty;

const int windowWidth = 1000;
const int windowHeight = 600;
const float initialSpeed = 500.0f;
const float stopSpeed = 3.0f;
const int boardSize = 100;

sf::Texture diceTextures[6];
std::string diceImageFiles[6] = { "1.png", "2.png", "3.png", "4.png", "5.png", "6.png" };

class Player {
public:
    Player(const sf::Color& color, const std::string& name) : mPosition(0), mName(name) {
        mShape.setRadius(10);
        mShape.setFillColor(color);
        mShape.setPosition(0, 0);
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(mShape);
    }

    void setPosition(int position) {
        mPosition = position;
        if (mPosition >= boardSize - 1) {
            mPosition = boardSize - 1;
        }
        int row = mPosition / 10;
        int col = mPosition % 10;
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
        std::cout << player.getName() << " landed on a snake! Sliding down to " << mEnd << ".\n";
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
        std::cout << player.getName() << " climbed a ladder! Moving up to " << mEnd << ".\n";
    }

private:
    int mStart;
    int mEnd;
};

class Bonus : public Square {
public:
    Bonus(int position) : mPosition(position) {}
    void triggerEvent(Player& player) override {
        std::cout << player.getName() << " landed on a bonus! Double roll allowed.\n";
    }

private:
    int mPosition;
};

class Board {
public:
    Board() {
        if (!mBoardTexture.loadFromFile("board.png")) {
            std::cerr << "Failed to load board texture\n";
        }
        mBoardSprite.setTexture(mBoardTexture);

        mSquares.resize(boardSize, nullptr);
        mSquares[16] = new Snake(16, 6);
        mSquares[47] = new Ladder(47, 85);
        mSquares[36] = new Bonus(58);
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(mBoardSprite);
    }

    void triggerEvent(Player& player) const {
        if (mSquares[player.getPosition()]) {
            mSquares[player.getPosition()]->triggerEvent(player);
        }
    }

    ~Board() {
        for (Square* square : mSquares) {
            delete square;
        }
    }

private:
    sf::Texture mBoardTexture;
    sf::Sprite mBoardSprite;
    std::vector<Square*> mSquares;
};

void loadTextures() {
    for (int i = 0; i < 6; ++i) {
        diceTextures[i].loadFromFile(diceImageFiles[i]);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Snakes and Ladders with Dice Simulation");
    window.setFramerateLimit(60);

    loadTextures();

    sf::Sprite diceSprite;
    diceSprite.setTexture(diceTextures[0]);
    diceSprite.setPosition(windowWidth / 2 - diceSprite.getGlobalBounds().width / 2, windowHeight / 2 - diceSprite.getGlobalBounds().height / 2);

    sf::Font font;
    font.loadFromFile("arial.ttf");
    sf::Text rollButton("Rzuæ kostk¹", font, 30);
    rollButton.setPosition(windowWidth - 200, 50);

    sf::Text stopButton("Stop", font, 30);
    stopButton.setPosition(windowWidth - 200, 100);

    sf::Text diceValue("", font, 30);
    diceValue.setPosition(windowWidth - 200, 150);

    sf::Text playerPositions("", font, 20);
    playerPositions.setPosition(windowWidth - 200, 200);

    Board board;
    Player player1(sf::Color::Red, "Player 1");
    Player player2(sf::Color::Blue, "Player 2");
    Player player3(sf::Color::Green, "Player 3");

    std::vector<Player*> players = { &player1, &player2, &player3 };
    int currentPlayerIndex = 0;

    bool isRolling = false;
    float speed = initialSpeed;
    sf::Vector2f direction(1.0f, 1.0f);
    sf::Clock clock;
    sf::Clock faceChangeClock;
    int currentFace = 0;
    int diceResult = 0;

    srand(static_cast<unsigned int>(time(0)));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (rollButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) && !isRolling) {
                        isRolling = true;
                        speed = initialSpeed;
                        direction.x = static_cast<float>(rand() % 200 - 100) / 100.0f;
                        direction.y = static_cast<float>(rand() % 200 - 100) / 100.0f;
                        faceChangeClock.restart();
                        clock.restart();
                        diceValue.setString("");
                    }
                    else if (stopButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) && isRolling) {
                        isRolling = false;
                        diceSprite.setTexture(diceTextures[currentFace]);
                        diceResult = currentFace + 1;
                        Player* currentPlayer = players[currentPlayerIndex];
                        currentPlayer->setPosition(currentPlayer->getPosition() + diceResult);
                        board.triggerEvent(*currentPlayer);
                        std::string imageName = diceImageFiles[currentFace].substr(0, diceImageFiles[currentFace].find(".png"));
                        diceValue.setString("Wartoœæ: " + imageName);
                        diceValue.setPosition(windowWidth - 200, 150);
                        currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
                    }
                }
            }
        }

        if (isRolling) {
            float deltaTime = clock.restart().asSeconds();
            diceSprite.move(direction * speed * deltaTime);

            speed -= (static_cast<float>(rand() % 80 + 20)) * deltaTime;
            if (speed <= 0) {
                isRolling = false;
                diceSprite.setTexture(diceTextures[currentFace]);
                diceResult = currentFace + 1;
                Player* currentPlayer = players[currentPlayerIndex];
                currentPlayer->setPosition(currentPlayer->getPosition() + diceResult);
                board.triggerEvent(*currentPlayer);
                std::string imageName = diceImageFiles[currentFace].substr(0, diceImageFiles[currentFace].find(".png"));
                diceValue.setString("Wartoœæ: " + imageName);
                diceValue.setPosition(windowWidth - 200, 150);
                currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
            }

            if (speed > stopSpeed) {
                float faceChangeInterval = std::max(0.01f, 0.1f * (initialSpeed / speed)); // Faster speed results in shorter interval

                if (faceChangeClock.getElapsedTime().asSeconds() >= faceChangeInterval) {
                    currentFace = rand() % 6;
                    diceSprite.setTexture(diceTextures[currentFace]);
                    faceChangeClock.restart();
                }
            }

            if (diceSprite.getPosition().x < 0 || diceSprite.getPosition().x + diceSprite.getGlobalBounds().width > windowWidth) {
                direction.x = -direction.x;
            }
            if (diceSprite.getPosition().y < 0 || diceSprite.getPosition().y + diceSprite.getGlobalBounds().height > windowHeight) {
                direction.y = -direction.y;
            }
        }

        std::string positions = "Player Positions:\n";
        for (const auto& player : players) {
            positions += player->getName() + ": " + std::to_string(player->getPosition()) + "\n";
        }
        playerPositions.setString(positions);

        window.clear();
        board.draw(window);
        player1.draw(window);
        player2.draw(window);
        player3.draw(window);
        window.draw(diceSprite);
        window.draw(rollButton);
        window.draw(stopButton);
        window.draw(playerPositions);
        if (!isRolling && !diceValue.getString().isEmpty()) {
            window.draw(diceValue);
        }
        window.display();
    }

    return 0;
}
