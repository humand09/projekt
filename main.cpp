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
std::string diceImageFiles[6] = { "assets/1.png", "assets/2.png", "assets/3.png", "assets/4.png", "assets/5.png", "assets/6.png" };

void drawRectangleWithTextureAndRotation(sf::RenderWindow& window, const sf::Vector2f& topLeft, const sf::Vector2f& bottomRight, const std::string& textureFile) {
    sf::Texture texture;
    if (!texture.loadFromFile(textureFile)) {
        return;
    }

    float length = std::sqrt(std::pow(bottomRight.x - topLeft.x, 2) + std::pow(bottomRight.y - topLeft.y, 2));
    float width = 50;

    sf::RectangleShape rectangle(sf::Vector2f(length, width));
    rectangle.setPosition(topLeft);
    rectangle.setTexture(&texture);

    float dx = bottomRight.x - topLeft.x;
    float dy = bottomRight.y - topLeft.y;
    float angle = std::atan2(dy, dx) * 180 / 3.14159f;

    rectangle.setOrigin(0, width / 2);
    rectangle.setRotation(angle);

    window.draw(rectangle);
}

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
    virtual void triggerEvent(Player& player, int& diceResult, bool& extraRoll) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual ~Square() {}
};

class Snake : public Square {
public:
    Snake(int start, int end) : mStart(start), mEnd(end) {}
    void triggerEvent(Player& player, int& diceResult, bool& extraRoll) override {
        player.setPosition(mEnd);
        std::cout << player.getName() << " landed on a snake! Sliding down to " << mEnd << ".\n";
    }

    void draw(sf::RenderWindow& window) const override {
        sf::Vector2f topLeft = getPosition(mStart);
        sf::Vector2f bottomRight = getPosition(mEnd);
        drawRectangleWithTextureAndRotation(window, topLeft, bottomRight, "assets/snake1.png");
    }

private:
    int mStart;
    int mEnd;

    sf::Vector2f getPosition(int squareIndex) const {
        int row = squareIndex / 10;
        int col = squareIndex % 10;
        if (row % 2 == 0) {
            return sf::Vector2f(col * 60, (9 - row) * 60);
        }
        else {
            return sf::Vector2f((9 - col) * 60, (9 - row) * 60);
        }
    }
};

class Ladder : public Square {
public:
    Ladder(int start, int end) : mStart(start), mEnd(end) {}
    void triggerEvent(Player& player, int& diceResult, bool& extraRoll) override {
        player.setPosition(mEnd);
        std::cout << player.getName() << " climbed a ladder! Moving up to " << mEnd << ".\n";
    }

    void draw(sf::RenderWindow& window) const override {
        sf::Vector2f topLeft = getPosition(mStart);
        sf::Vector2f bottomRight = getPosition(mEnd);
        drawRectangleWithTextureAndRotation(window, topLeft, bottomRight, "assets/ladder1.png");
    }

private:
    int mStart;
    int mEnd;

    sf::Vector2f getPosition(int squareIndex) const {
        int row = squareIndex / 10;
        int col = squareIndex % 10;
        if (row % 2 == 0) {
            return sf::Vector2f(col * 60, (9 - row) * 60);
        }
        else {
            return sf::Vector2f((9 - col) * 60, (9 - row) * 60);
        }
    }
};

class Bonus : public Square {
public:
    Bonus(int position) : mPosition(position) {}
    void triggerEvent(Player& player, int& diceResult, bool& extraRoll) override {
        std::cout << player.getName() << " landed on a bonus! Double roll allowed.\n";
        extraRoll = true;
    }

    void draw(sf::RenderWindow& window) const override {
        // Drawing logic for Bonus can be added here if needed
    }

private:
    int mPosition;
};

