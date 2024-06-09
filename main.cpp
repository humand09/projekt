#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>

void printMousePosition(sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    std::cout << "Mouse Position: x = " << mousePos.x << ", y = " << mousePos.y << std::endl;
}

enum Difficulty { EASY, MEDIUM, HARD };
Difficulty gameDifficulty;

const int windowWidth = 1000;
const int windowHeight = 600;
const float initialSpeed = 500.0f;
const float stopSpeed = 3.0f;
const int boardSize = 100;

sf::Texture diceTextures[6];
std::string diceImageFiles[6] = { "assets/1.png", "assets/2.png", "assets/3.png", "assets/4.png", "assets/5.png", "assets/6.png" };
sf::Texture bonusTexture;

sf::SoundBuffer snakeHissBuffer;
sf::SoundBuffer bonusBuffer;
sf::SoundBuffer ladderKnockBuffer;
sf::SoundBuffer clickInMenuBuffer;
sf::SoundBuffer clickInGameBuffer;
sf::SoundBuffer diceStartBuffer;

sf::Sound snakeHissSound;
sf::Sound bonusSound;
sf::Sound ladderKnockSound;
sf::Sound clickInMenuSound;
sf::Sound clickInGameSound;
sf::Sound diceStartSound;

void drawRectangleWithTextureAndRotation(sf::RenderWindow& window, const sf::Vector2f& topLeft, const sf::Vector2f& bottomRight, const std::string& textureFile) {
    sf::Texture texture;
    if (!texture.loadFromFile(textureFile)) {
        return;
    }

    float length = std::sqrt(std::pow(bottomRight.x - topLeft.x, 2) + std::pow(bottomRight.y - topLeft.y, 2));
    float width = 50;

    sf::RectangleShape rectangle(sf::Vector2f(length, width));
    rectangle.setPosition(topLeft.x + 10, topLeft.y + 10);
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
    Player(const sf::Color& color, const std::string& name)
        : mPosition(0), mTargetPosition(0, 0), mMoving(false), mName(name) {
        mShape.setRadius(10);
        mShape.setFillColor(color);
        mShape.setPosition(10, 10);
    }

    void draw(sf::RenderWindow& window, const sf::Vector2f& offset) const {
        sf::CircleShape shape = mShape;
        shape.move(offset);
        window.draw(shape);
    }

    void moveToPosition(int position) {
        mPosition = position;
        if (mPosition >= boardSize - 1) {
            mPosition = boardSize - 1;
        }
        int row = mPosition / 10;
        int col = mPosition % 10;
        if (row % 2 == 0) {
            mTargetPosition = sf::Vector2f(col * 60 + 10, (9 - row) * 60 + 10);
        }
        else {
            mTargetPosition = sf::Vector2f((9 - col) * 60 + 10, (9 - row) * 60 + 10);
        }
        mMoving = true;
    }

    void update(float deltaTime) {
        if (mMoving) {
            sf::Vector2f currentPosition = mShape.getPosition();
            sf::Vector2f direction = mTargetPosition - currentPosition;
            float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            if (distance < 5.0f) {
                mShape.setPosition(mTargetPosition);
                mShape.move(5 * (285 - mShape.getPosition().x) / std::sqrt(std::pow(285 - mShape.getPosition().x, 2) + std::pow(280 - mShape.getPosition().y, 2)), 5 * (280 - mShape.getPosition().y) / std::sqrt(std::pow(285 - mShape.getPosition().x, 2) + std::pow(280 - mShape.getPosition().y, 2)));
                mMoving = false;
            }
            else {
                direction /= distance;
                mShape.move(direction * 100.0f * deltaTime);
            }
        }
    }

    int getPosition() const {
        return mPosition;
    }

    std::string getName() const {
        return mName;
    }

    void resetPosition() {
        mPosition = 0;
        mShape.setPosition(34, 540);
        mTargetPosition = sf::Vector2f(33, 540);
    }

private:
    sf::CircleShape mShape;
    sf::Vector2f mTargetPosition;
    bool mMoving;
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
    Snake(int start, int end) : mStart(start), mEnd(end) {
        int textureIndex = rand() % 3;
        mTextureFile = "assets/snake" + std::to_string(textureIndex + 1) + ".png";
    }
    void triggerEvent(Player& player, int& diceResult, bool& extraRoll) override {
        player.moveToPosition(mEnd);
        snakeHissSound.play();
        std::cout << player.getName() << " landed on a snake! Sliding down to " << mEnd << ".\n";
    }

    void draw(sf::RenderWindow& window) const override {
        sf::Vector2f topLeft = getPosition(mStart);
        sf::Vector2f bottomRight = getPosition(mEnd);
        drawRectangleWithTextureAndRotation(window, topLeft, bottomRight, mTextureFile);
    }

private:
    int mStart;
    int mEnd;
    std::string mTextureFile;

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
    Ladder(int start, int end) : mStart(start), mEnd(end) {
        int textureIndex = rand() % 3;
        mTextureFile = "assets/ladder" + std::to_string(textureIndex + 1) + ".png";
    }
    void triggerEvent(Player& player, int& diceResult, bool& extraRoll) override {
        player.moveToPosition(mEnd);
        ladderKnockSound.play();
        std::cout << player.getName() << " climbed a ladder! Moving up to " << mEnd << ".\n";
    }

    void draw(sf::RenderWindow& window) const override {
        sf::Vector2f topLeft = getPosition(mStart);
        sf::Vector2f bottomRight = getPosition(mEnd);
        drawRectangleWithTextureAndRotation(window, topLeft, bottomRight, mTextureFile);
    }

private:
    int mStart;
    int mEnd;
    std::string mTextureFile;

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
        bonusSound.play();
        std::cout << player.getName() << " landed on a bonus! Double roll allowed.\n";
        extraRoll = true;
    }

    void draw(sf::RenderWindow& window) const override {
        sf::Sprite bonusSprite(bonusTexture);
        bonusSprite.setScale(0.04f, 0.04f);
        int row = mPosition / 10;
        int col = mPosition % 10;
        if (row % 2 == 0) {
            bonusSprite.setPosition(col * 60 + 10, (9 - row) * 60 + 10);
        }
        else {
            bonusSprite.setPosition((9 - col) * 60 + 10, (9 - row) * 60 + 10);
        }
        sf::Vector2f targetPos(290, 290);
        sf::Vector2f currentPos = bonusSprite.getPosition();
        sf::Vector2f direction = targetPos - currentPos;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (length > 10) {
            direction.x /= length;
            direction.y /= length;
            sf::Vector2f moveVector(direction.x * 10, direction.y * 10);
            bonusSprite.move(moveVector);
        }
        window.draw(bonusSprite);
    }

