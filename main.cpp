#include <SFML/Graphics.hpp> // Biblioteka SFML do obsługi grafiki
#include <SFML/Audio.hpp> // Biblioteka SFML do obsługi dźwięku
#include <vector> // Biblioteka STL do obsługi wektorów
#include <ctime> // Biblioteka do obsługi czasu
#include <cstdlib> // Biblioteka do obsługi funkcji losowych
#include <iostream> // Biblioteka do obsługi wejścia/wyjścia
#include <string> // Biblioteka do obsługi stringów
#include <map> // Biblioteka STL do obsługi map
#include <fstream> // Biblioteka do obsługi plików
#include <sstream> // Biblioteka do obsługi stringstream
#include <memory> // Biblioteka do obsługi wskaźników inteligentnych

// Definicja poziomów trudności
enum Difficulty { EASY, MEDIUM, HARD };
Difficulty gameDifficulty; // Zmienna globalna do przechowywania wybranego poziomu trudności

// Stałe globalne do ustawień okna gry i szybkości
const int windowWidth = 1000;
const int windowHeight = 600;
const float initialSpeed = 500.0f;
const float stopSpeed = 3.0f;
const int boardSize = 100;

// Klasa gry
class Game {
public:
    Game() {
        loadTextures(); // Ładowanie tekstur
        loadSounds(); // Ładowanie dźwięków
    }

    void run() {
        // Tworzenie okna gry
        sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Snakes and Ladders", sf::Style::Titlebar | sf::Style::Close);
        window.setFramerateLimit(60); // Ustawienie limitu klatek na sekundę

        sf::Music music; // Obiekt muzyki
        if (!music.openFromFile("assets/muzyka.ogg")) { // Ładowanie muzyki z pliku
            std::cerr << "Failed to load music\n";
            return;
        }
        music.setLoop(true); // Ustawienie pętli dla muzyki
        music.setVolume(10); // Ustawienie głośności muzyki

        showMenu(window, music); // Wyświetlenie menu gry
    }

private:
    sf::Texture diceTextures[6]; // Tablica tekstur kostki
    std::string diceImageFiles[6] = { "assets/1.png", "assets/2.png", "assets/3.png", "assets/4.png", "assets/5.png", "assets/6.png" }; // Tablica plików z obrazami kostki
    sf::Texture bonusTexture; // Tekstura bonusu
    sf::Texture logoTexture; // Tekstura logo
    sf::Sprite logoSprite; // Sprite logo

    // Bufory dźwiękowe dla różnych efektów dźwiękowych
    sf::SoundBuffer snakeHissBuffer;
    sf::SoundBuffer bonusBuffer;
    sf::SoundBuffer ladderKnockBuffer;
    sf::SoundBuffer clickInGameBuffer;
    sf::SoundBuffer diceStartBuffer;

    // Obiekty dźwiękowe
    sf::Sound snakeHissSound;
    sf::Sound bonusSound;
    sf::Sound ladderKnockSound;
    sf::Sound clickInGameSound;
    sf::Sound diceStartSound;

    // Klasa gracza
    class Player {
    public:
        Player(const sf::Color& color, const std::string& name)
            : mPosition(0), mTargetPosition(0, 0), mMoving(false), mName(name) {
            mShape.setRadius(10); // Ustawienie promienia kształtu gracza
            mShape.setFillColor(color); // Ustawienie koloru kształtu gracza
            mShape.setPosition(10, 10); // Ustawienie początkowej pozycji gracza
        }

        void draw(sf::RenderWindow& window, const sf::Vector2f& offset) const {
            sf::CircleShape shape = mShape; // Skopiowanie kształtu gracza
            shape.move(offset); // Przesunięcie kształtu o offset
            window.draw(shape); // Rysowanie kształtu gracza w oknie
        }

        void moveToPosition(int position) {
            mPosition = position; // Aktualizacja pozycji gracza
            if (mPosition >= boardSize - 1) {
                mPosition = boardSize - 1; // Ograniczenie pozycji do rozmiaru planszy
            }
            int row = mPosition / 10; // Obliczenie wiersza na planszy
            int col = mPosition % 10; // Obliczenie kolumny na planszy
            if (row % 2 == 0) {
                mTargetPosition = sf::Vector2f(col * 60 + 10, (9 - row) * 60 + 10); // Ustawienie celu dla parzystych wierszy
            }
            else {
                mTargetPosition = sf::Vector2f((9 - col) * 60 + 10, (9 - row) * 60 + 10); // Ustawienie celu dla nieparzystych wierszy
            }
            mMoving = true; // Ustawienie flagi ruchu na prawda
        }

        void update(float deltaTime) {
            if (mMoving) {
                sf::Vector2f currentPosition = mShape.getPosition(); // Pobranie aktualnej pozycji gracza
                sf::Vector2f direction = mTargetPosition - currentPosition; // Obliczenie kierunku ruchu
                float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y); // Obliczenie odległości do celu

                if (distance < 5.0f) {
                    mShape.setPosition(mTargetPosition); // Ustawienie pozycji gracza na celu
                    mShape.move(5 * (285 - mShape.getPosition().x) / std::sqrt(std::pow(285 - mShape.getPosition().x, 2) + std::pow(280 - mShape.getPosition().y, 2)), 5 * (280 - mShape.getPosition().y) / std::sqrt(std::pow(285 - mShape.getPosition().x, 2) + std::pow(280 - mShape.getPosition().y, 2))); // Ruch w kierunku celu
                    mMoving = false; // Zatrzymanie ruchu
                }
                else {
                    direction /= distance; // Normalizacja kierunku
                    mShape.move(direction * 100.0f * deltaTime); // Ruch gracza w kierunku celu
                }
            }
        }