class Board {
public:
    Board(Difficulty difficulty) {
        if (!mBoardTexture.loadFromFile("assets/board.png")) {
            std::cerr << "Failed to load board texture\n";
        }
        mBoardSprite.setTexture(mBoardTexture);

        mSquares.resize(boardSize, nullptr);
        if (difficulty == EASY) {
            mSquares[16] = new Ladder(16, 26);
            mSquares[47] = new Ladder(47, 85);
            mSquares[36] = new Bonus(58);
            mSquares[62] = new Snake(62, 19);
        }
        else if (difficulty == MEDIUM) {
            mSquares[16] = new Ladder(16, 26);
            mSquares[47] = new Ladder(47, 85);
            mSquares[36] = new Bonus(58);
            mSquares[62] = new Snake(62, 19);
            mSquares[89] = new Snake(89, 70);
        }
        else if (difficulty == HARD) {
            mSquares[16] = new Ladder(16, 26);
            mSquares[47] = new Ladder(47, 85);
            mSquares[4] = new Ladder(4, 8);
            mSquares[36] = new Bonus(58);
            mSquares[16] = new Snake(16, 2);
            mSquares[8] = new Snake(8, 5);
            mSquares[95] = new Snake(95, 56);
        }
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(mBoardSprite);
        for (const auto& square : mSquares) {
            if (square) {
                square->draw(window);
            }
        }
    }