private:
    int mPosition;
};

class Board {
public:
    Board(Difficulty difficulty, const std::string& textureFile) {
        if (!mBoardTexture.loadFromFile(textureFile)) {
            std::cerr << "Failed to load board texture\n";
        }
        mBoardSprite.setTexture(mBoardTexture);
        mBoardSprite.setPosition(10, 10);

        mSquares.resize(boardSize, nullptr);
        loadFromFile(getFilenameForDifficulty(difficulty));
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

    void setBoardTexture(const std::string& textureFile) {
        if (!mBoardTexture.loadFromFile(textureFile)) {
            std::cerr << "Failed to load board texture: " << textureFile << "\n";
        }
        mBoardSprite.setTexture(mBoardTexture);
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

    std::string getFilenameForDifficulty(Difficulty difficulty) {
        switch (difficulty) {
        case EASY:
            return "assets/easy.txt";
        case MEDIUM:
            return "assets/medium.txt";
        case HARD:
            return "assets/hard.txt";
        default:
            return "assets/easy.txt";
        }
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            int type, start, end;
            iss >> type >> start >> end;

            switch (type) {
            case 1:
                mSquares[start] = new Ladder(start, end);
                break;
            case 2:
                mSquares[start] = new Snake(start, end);
                break;
            case 3:
                mSquares[start] = new Bonus(start);
                break;
            default:
                std::cerr << "Unknown type: " << type << std::endl;
                break;
            }
        }
    }
};

void loadTextures() {
    for (int i = 0; i < 6; ++i) {
        diceTextures[i].loadFromFile(diceImageFiles[i]);
    }
    if (!bonusTexture.loadFromFile("assets/extraroll.png")) {
        std::cerr << "Failed to load bonus texture\n";
    }
}

void loadSounds() {
    if (!snakeHissBuffer.loadFromFile("assets/snakeHiss.ogg")) {
        std::cerr << "Failed to load snake hiss sound\n";
    }
    if (!bonusBuffer.loadFromFile("assets/bonus.ogg")) {
        std::cerr << "Failed to load bonus sound\n";
    }
    if (!ladderKnockBuffer.loadFromFile("assets/ladderKnock.ogg")) {
        std::cerr << "Failed to load ladder knock sound\n";
    }
    if (!clickInMenuBuffer.loadFromFile("assets/clickInMenu.ogg")) {
        std::cerr << "Failed to load click in menu sound\n";
    }
    if (!clickInGameBuffer.loadFromFile("assets/clickInGame.ogg")) {
        std::cerr << "Failed to load click in game sound\n";
    }
    if (!diceStartBuffer.loadFromFile("assets/diceStart.ogg")) {
        std::cerr << "Failed to load dice start sound\n";
    }

    snakeHissSound.setBuffer(snakeHissBuffer);
    bonusSound.setBuffer(bonusBuffer);
    ladderKnockSound.setBuffer(ladderKnockBuffer);
    clickInMenuSound.setBuffer(clickInMenuBuffer);
    clickInGameSound.setBuffer(clickInGameBuffer);
    diceStartSound.setBuffer(diceStartBuffer);
}

void startGame(sf::RenderWindow& window, Difficulty difficulty, int numPlayers, sf::Music& music, const std::string& boardTexture) {
    loadTextures();
    loadSounds();

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

    Board board(difficulty, boardTexture);
    Player player1(sf::Color::Red, "Player 1");
    Player player2(sf::Color::Blue, "Player 2");
    Player player3(sf::Color::Green, "Player 3");
    Player player4(sf::Color::Yellow, "Player 4");

    std::vector<Player*> players = { &player1, &player2, &player3, &player4 };
    players.resize(numPlayers);

    for (auto& player : players) {
        player->resetPosition();
    }

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

    if (music.getStatus() != sf::Music::Playing) {
        music.play();
    }

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
                        diceStartSound.play();
                    }
                    else if (stopButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) && isRolling) {
                        isRolling = false;
                        diceSprite.setTexture(diceTextures[currentFace]);
                        diceResult = currentFace + 1;
                        Player* currentPlayer = players[currentPlayerIndex];
                        currentPlayer->moveToPosition(currentPlayer->getPosition() + diceResult);
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
                        clickInGameSound.play();
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
                currentPlayer->moveToPosition(currentPlayer->getPosition() + diceResult);
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

        float deltaTime = clock.restart().asSeconds();
        for (auto& player : players) {
            player->update(deltaTime);
        }

        std::string positions = "Player Positions:\n";
        std::map<int, int> positionCount;
        for (const auto& player : players) {
            positionCount[player->getPosition()]++;
        }

        for (const auto& player : players) {
            positions += player->getName() + ": " + std::to_string(player->getPosition()) + "\n";
        }
        playerPositions.setString(positions);

        window.clear();
        board.draw(window);

        std::map<int, int> drawnCount;
        for (const auto& player : players) {
            int pos = player->getPosition();
            sf::Vector2f offset(0.f, 0.f);
            if (positionCount[pos] > 1) {
                int count = drawnCount[pos]++;
                float angle = 2 * 3.14159f * count / positionCount[pos];
                offset = sf::Vector2f(std::cos(angle) * 10.f, std::sin(angle) * 10.f);
            }
            player->draw(window, offset);
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

void showRules(sf::RenderWindow& window, sf::Font& font, sf::Music& music) {
    std::ifstream file("assets/zasady.txt");
    if (!file.is_open()) {
        std::cerr << "Error opening file: zasady.txt" << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string rulesTextString = buffer.str();

    sf::Text rulesText(rulesTextString, font, 20);
    rulesText.setPosition(50, 100);
    rulesText.setFillColor(sf::Color::Black);

    sf::Text backButton("Powrot do menu", font, 30);
    backButton.setPosition(windowWidth / 2 - backButton.getGlobalBounds().width / 2, windowHeight - 100);
    backButton.setFillColor(sf::Color::Black);

    if (music.getStatus() != sf::Music::Playing) {
        music.play();
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        clickInMenuSound.play();
                        return;
                    }
                }
            }
        }

        window.clear(sf::Color::White);
        window.draw(rulesText);
        window.draw(backButton);
        window.display();
    }
}