        int getPosition() const {
            return mPosition; // Zwrócenie pozycji gracza
        }

        std::string getName() const {
            return mName; // Zwrócenie nazwy gracza
        }

        void resetPosition() {
            mPosition = 0; // Reset pozycji gracza do początkowej
            mShape.setPosition(34, 540); // Ustawienie początkowej pozycji kształtu gracza
            mTargetPosition = sf::Vector2f(33, 540); // Ustawienie początkowego celu gracza
        }

    private:
        sf::CircleShape mShape; // Kształt gracza
        sf::Vector2f mTargetPosition; // Docelowa pozycja gracza
        bool mMoving; // Flaga ruchu gracza
        int mPosition; // Pozycja gracza na planszy
        std::string mName; // Nazwa gracza
    };

    // Klasa kwadratu na planszy
    class Square {
    public:
        virtual void triggerEvent(Player& player, int& diceResult, bool& extraRoll) = 0; // Wirtualna metoda wywołująca zdarzenie
        virtual void draw(sf::RenderWindow& window) const = 0; // Wirtualna metoda rysowania kwadratu
        virtual ~Square() {} // Wirtualny destruktor
    };

    // Klasa węża na planszy
    class Snake : public Square {
    public:
        Snake(int start, int end, sf::Sound& sound) : mStart(start), mEnd(end), snakeHissSound(sound) {
            int textureIndex = rand() % 3; // Losowanie indeksu tekstury
            mTextureFile = "assets/snake" + std::to_string(textureIndex + 1) + ".png"; // Ustawienie pliku tekstury
        }
        void triggerEvent(Player& player, int& diceResult, bool& extraRoll) override {
            player.moveToPosition(mEnd); // Przesunięcie gracza na koniec węża
            snakeHissSound.play(); // Odtworzenie dźwięku węża
            std::cout << player.getName() << " landed on a snake! Sliding down to " << mEnd << ".\n"; // Informacja o zdarzeniu
        }

        void draw(sf::RenderWindow& window) const override {
            sf::Vector2f topLeft = adjustPosition(getPosition(mStart), sf::Vector2f(290, 290), 6); // Obliczenie pozycji początkowej węża
            sf::Vector2f bottomRight = adjustPosition(getPosition(mEnd), sf::Vector2f(290, 290), 6); // Obliczenie pozycji końcowej węża
            drawRectangleWithTextureAndRotation(window, topLeft, bottomRight, mTextureFile); // Rysowanie węża na planszy
        }

    private:
        int mStart; // Pozycja początkowa węża
        int mEnd; // Pozycja końcowa węża
        std::string mTextureFile; // Plik tekstury węża
        sf::Sound& snakeHissSound; // Dźwięk węża

        sf::Vector2f getPosition(int squareIndex) const {
            int row = squareIndex / 10; // Obliczenie wiersza
            int col = squareIndex % 10; // Obliczenie kolumny
            if (row % 2 == 0) {
                return sf::Vector2f(col * 60, (9 - row) * 60); // Obliczenie pozycji dla parzystych wierszy
            }
            else {
                return sf::Vector2f((9 - col) * 60, (9 - row) * 60); // Obliczenie pozycji dla nieparzystych wierszy
            }
        }
    };

    // Klasa drabiny na planszy
    class Ladder : public Square {
    public:
        Ladder(int start, int end, sf::Sound& sound) : mStart(start), mEnd(end), ladderKnockSound(sound) {
            int textureIndex = rand() % 3; // Losowanie indeksu tekstury
            mTextureFile = "assets/ladder" + std::to_string(textureIndex + 1) + ".png"; // Ustawienie pliku tekstury
        }
        void triggerEvent(Player& player, int& diceResult, bool& extraRoll) override {
            player.moveToPosition(mEnd); // Przesunięcie gracza na koniec drabiny
            ladderKnockSound.play(); // Odtworzenie dźwięku drabiny
            std::cout << player.getName() << " climbed a ladder! Moving up to " << mEnd << ".\n"; // Informacja o zdarzeniu
        }

        void draw(sf::RenderWindow& window) const override {
            sf::Vector2f topLeft = adjustPosition(getPosition(mStart), sf::Vector2f(290, 290), 14); // Obliczenie pozycji początkowej drabiny
            sf::Vector2f bottomRight = adjustPosition(getPosition(mEnd), sf::Vector2f(290, 290), 14); // Obliczenie pozycji końcowej drabiny
            drawRectangleWithTextureAndRotation(window, topLeft, bottomRight, mTextureFile); // Rysowanie drabiny na planszy
        }

    private:
        int mStart; // Pozycja początkowa drabiny
        int mEnd; // Pozycja końcowa drabiny
        std::string mTextureFile; // Plik tekstury drabiny
        sf::Sound& ladderKnockSound; // Dźwięk drabiny

        sf::Vector2f getPosition(int squareIndex) const {
            int row = squareIndex / 10; // Obliczenie wiersza
            int col = squareIndex % 10; // Obliczenie kolumny
            if (row % 2 == 0) {
                return sf::Vector2f(col * 60, (9 - row) * 60); // Obliczenie pozycji dla parzystych wierszy
            }
            else {
                return sf::Vector2f((9 - col) * 60, (9 - row) * 60); // Obliczenie pozycji dla nieparzystych wierszy
            }
        }
    };

    // Klasa bonusu na planszy
    class Bonus : public Square {
    public:
        Bonus(int position, sf::Sound& sound, sf::Texture& texture) : mPosition(position), bonusSound(sound), bonusTexture(texture) {}
        void triggerEvent(Player& player, int& diceResult, bool& extraRoll) override {
            bonusSound.play(); // Odtworzenie dźwięku bonusu
            std::cout << player.getName() << " landed on a bonus! Double roll allowed.\n"; // Informacja o zdarzeniu
            extraRoll = true; // Ustawienie flagi dodatkowego rzutu
        }

        void draw(sf::RenderWindow& window) const override {
            sf::Sprite bonusSprite(bonusTexture); // Utworzenie sprite'a bonusu
            bonusSprite.setScale(0.04f, 0.04f); // Skalowanie sprite'a
            int row = mPosition / 10; // Obliczenie wiersza
            int col = mPosition % 10; // Obliczenie kolumny
            if (row % 2 == 0) {
                bonusSprite.setPosition(col * 60 + 10, (9 - row) * 60 + 10); // Ustawienie pozycji dla parzystych wierszy
            }
            else {
                bonusSprite.setPosition((9 - col) * 60 + 10, (9 - row) * 60 + 10); // Ustawienie pozycji dla nieparzystych wierszy
            }
            sf::Vector2f targetPos(290, 290); // Pozycja docelowa
            sf::Vector2f currentPos = bonusSprite.getPosition(); // Aktualna pozycja
            sf::Vector2f direction = targetPos - currentPos; // Obliczenie kierunku ruchu
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y); // Obliczenie długości wektora

            if (length > 10) {
                direction.x /= length; // Normalizacja wektora
                direction.y /= length; // Normalizacja wektora
                sf::Vector2f moveVector(direction.x * 10, direction.y * 10); // Wektor ruchu
                bonusSprite.move(moveVector); // Przesunięcie sprite'a
            }
            window.draw(bonusSprite); // Rysowanie sprite'a bonusu w oknie
        }

    private:
        int mPosition; // Pozycja bonusu
        sf::Sound& bonusSound; // Dźwięk bonusu
        sf::Texture& bonusTexture; // Tekstura bonusu
    };

    // Klasa planszy
    class Board {
    public:
        Board(Difficulty difficulty, const std::string& textureFile, Game& game) {
            if (!mBoardTexture.loadFromFile(textureFile)) {
                std::cerr << "Failed to load board texture\n"; // Informacja o błędzie ładowania tekstury planszy
            }
            mBoardSprite.setTexture(mBoardTexture); // Ustawienie tekstury planszy
            mBoardSprite.setPosition(10, 10); // Ustawienie pozycji planszy

            mSquares.resize(boardSize); // Zmiana rozmiaru wektora kwadratów planszy
            loadFromFile(getFilenameForDifficulty(difficulty), game); // Wczytanie kwadratów z pliku
        }

        void draw(sf::RenderWindow& window) const {
            window.draw(mBoardSprite); // Rysowanie planszy w oknie
            for (const auto& square : mSquares) {
                if (square) {
                    square->draw(window); // Rysowanie kwadratów na planszy
                }
            }
        }

        void triggerEvent(Player& player, int& diceResult, bool& extraRoll) const {
            if (mSquares[player.getPosition()]) {
                if (auto snake = dynamic_cast<Snake*>(mSquares[player.getPosition()].get())) {
                    snake->triggerEvent(player, diceResult, extraRoll); // Wywołanie zdarzenia węża
                }
                else if (auto ladder = dynamic_cast<Ladder*>(mSquares[player.getPosition()].get())) {
                    ladder->triggerEvent(player, diceResult, extraRoll); // Wywołanie zdarzenia drabiny
                }
                else if (auto bonus = dynamic_cast<Bonus*>(mSquares[player.getPosition()].get())) {
                    bonus->triggerEvent(player, diceResult, extraRoll); // Wywołanie zdarzenia bonusu
                }
            }
        }

        void setBoardTexture(const std::string& textureFile) {
            if (!mBoardTexture.loadFromFile(textureFile)) {
                std::cerr << "Failed to load board texture: " << textureFile << "\n"; // Informacja o błędzie ładowania tekstury planszy
            }
            mBoardSprite.setTexture(mBoardTexture); // Ustawienie tekstury planszy
        }

        ~Board() = default; // Destruktor

    private:
        sf::Texture mBoardTexture; // Tekstura planszy
        sf::Sprite mBoardSprite; // Sprite planszy
        std::vector<std::unique_ptr<Square>> mSquares; // Wektor unikalnych wskaźników do kwadratów planszy

        std::string getFilenameForDifficulty(Difficulty difficulty) {
            switch (difficulty) {
            case EASY:
                return "assets/easy.txt"; // Plik z planszą dla łatwego poziomu
            case MEDIUM:
                return "assets/medium.txt"; // Plik z planszą dla średniego poziomu
            case HARD:
                return "assets/hard.txt"; // Plik z planszą dla trudnego poziomu
            default:
                return "assets/easy.txt"; // Domyślnie plik z planszą dla łatwego poziomu
            }
        }

        void loadFromFile(const std::string& filename, Game& game) {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error opening file: " << filename << std::endl; // Informacja o błędzie otwarcia pliku
                return;
            }

            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                int type, start, end;
                iss >> type >> start >> end;

                switch (type) {
                case 1:
                    mSquares[start] = std::make_unique<Ladder>(start, end, game.ladderKnockSound); // Utworzenie drabiny
                    break;
                case 2:
                    mSquares[start] = std::make_unique<Snake>(start, end, game.snakeHissSound); // Utworzenie węża
                    break;
                case 3:
                    mSquares[start] = std::make_unique<Bonus>(start, game.bonusSound, game.bonusTexture); // Utworzenie bonusu
                    break;
                default:
                    std::cerr << "Unknown type: " << type << std::endl; // Informacja o nieznanym typie
                    break;
                }
            }
        }
    };

    void startGame(sf::RenderWindow& window, Difficulty difficulty, int numPlayers, sf::Music& music, const std::string& boardTexture) {
        sf::Sprite diceSprite;
        diceSprite.setTexture(diceTextures[0]); // Ustawienie tekstury kostki
        diceSprite.setPosition(windowWidth / 2 - diceSprite.getGlobalBounds().width / 2, windowHeight / 2 - diceSprite.getGlobalBounds().height / 2); // Ustawienie pozycji kostki

        sf::Font font;
        font.loadFromFile("assets/arial.ttf"); // Ładowanie czcionki
        sf::Text rollButton("Rzuc kostka", font, 30); // Tekst przycisku rzutu kostką
        rollButton.setPosition(windowWidth - 200, 50); // Pozycja przycisku rzutu kostką
        rollButton.setFillColor(sf::Color::White); // Kolor tekstu przycisku rzutu kostką

        sf::Text stopButton("Stop", font, 30); // Tekst przycisku stop
        stopButton.setPosition(windowWidth - 200, 100); // Pozycja przycisku stop
        stopButton.setFillColor(sf::Color::White); // Kolor tekstu przycisku stop

        sf::Text diceValue("", font, 30); // Tekst wartości kostki
        diceValue.setPosition(windowWidth - 200, 150); // Pozycja tekstu wartości kostki
        diceValue.setFillColor(sf::Color::White); // Kolor tekstu wartości kostki

        sf::Text playerPositions("", font, 20); // Tekst pozycji graczy
        playerPositions.setPosition(windowWidth - 200, 200); // Pozycja tekstu pozycji graczy
        playerPositions.setFillColor(sf::Color::White); // Kolor tekstu pozycji graczy

        Board board(difficulty, boardTexture, *this); // Utworzenie planszy
        Player player1(sf::Color::Red, "Player 1"); // Utworzenie gracza 1
        Player player2(sf::Color::Blue, "Player 2"); // Utworzenie gracza 2
        Player player3(sf::Color::Green, "Player 3"); // Utworzenie gracza 3
        Player player4(sf::Color::Yellow, "Player 4"); // Utworzenie gracza 4

        std::vector<Player*> players = { &player1, &player2, &player3, &player4 }; // Wektor wskaźników do graczy
        players.resize(numPlayers); // Ustawienie rozmiaru wektora graczy

        for (auto& player : players) {
            player->resetPosition(); // Reset pozycji graczy
        }

        int currentPlayerIndex = 0; // Indeks aktualnego gracza

        bool isRolling = false; // Flaga rzutu kostką
        bool extraRoll = false; // Flaga dodatkowego rzutu
        float speed = initialSpeed; // Początkowa szybkość kostki
        sf::Vector2f direction(1.0f, 1.0f); // Kierunek ruchu kostki
        sf::Clock clock; // Zegar
        sf::Clock faceChangeClock; // Zegar zmiany strony kostki
        int currentFace = 0; // Aktualna strona kostki
        int diceResult = 0; // Wynik rzutu kostką

        srand(static_cast<unsigned int>(time(0))); // Inicjalizacja generatora losowego

        if (music.getStatus() != sf::Music::Playing) {
            music.play(); // Odtwarzanie muzyki
        }

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close(); // Zamknięcie okna
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if (rollButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) && !isRolling) {
                            isRolling = true; // Rozpoczęcie rzutu kostką
                            speed = initialSpeed; // Ustawienie szybkości
                            direction.x = static_cast<float>(rand() % 200 - 100) / 100.0f; // Losowanie kierunku ruchu x
                            direction.y = static_cast<float>(rand() % 200 - 100) / 100.0f; // Losowanie kierunku ruchu y
                            faceChangeClock.restart(); // Restart zegara zmiany strony kostki
                            clock.restart(); // Restart zegara
                            diceValue.setString(""); // Reset tekstu wartości kostki
                            diceStartSound.play(); // Odtworzenie dźwięku rzutu kostką
                        }
                        else if (stopButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) && isRolling) {
                            isRolling = false; // Zatrzymanie rzutu kostką
                            diceSprite.setTexture(diceTextures[currentFace]); // Ustawienie tekstury kostki
                            diceResult = currentFace + 1; // Obliczenie wyniku rzutu kostką
                            Player* currentPlayer = players[currentPlayerIndex]; // Aktualny gracz
                            currentPlayer->moveToPosition(currentPlayer->getPosition() + diceResult); // Przesunięcie gracza na nową pozycję
                            board.triggerEvent(*currentPlayer, diceResult, extraRoll); // Wywołanie zdarzenia na planszy
                            std::string imageName = diceImageFiles[currentFace]; // Nazwa pliku obrazu kostki
                            std::size_t startPos = imageName.find("assets/") + 7; // Początek nazwy pliku
                            std::size_t endPos = imageName.find(".png"); // Koniec nazwy pliku
                            imageName = imageName.substr(startPos, endPos - startPos); // Wyodrębnienie nazwy pliku
                            diceValue.setString("Wartosc: " + imageName); // Ustawienie tekstu wartości kostki
                            diceValue.setPosition(windowWidth - 200, 150); // Ustawienie pozycji tekstu wartości kostki
                            diceSprite.setPosition(666, 91); // Ustawienie pozycji sprite'a kostki
                            if (!extraRoll) {
                                if (currentPlayer->getPosition() == boardSize - 1) {
                                    displayWinMessage(window, currentPlayerIndex + 1); // Wyświetlenie komunikatu o wygranej
                                    window.close(); // Zamknięcie okna
                                    return;
                                }
                                currentPlayerIndex = (currentPlayerIndex + 1) % players.size(); // Zmiana aktualnego gracza
                            }
                            else {
                                extraRoll = false; // Reset flagi dodatkowego rzutu
                            }
                            clickInGameSound.play(); // Odtworzenie dźwięku kliknięcia w grze
                        }
                    }
                }
            }

            if (isRolling) {
                float deltaTime = clock.restart().asSeconds(); // Pobranie upływu czasu
                diceSprite.move(direction * speed * deltaTime); // Przesunięcie sprite'a kostki

                speed -= (static_cast<float>(rand() % 80 + 20)) * deltaTime; // Zmniejszenie szybkości
                if (speed <= 0) {
                    isRolling = false; // Zatrzymanie rzutu kostką
                    diceSprite.setTexture(diceTextures[currentFace]); // Ustawienie tekstury kostki
                    diceResult = currentFace + 1; // Obliczenie wyniku rzutu kostką
                    Player* currentPlayer = players[currentPlayerIndex]; // Aktualny gracz
                    currentPlayer->moveToPosition(currentPlayer->getPosition() + diceResult); // Przesunięcie gracza na nową pozycję
                    board.triggerEvent(*currentPlayer, diceResult, extraRoll); // Wywołanie zdarzenia na planszy
                    std::string imageName = diceImageFiles[currentFace]; // Nazwa pliku obrazu kostki
                    std::size_t startPos = imageName.find("assets/") + 7; // Początek nazwy pliku
                    std::size_t endPos = imageName.find(".png"); // Koniec nazwy pliku
                    imageName = imageName.substr(startPos, endPos - startPos); // Wyodrębnienie nazwy pliku
                    diceValue.setString("Wartosc: " + imageName); // Ustawienie tekstu wartości kostki
                    diceValue.setPosition(windowWidth - 200, 150); // Ustawienie pozycji tekstu wartości kostki
                    diceSprite.setPosition(666, 91); // Ustawienie pozycji sprite'a kostki
                    if (!extraRoll) {
                        if (currentPlayer->getPosition() == boardSize - 1) {
                            displayWinMessage(window, currentPlayerIndex + 1); // Wyświetlenie komunikatu o wygranej
                            window.close(); // Zamknięcie okna
                            return;
                        }
                        currentPlayerIndex = (currentPlayerIndex + 1) % players.size(); // Zmiana aktualnego gracza
                    }
                    else {
                        extraRoll = false; // Reset flagi dodatkowego rzutu
                    }
                }

                if (speed > stopSpeed) {
                    float faceChangeInterval = std::max(0.01f, 0.1f * (initialSpeed / speed)); // Obliczenie interwału zmiany strony kostki

                    if (faceChangeClock.getElapsedTime().asSeconds() >= faceChangeInterval) {
                        currentFace = rand() % 6; // Losowanie aktualnej strony kostki
                        diceSprite.setTexture(diceTextures[currentFace]); // Ustawienie tekstury kostki
                        faceChangeClock.restart(); // Restart zegara zmiany strony kostki
                    }
                }

                if (diceSprite.getPosition().x < 0 || diceSprite.getPosition().x + diceSprite.getGlobalBounds().width > windowWidth) {
                    direction.x = -direction.x; // Zmiana kierunku ruchu w osi x
                }
                if (diceSprite.getPosition().y < 0 || diceSprite.getPosition().y + diceSprite.getGlobalBounds().height > windowHeight) {
                    direction.y = -direction.y; // Zmiana kierunku ruchu w osi y
                }
            }

            float deltaTime = clock.restart().asSeconds(); // Pobranie upływu czasu
            for (auto& player : players) {
                player->update(deltaTime); // Aktualizacja pozycji graczy
            }

            std::string positions = "Player Positions:\n"; // Tekst pozycji graczy
            std::map<int, int> positionCount; // Mapa liczników pozycji
            for (const auto& player : players) {
                positionCount[player->getPosition()]++; // Zliczanie graczy na poszczególnych pozycjach
            }

            for (const auto& player : players) {
                positions += player->getName() + ": " + std::to_string(player->getPosition()) + "\n"; // Dodawanie pozycji graczy do tekstu
            }
            playerPositions.setString(positions); // Ustawienie tekstu pozycji graczy

            window.clear(); // Czyszczenie okna
            board.draw(window); // Rysowanie planszy w oknie

            std::map<int, int> drawnCount; // Mapa liczników rysowanych graczy
            for (const auto& player : players) {
                int pos = player->getPosition(); // Pozycja gracza
                sf::Vector2f offset(0.f, 0.f); // Offset pozycji gracza
                if (positionCount[pos] > 1) {
                    int count = drawnCount[pos]++; // Licznik rysowanych graczy na tej samej pozycji
                    float angle = 2 * 3.14159f * count / positionCount[pos]; // Obliczenie kąta przesunięcia gracza
                    offset = sf::Vector2f(std::cos(angle) * 10.f, std::sin(angle) * 10.f); // Obliczenie offsetu pozycji gracza
                }
                player->draw(window, offset); // Rysowanie gracza w oknie z offsetem
            }

            window.draw(logoSprite); // Rysowanie logo w oknie
            window.draw(rollButton); // Rysowanie przycisku rzutu kostką w oknie
            window.draw(stopButton); // Rysowanie przycisku stop w oknie
            window.draw(playerPositions); // Rysowanie pozycji graczy w oknie
            window.draw(diceSprite); // Rysowanie sprite'a kostki w oknie
            if (!isRolling && !diceValue.getString().isEmpty()) {
                window.draw(diceValue); // Rysowanie tekstu wartości kostki w oknie
            }
            window.display(); // Wyświetlenie zawartości okna
        }
    }

    void displayWinMessage(sf::RenderWindow& window, int playerNumber) {
        sf::Font font;
        font.loadFromFile("assets/arial.ttf"); // Ładowanie czcionki
        sf::Text winText("Gracz " + std::to_string(playerNumber) + " Wygral!", font, 50); // Tekst komunikatu o wygranej
        winText.setPosition(windowWidth / 2 - winText.getGlobalBounds().width / 2, windowHeight / 2 - winText.getGlobalBounds().height / 2); // Pozycja tekstu komunikatu o wygranej
        winText.setFillColor(sf::Color::Red); // Kolor tekstu komunikatu o wygranej

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed) {
                    window.close(); // Zamknięcie okna
                    return;
                }
            }

            window.clear(); // Czyszczenie okna
            window.draw(winText); // Rysowanie tekstu komunikatu o wygranej w oknie
            window.display(); // Wyświetlenie zawartości okna
        }
    }

    void showRules(sf::RenderWindow& window, sf::Font& font, sf::Music& music) {
        std::ifstream file("assets/zasady.txt");
        if (!file.is_open()) {
            std::cerr << "Error opening file: zasady.txt" << std::endl; // Informacja o błędzie otwarcia pliku
            return;
        }

        std::stringstream buffer;
        buffer << file.rdbuf(); // Wczytanie zawartości pliku do stringstream
        std::string rulesTextString = buffer.str(); // Konwersja zawartości pliku na string

        sf::Text rulesText(rulesTextString, font, 20); // Tekst zasad gry
        rulesText.setPosition(50, 100); // Pozycja tekstu zasad gry
        rulesText.setFillColor(sf::Color::Black); // Kolor tekstu zasad gry

        sf::Text backButton("Powrot do menu", font, 30); // Tekst przycisku powrotu do menu
        backButton.setPosition(windowWidth / 2 - backButton.getGlobalBounds().width / 2, windowHeight - 100); // Pozycja przycisku powrotu do menu
        backButton.setFillColor(sf::Color::Black); // Kolor tekstu przycisku powrotu do menu

        if (music.getStatus() != sf::Music::Playing) {
            music.play(); // Odtwarzanie muzyki
        }

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close(); // Zamknięcie okna
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                        if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            return; // Powrót do menu
                        }
                    }
                }
            }

            window.clear(sf::Color::White); // Czyszczenie okna na biało
            window.draw(rulesText); // Rysowanie tekstu zasad gry w oknie
            window.draw(backButton); // Rysowanie przycisku powrotu do menu w oknie
            window.display(); // Wyświetlenie zawartości okna
        }
    }

    void showMenu(sf::RenderWindow& window, sf::Music& music) {
        sf::Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("assets/background.png")) {
            std::cerr << "Failed to load background texture\n"; // Informacja o błędzie ładowania tekstury tła
        }
        sf::Sprite backgroundSprite;
        backgroundSprite.setTexture(backgroundTexture); // Ustawienie tekstury tła

        sf::Font font;
        font.loadFromFile("assets/arial.ttf"); // Ładowanie czcionki

        sf::Text title("Weze i drabiny", font, 50); // Tekst tytułu gry
        title.setPosition(windowWidth / 2 - title.getGlobalBounds().width / 2, 20); // Pozycja tekstu tytułu gry
        title.setFillColor(sf::Color::Black); // Kolor tekstu tytułu gry

        sf::Text newGameButton("Nowa gra", font, 30); // Tekst przycisku nowej gry
        newGameButton.setPosition(windowWidth / 2 - newGameButton.getGlobalBounds().width / 2, 100); // Pozycja przycisku nowej gry
        newGameButton.setFillColor(sf::Color::Black); // Kolor tekstu przycisku nowej gry

        sf::Text rulesButton("Zasady", font, 30); // Tekst przycisku zasad gry
        rulesButton.setPosition(windowWidth / 2 - rulesButton.getGlobalBounds().width / 2, 150); // Pozycja przycisku zasad gry
        rulesButton.setFillColor(sf::Color::Black); // Kolor tekstu przycisku zasad gry

        sf::Text difficultyText("Wybor trudnosci:", font, 20); // Tekst wyboru trudności
        difficultyText.setPosition(windowWidth / 2 - difficultyText.getGlobalBounds().width / 2, 200); // Pozycja tekstu wyboru trudności
        difficultyText.setFillColor(sf::Color::Black); // Kolor tekstu wyboru trudności

        sf::Text easyOption("Latwy", font, 20); // Tekst opcji łatwego poziomu
        easyOption.setPosition(windowWidth / 2 - 110, 250); // Pozycja tekstu łatwego poziomu

        sf::Text mediumOption("Sredni", font, 20); // Tekst opcji średniego poziomu
        mediumOption.setPosition(windowWidth / 2 - 30, 250); // Pozycja tekstu średniego poziomu

        sf::Text hardOption("Trudny", font, 20); // Tekst opcji trudnego poziomu
        hardOption.setPosition(windowWidth / 2 + 50, 250); // Pozycja tekstu trudnego poziomu

        sf::Text playersText("Ilosc graczy", font, 20); // Tekst ilości graczy
        playersText.setPosition(windowWidth / 2 - playersText.getGlobalBounds().width / 2, 300); // Pozycja tekstu ilości graczy
        playersText.setFillColor(sf::Color::Black); // Kolor tekstu ilości graczy

        sf::Text twoPlayers("2", font, 20); // Tekst opcji dwóch graczy
        twoPlayers.setPosition(windowWidth / 2 - 50, 350); // Pozycja tekstu dwóch graczy

        sf::Text threePlayers("3", font, 20); // Tekst opcji trzech graczy
        threePlayers.setPosition(windowWidth / 2 - 10, 350); // Pozycja tekstu trzech graczy

        sf::Text fourPlayers("4", font, 20); // Tekst opcji czterech graczy
        fourPlayers.setPosition(windowWidth / 2 + 30, 350); // Pozycja tekstu czterech graczy

        sf::Text boardColorText("Kolor Planszy", font, 20); // Tekst wyboru koloru planszy
        boardColorText.setPosition(windowWidth / 2 - boardColorText.getGlobalBounds().width / 2, 400); // Pozycja tekstu wyboru koloru planszy
        boardColorText.setFillColor(sf::Color::Black); // Kolor tekstu wyboru koloru planszy

        sf::Text lightBoardOption("Jasny", font, 20); // Tekst opcji jasnego koloru planszy
        lightBoardOption.setPosition(windowWidth / 2 - 60, 450); // Pozycja tekstu jasnego koloru planszy

        sf::Text darkBoardOption("Ciemny", font, 20); // Tekst opcji ciemnego koloru planszy
        darkBoardOption.setPosition(windowWidth / 2 + 0, 450); // Pozycja tekstu ciemnego koloru planszy

        Difficulty selectedDifficulty = EASY; // Wybrany poziom trudności
        int selectedPlayers = 2; // Wybrana ilość graczy
        std::string selectedBoardTexture = "assets/lightBoard.png"; // Wybrana tekstura planszy

        if (music.getStatus() != sf::Music::Playing) {
            music.play(); // Odtwarzanie muzyki
        }

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close(); // Zamknięcie okna
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                        if (newGameButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            startGame(window, selectedDifficulty, selectedPlayers, music, selectedBoardTexture); // Rozpoczęcie nowej gry
                        }
                        else if (rulesButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            showRules(window, font, music); // Wyświetlenie zasad gry
                        }
                        else if (easyOption.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            selectedDifficulty = EASY; // Wybranie łatwego poziomu trudności
                        }
                        else if (mediumOption.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            selectedDifficulty = MEDIUM; // Wybranie średniego poziomu trudności
                        }
                        else if (hardOption.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            selectedDifficulty = HARD; // Wybranie trudnego poziomu trudności
                        }
                        else if (twoPlayers.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            selectedPlayers = 2; // Wybranie dwóch graczy
                        }
                        else if (threePlayers.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            selectedPlayers = 3; // Wybranie trzech graczy
                        }
                        else if (fourPlayers.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            selectedPlayers = 4; // Wybranie czterech graczy
                        }
                        else if (lightBoardOption.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            selectedBoardTexture = "assets/lightBoard.png"; // Wybranie jasnego koloru planszy
                        }
                        else if (darkBoardOption.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            selectedBoardTexture = "assets/darkBoard.png"; // Wybranie ciemnego koloru planszy
                        }
                    }
                }
            }

            easyOption.setFillColor(selectedDifficulty == EASY ? sf::Color::Red : sf::Color::Black); // Ustawienie koloru tekstu łatwego poziomu trudności
            mediumOption.setFillColor(selectedDifficulty == MEDIUM ? sf::Color::Red : sf::Color::Black); // Ustawienie koloru tekstu średniego poziomu trudności
            hardOption.setFillColor(selectedDifficulty == HARD ? sf::Color::Red : sf::Color::Black); // Ustawienie koloru tekstu trudnego poziomu trudności
            twoPlayers.setFillColor(selectedPlayers == 2 ? sf::Color::Red : sf::Color::Black); // Ustawienie koloru tekstu opcji dwóch graczy
            threePlayers.setFillColor(selectedPlayers == 3 ? sf::Color::Red : sf::Color::Black); // Ustawienie koloru tekstu opcji trzech graczy
            fourPlayers.setFillColor(selectedPlayers == 4 ? sf::Color::Red : sf::Color::Black); // Ustawienie koloru tekstu opcji czterech graczy
            lightBoardOption.setFillColor(selectedBoardTexture == "assets/lightBoard.png" ? sf::Color::Red : sf::Color::Black); // Ustawienie koloru tekstu jasnego koloru planszy
            darkBoardOption.setFillColor(selectedBoardTexture == "assets/darkBoard.png" ? sf::Color::Red : sf::Color::Black); // Ustawienie koloru tekstu ciemnego koloru planszy

            window.clear(); // Czyszczenie okna
            window.draw(backgroundSprite); // Rysowanie tła w oknie
            window.draw(title); // Rysowanie tytułu gry w oknie
            window.draw(newGameButton); // Rysowanie przycisku nowej gry w oknie
            window.draw(rulesButton); // Rysowanie przycisku zasad gry w oknie
            window.draw(difficultyText); // Rysowanie tekstu wyboru trudności w oknie
            window.draw(easyOption); // Rysowanie opcji łatwego poziomu w oknie
            window.draw(mediumOption); // Rysowanie opcji średniego poziomu w oknie
            window.draw(hardOption); // Rysowanie opcji trudnego poziomu w oknie
            window.draw(playersText); // Rysowanie tekstu ilości graczy w oknie
            window.draw(twoPlayers); // Rysowanie opcji dwóch graczy w oknie
            window.draw(threePlayers); // Rysowanie opcji trzech graczy w oknie
            window.draw(fourPlayers); // Rysowanie opcji czterech graczy w oknie
            window.draw(boardColorText); // Rysowanie tekstu wyboru koloru planszy w oknie
            window.draw(lightBoardOption); // Rysowanie opcji jasnego koloru planszy w oknie
            window.draw(darkBoardOption); // Rysowanie opcji ciemnego koloru planszy w oknie

            window.display(); // Wyświetlenie zawartości okna
        }
    }

    static sf::Vector2f adjustPosition(const sf::Vector2f& originalPosition, const sf::Vector2f& target, float offset) {
        sf::Vector2f direction = target - originalPosition; // Obliczenie kierunku ruchu
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y); // Obliczenie długości wektora
        direction /= length; // Normalizacja wektora
        return originalPosition + direction * offset; // Zwrócenie przesuniętej pozycji
    }

    static void drawRectangleWithTextureAndRotation(sf::RenderWindow& window, const sf::Vector2f& topLeft, const sf::Vector2f& bottomRight, const std::string& textureFile) {
        sf::Texture texture;
        if (!texture.loadFromFile(textureFile)) {
            return; // Informacja o błędzie ładowania tekstury
        }

        float length = std::sqrt(std::pow(bottomRight.x - topLeft.x, 2) + std::pow(bottomRight.y - topLeft.y, 2)); // Obliczenie długości prostokąta
        float width = 50; // Szerokość prostokąta

        sf::RectangleShape rectangle(sf::Vector2f(length, width)); // Utworzenie prostokąta
        rectangle.setPosition(topLeft.x + 10, topLeft.y + 10); // Ustawienie pozycji prostokąta
        rectangle.setTexture(&texture); // Ustawienie tekstury prostokąta

        float dx = bottomRight.x - topLeft.x; // Różnica w osi x
        float dy = bottomRight.y - topLeft.y; // Różnica w osi y
        float angle = std::atan2(dy, dx) * 180 / 3.14159f; // Obliczenie kąta obrotu

        rectangle.setOrigin(0, width / 2); // Ustawienie punktu początkowego prostokąta
        rectangle.setRotation(angle); // Ustawienie kąta obrotu prostokąta

        window.draw(rectangle); // Rysowanie prostokąta w oknie
    }

    void loadTextures() {
        for (int i = 0; i < 6; ++i) {
            diceTextures[i].loadFromFile(diceImageFiles[i]); // Ładowanie tekstur kostki
        }
        if (!bonusTexture.loadFromFile("assets/extraroll.png")) {
            std::cerr << "Failed to load bonus texture\n"; // Informacja o błędzie ładowania tekstury bonusu
        }
        if (!logoTexture.loadFromFile("assets/coolLogo.png")) {
            std::cerr << "Failed to load logo texture\n"; // Informacja o błędzie ładowania tekstury logo
        }
        else {
            logoSprite.setTexture(logoTexture); // Ustawienie tekstury logo
            logoSprite.setPosition(604, 338); // Ustawienie pozycji logo
            logoSprite.setScale(0.38f, 0.27f); // Skalowanie logo
        }
    }

    void loadSounds() {
        if (!snakeHissBuffer.loadFromFile("assets/snakeHiss.ogg")) {
            std::cerr << "Failed to load snake sound\n"; // Informacja o błędzie ładowania dźwięku węża
        }
        if (!bonusBuffer.loadFromFile("assets/bonus.ogg")) {
            std::cerr << "Failed to load bonus sound\n"; // Informacja o błędzie ładowania dźwięku bonusu
        }
        if (!ladderKnockBuffer.loadFromFile("assets/ladderKnock.ogg")) {
            std::cerr << "Failed to load ladder sound\n"; // Informacja o błędzie ładowania dźwięku drabiny
        }
        if (!clickInGameBuffer.loadFromFile("assets/clickInGame.ogg")) {
            std::cerr << "Failed to load click sound\n"; // Informacja o błędzie ładowania dźwięku kliknięcia
        }
        if (!diceStartBuffer.loadFromFile("assets/diceStart.ogg")) {
            std::cerr << "Failed to load dice start sound\n"; // Informacja o błędzie ładowania dźwięku rzutu kostką
        }

        snakeHissSound.setBuffer(snakeHissBuffer); // Ustawienie bufora dźwięku węża
        bonusSound.setBuffer(bonusBuffer); // Ustawienie bufora dźwięku bonusu
        ladderKnockSound.setBuffer(ladderKnockBuffer); // Ustawienie bufora dźwięku drabiny
        clickInGameSound.setBuffer(clickInGameBuffer); // Ustawienie bufora dźwięku kliknięcia
        diceStartSound.setBuffer(diceStartBuffer); // Ustawienie bufora dźwięku rzutu kostką
    }
};

int main() {
    Game game; // Utworzenie obiektu gry
    game.run(); // Uruchomienie gry
    return 0; // Zakończenie programu
}