    void triggerEvent(Player& player, int& diceResult, bool& extraRoll) const {
        if (mSquares[player.getPosition()]) {
            mSquares[player.getPosition()]->triggerEvent(player, diceResult, extraRoll);
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

void startGame(sf::RenderWindow& window, Difficulty difficulty, int numPlayers) {
    loadTextures();

    sf::Sprite diceSprite;
    diceSprite.setTexture(diceTextures[0]);
    diceSprite.setPosition(windowWidth / 2 - diceSprite.getGlobalBounds().width / 2, windowHeight / 2 - diceSprite.getGlobalBounds().height / 2);

    sf::Font font;
    font.loadFromFile("assets/arial.ttf");
    sf::Text rollButton("Rzuc kostka", font, 30);
    rollButton.setPosition(windowWidth - 200, 50);
    rollButton.setFillColor(sf::Color::White);

    sf::Text stopButton("Stop", font, 30);
    stopButton.setPosition(windowWidth - 200, 100);
    stopButton.setFillColor(sf::Color::White);

    sf::Text diceValue("", font, 30);
    diceValue.setPosition(windowWidth - 200, 150);
    diceValue.setFillColor(sf::Color::White);

    sf::Text playerPositions("", font, 20);
    playerPositions.setPosition(windowWidth - 200, 200);
    playerPositions.setFillColor(sf::Color::White);

    Board board(difficulty);
    Player player1(sf::Color::Red, "Player 1");
    Player player2(sf::Color::Blue, "Player 2");
    Player player3(sf::Color::Green, "Player 3");
    Player player4(sf::Color::Yellow, "Player 4");

    std::vector<Player*> players = { &player1, &player2, &player3, &player4 };
    players.resize(numPlayers);

    int currentPlayerIndex = 0;

    bool isRolling = false;
    bool extraRoll = false;
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
                        board.triggerEvent(*currentPlayer, diceResult, extraRoll);
                        std::string imageName = diceImageFiles[currentFace];
                        std::size_t startPos = imageName.find("assets/") + 7;
                        std::size_t endPos = imageName.find(".png");
                        imageName = imageName.substr(startPos, endPos - startPos);
                        diceValue.setString("Wartosc: " + imageName);
                        diceValue.setPosition(windowWidth - 200, 150);
                        if (!extraRoll) {
                            currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
                        }
                        else {
                            extraRoll = false;
                        }
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
                board.triggerEvent(*currentPlayer, diceResult, extraRoll);
                std::string imageName = diceImageFiles[currentFace];
                std::size_t startPos = imageName.find("assets/") + 7;
                std::size_t endPos = imageName.find(".png");
                imageName = imageName.substr(startPos, endPos - startPos);
                diceValue.setString("Wartosc: " + imageName);
                diceValue.setPosition(windowWidth - 200, 150);
                if (!extraRoll) {
                    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
                }
                else {
                    extraRoll = false;
                }
            }

            if (speed > stopSpeed) {
                float faceChangeInterval = std::max(0.01f, 0.1f * (initialSpeed / speed));

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
        for (const auto& player : players) {
            player->draw(window);
        }
        window.draw(diceSprite);
        window.draw(rollButton);
        window.draw(stopButton);
        window.draw(playerPositions);
        if (!isRolling && !diceValue.getString().isEmpty()) {
            window.draw(diceValue);
        }
        window.display();
    }
}

void showMenu(sf::RenderWindow& window) {
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("assets/background.png")) {
        std::cerr << "Failed to load background texture\n";
    }
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    sf::Font font;
    font.loadFromFile("assets/arial.ttf");

    sf::Text title("Weze i drabiny", font, 50);
    title.setPosition(windowWidth / 2 - title.getGlobalBounds().width / 2, 20);
    title.setFillColor(sf::Color::Black);

    sf::Text newGameButton("Nowa gra", font, 30);
    newGameButton.setPosition(windowWidth / 2 - newGameButton.getGlobalBounds().width / 2, 100);
    newGameButton.setFillColor(sf::Color::Black);

    sf::Text difficultyText("Wybor trudnosci:", font, 20);
    difficultyText.setPosition(windowWidth / 2 - difficultyText.getGlobalBounds().width / 2, 200);
    difficultyText.setFillColor(sf::Color::Black);

    sf::Text easyOption("Latwy", font, 20);
    easyOption.setPosition(windowWidth / 2 - 100, 250);
    easyOption.setFillColor(sf::Color::Black);

    sf::Text mediumOption("Sredni", font, 20);
    mediumOption.setPosition(windowWidth / 2 - 30, 250);
    mediumOption.setFillColor(sf::Color::Black);

    sf::Text hardOption("Trudny", font, 20);
    hardOption.setPosition(windowWidth / 2 + 50, 250);
    hardOption.setFillColor(sf::Color::Black);

    sf::Text playersText("Ilosc graczy", font, 20);
    playersText.setPosition(windowWidth / 2 - playersText.getGlobalBounds().width / 2, 300);
    playersText.setFillColor(sf::Color::Black);

    sf::Text twoPlayers("2", font, 20);
    twoPlayers.setPosition(windowWidth / 2 - 60, 350);
    twoPlayers.setFillColor(sf::Color::Black);

    sf::Text threePlayers("3", font, 20);
    threePlayers.setPosition(windowWidth / 2 - 20, 350);
    threePlayers.setFillColor(sf::Color::Black);

    sf::Text fourPlayers("4", font, 20);
    fourPlayers.setPosition(windowWidth / 2 + 20, 350);
    fourPlayers.setFillColor(sf::Color::Black);

    Difficulty selectedDifficulty = EASY;
    int selectedPlayers = 2;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    if (newGameButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        startGame(window, selectedDifficulty, selectedPlayers);
                    }
                    else if (easyOption.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        selectedDifficulty = EASY;
                    }
                    else if (mediumOption.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        selectedDifficulty = MEDIUM;
                    }
                    else if (hardOption.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        selectedDifficulty = HARD;
                    }
                    else if (twoPlayers.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        selectedPlayers = 2;
                    }
                    else if (threePlayers.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        selectedPlayers = 3;
                    }
                    else if (fourPlayers.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        selectedPlayers = 4;
                    }
                }
            }
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(title);
        window.draw(newGameButton);
        window.draw(difficultyText);
        window.draw(easyOption);
        window.draw(mediumOption);
        window.draw(hardOption);
        window.draw(playersText);
        window.draw(twoPlayers);
        window.draw(threePlayers);
        window.draw(fourPlayers);

        window.display();
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Snakes and Ladders");
    window.setFramerateLimit(60);

    showMenu(window);

    return 0;
}