void showMenu(sf::RenderWindow& window, sf::Music& music) {
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

    sf::Text rulesButton("Zasady", font, 30);
    rulesButton.setPosition(windowWidth / 2 - rulesButton.getGlobalBounds().width / 2, 150);
    rulesButton.setFillColor(sf::Color::Black);

    sf::Text difficultyText("Wybor trudnosci:", font, 20);
    difficultyText.setPosition(windowWidth / 2 - difficultyText.getGlobalBounds().width / 2, 200);
    difficultyText.setFillColor(sf::Color::Black);

    sf::Text easyOption("Latwy", font, 20);
    easyOption.setPosition(windowWidth / 2 - 110, 250);

    sf::Text mediumOption("Sredni", font, 20);
    mediumOption.setPosition(windowWidth / 2 - 30, 250);

    sf::Text hardOption("Trudny", font, 20);
    hardOption.setPosition(windowWidth / 2 + 50, 250);

    sf::Text playersText("Ilosc graczy", font, 20);
    playersText.setPosition(windowWidth / 2 - playersText.getGlobalBounds().width / 2, 300);
    playersText.setFillColor(sf::Color::Black);

    sf::Text twoPlayers("2", font, 20);
    twoPlayers.setPosition(windowWidth / 2 - 50, 350);

    sf::Text threePlayers("3", font, 20);
    threePlayers.setPosition(windowWidth / 2 - 10, 350);

    sf::Text fourPlayers("4", font, 20);
    fourPlayers.setPosition(windowWidth / 2 + 30, 350);

    sf::Text boardColorText("Kolor Planszy", font, 20);
    boardColorText.setPosition(windowWidth / 2 - boardColorText.getGlobalBounds().width / 2, 400);
    boardColorText.setFillColor(sf::Color::Black);

    sf::Text lightBoardOption("Jasny", font, 20);
    lightBoardOption.setPosition(windowWidth / 2 - 60, 450);

    sf::Text darkBoardOption("Ciemny", font, 20);
    darkBoardOption.setPosition(windowWidth / 2 + 0, 450);

    Difficulty selectedDifficulty = EASY;
    int selectedPlayers = 2;
    std::string selectedBoardTexture = "assets/lightBoard.png";

    if (music.getStatus() != sf::Music::Playing) {
        music.play();
    }

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
                        clickInMenuSound.play();
                        startGame(window, selectedDifficulty, selectedPlayers, music, selectedBoardTexture);
                    }
                    else if (rulesButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        clickInMenuSound.play();
                        showRules(window, font, music);
                    }
                    else if (easyOption.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        clickInMenuSound.play();
                        selectedDifficulty = EASY;
                    }
                    else if (mediumOption.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        clickInMenuSound.play();
                        selectedDifficulty = MEDIUM;
                    }
                    else if (hardOption.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        clickInMenuSound.play();
                        selectedDifficulty = HARD;
                    }
                    else if (twoPlayers.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        clickInMenuSound.play();
                        selectedPlayers = 2;
                    }
                    else if (threePlayers.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        clickInMenuSound.play();
                        selectedPlayers = 3;
                    }
                    else if (fourPlayers.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        clickInMenuSound.play();
                        selectedPlayers = 4;
                    }
                    else if (lightBoardOption.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        clickInMenuSound.play();
                        selectedBoardTexture = "assets/lightBoard.png";
                    }
                    else if (darkBoardOption.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        clickInMenuSound.play();
                        selectedBoardTexture = "assets/darkBoard.png";
                    }
                }
            }
        }

        easyOption.setFillColor(selectedDifficulty == EASY ? sf::Color::Red : sf::Color::Black);
        mediumOption.setFillColor(selectedDifficulty == MEDIUM ? sf::Color::Red : sf::Color::Black);
        hardOption.setFillColor(selectedDifficulty == HARD ? sf::Color::Red : sf::Color::Black);
        twoPlayers.setFillColor(selectedPlayers == 2 ? sf::Color::Red : sf::Color::Black);
        threePlayers.setFillColor(selectedPlayers == 3 ? sf::Color::Red : sf::Color::Black);
        fourPlayers.setFillColor(selectedPlayers == 4 ? sf::Color::Red : sf::Color::Black);
        lightBoardOption.setFillColor(selectedBoardTexture == "assets/lightBoard.png" ? sf::Color::Red : sf::Color::Black);
        darkBoardOption.setFillColor(selectedBoardTexture == "assets/darkBoard.png" ? sf::Color::Red : sf::Color::Black);

        window.clear();
        window.draw(backgroundSprite);
        window.draw(title);
        window.draw(newGameButton);
        window.draw(rulesButton);
        window.draw(difficultyText);
        window.draw(easyOption);
        window.draw(mediumOption);
        window.draw(hardOption);
        window.draw(playersText);
        window.draw(twoPlayers);
        window.draw(threePlayers);
        window.draw(fourPlayers);
        window.draw(boardColorText);
        window.draw(lightBoardOption);
        window.draw(darkBoardOption);

        window.display();
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Snakes and Ladders");
    window.setFramerateLimit(60);

    sf::Music music;
    if (!music.openFromFile("assets/muzyka.ogg")) {
        std::cerr << "Failed to load music\n";
        return -1;
    }
    music.setLoop(true);
    music.setVolume(10);

    showMenu(window, music);

    return 0;
}
