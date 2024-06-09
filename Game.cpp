#include "Game.h"
#include "Difficulty.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cmath>
#include <map>

// Definicja zmiennych globalnych
const int windowWidth = 1000;
const int windowHeight = 600;
const float initialSpeed = 500.0f;
const float stopSpeed = 3.0f;
const int boardSize = 100;

Game::Game() {
    loadTextures(); // �adowanie tekstur
    loadSounds(); // �adowanie d�wi�k�w
}

void Game::run() {
    // Tworzenie okna gry
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Snakes and Ladders", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60); // Ustawienie limitu klatek na sekund�

    sf::Music music; // Obiekt muzyki
    if (!music.openFromFile("assets/muzyka.ogg")) { // �adowanie muzyki z pliku
        std::cerr << "Failed to load music\n";
        return;
    }
    music.setLoop(true); // Ustawienie p�tli dla muzyki
    music.setVolume(10); // Ustawienie g�o�no�ci muzyki

    showMenu(window, music); // Wy�wietlenie menu gry
}

void Game::startGame(sf::RenderWindow& window, Difficulty difficulty, int numPlayers, sf::Music& music, const std::string& boardTexture) {
    sf::Sprite diceSprite;
    diceSprite.setTexture(diceTextures[0]); // Ustawienie tekstury kostki
    diceSprite.setPosition(windowWidth / 2 - diceSprite.getGlobalBounds().width / 2, windowHeight / 2 - diceSprite.getGlobalBounds().height / 2); // Ustawienie pozycji kostki

    sf::Font font;
    font.loadFromFile("assets/arial.ttf"); // �adowanie czcionki
    sf::Text rollButton("Rzuc kostka", font, 30); // Tekst przycisku rzutu kostk�
    rollButton.setPosition(windowWidth - 200, 50); // Pozycja przycisku rzutu kostk�
    rollButton.setFillColor(sf::Color::White); // Kolor tekstu przycisku rzutu kostk�

    sf::Text stopButton("Stop", font, 30); // Tekst przycisku stop
    stopButton.setPosition(windowWidth - 200, 100); // Pozycja przycisku stop
    stopButton.setFillColor(sf::Color::White); // Kolor tekstu przycisku stop

    sf::Text diceValue("", font, 30); // Tekst warto�ci kostki
    diceValue.setPosition(windowWidth - 200, 150); // Pozycja tekstu warto�ci kostki
    diceValue.setFillColor(sf::Color::White); // Kolor tekstu warto�ci kostki

    sf::Text playerPositions("", font, 20); // Tekst pozycji graczy
    playerPositions.setPosition(windowWidth - 200, 200); // Pozycja tekstu pozycji graczy
    playerPositions.setFillColor(sf::Color::White); // Kolor tekstu pozycji graczy

    Board board(difficulty, boardTexture, *this); // Utworzenie planszy
    Player player1(sf::Color::Red, "Player 1"); // Utworzenie gracza 1
    Player player2(sf::Color::Blue, "Player 2"); // Utworzenie gracza 2
    Player player3(sf::Color::Green, "Player 3"); // Utworzenie gracza 3
    Player player4(sf::Color::Yellow, "Player 4"); // Utworzenie gracza 4

    std::vector<Player*> players = { &player1, &player2, &player3, &player4 }; // Wektor wska�nik�w do graczy
    players.resize(numPlayers); // Ustawienie rozmiaru wektora graczy

    for (auto& player : players) {
        player->resetPosition(); // Reset pozycji graczy
    }

    int currentPlayerIndex = 0; // Indeks aktualnego gracza

    bool isRolling = false; // Flaga rzutu kostk�
    bool extraRoll = false; // Flaga dodatkowego rzutu
    float speed = initialSpeed; // Pocz�tkowa szybko�� kostki
    sf::Vector2f direction(1.0f, 1.0f); // Kierunek ruchu kostki
    sf::Clock clock; // Zegar
    sf::Clock faceChangeClock; // Zegar zmiany strony kostki
    int currentFace = 0; // Aktualna strona kostki
    int diceResult = 0; // Wynik rzutu kostk�

    srand(static_cast<unsigned int>(time(0))); // Inicjalizacja generatora losowego

    if (music.getStatus() != sf::Music::Playing) {
        music.play(); // Odtwarzanie muzyki
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close(); // Zamkni�cie okna
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (rollButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) && !isRolling) {
                        isRolling = true; // Rozpocz�cie rzutu kostk�
                        speed = initialSpeed; // Ustawienie szybko�ci
                        direction.x = static_cast<float>(rand() % 200 - 100) / 100.0f; // Losowanie kierunku ruchu x
                        direction.y = static_cast<float>(rand() % 200 - 100) / 100.0f; // Losowanie kierunku ruchu y
                        faceChangeClock.restart(); // Restart zegara zmiany strony kostki
                        clock.restart(); // Restart zegara
                        diceValue.setString(""); // Reset tekstu warto�ci kostki
                        diceStartSound.play(); // Odtworzenie d�wi�ku rzutu kostk�
                    }
                    else if (stopButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) && isRolling) {
                        isRolling = false; // Zatrzymanie rzutu kostk�
                        diceSprite.setTexture(diceTextures[currentFace]); // Ustawienie tekstury kostki
                        diceResult = currentFace + 1; // Obliczenie wyniku rzutu kostk�
                        Player* currentPlayer = players[currentPlayerIndex]; // Aktualny gracz
                        currentPlayer->moveToPosition(currentPlayer->getPosition() + diceResult); // Przesuni�cie gracza na now� pozycj�
                        board.triggerEvent(*currentPlayer, diceResult, extraRoll); // Wywo�anie zdarzenia na planszy
                        std::string imageName = diceImageFiles[currentFace]; // Nazwa pliku obrazu kostki
                        std::size_t startPos = imageName.find("assets/") + 7; // Pocz�tek nazwy pliku
                        std::size_t endPos = imageName.find(".png"); // Koniec nazwy pliku
                        imageName = imageName.substr(startPos, endPos - startPos); // Wyodr�bnienie nazwy pliku
                        diceValue.setString("Wartosc: " + imageName); // Ustawienie tekstu warto�ci kostki
                        diceValue.setPosition(windowWidth - 200, 150); // Ustawienie pozycji tekstu warto�ci kostki
                        diceSprite.setPosition(666, 91); // Ustawienie pozycji sprite'a kostki
                        if (!extraRoll) {
                            if (currentPlayer->getPosition() == boardSize - 1) {
                                displayWinMessage(window, currentPlayerIndex + 1); // Wy�wietlenie komunikatu o wygranej
                                window.close(); // Zamkni�cie okna
                                return;
                            }
                            currentPlayerIndex = (currentPlayerIndex + 1) % players.size(); // Zmiana aktualnego gracza
                        }
                        else {
                            extraRoll = false; // Reset flagi dodatkowego rzutu
                        }
                        clickInGameSound.play(); // Odtworzenie d�wi�ku klikni�cia w grze
                    }
                }
            }
        }

        if (isRolling) {
            float deltaTime = clock.restart().asSeconds(); // Pobranie up�ywu czasu
            diceSprite.move(direction * speed * deltaTime); // Przesuni�cie sprite'a kostki

            speed -= (static_cast<float>(rand() % 80 + 20)) * deltaTime; // Zmniejszenie szybko�ci
            if (speed <= 0) {
                isRolling = false; // Zatrzymanie rzutu kostk�
                diceSprite.setTexture(diceTextures[currentFace]); // Ustawienie tekstury kostki
                diceResult = currentFace + 1; // Obliczenie wyniku rzutu kostk�
                Player* currentPlayer = players[currentPlayerIndex]; // Aktualny gracz
                currentPlayer->moveToPosition(currentPlayer->getPosition() + diceResult); // Przesuni�cie gracza na now� pozycj�
                board.triggerEvent(*currentPlayer, diceResult, extraRoll); // Wywo�anie zdarzenia na planszy
                std::string imageName = diceImageFiles[currentFace]; // Nazwa pliku obrazu kostki
                std::size_t startPos = imageName.find("assets/") + 7; // Pocz�tek nazwy pliku
                std::size_t endPos = imageName.find(".png"); // Koniec nazwy pliku
                imageName = imageName.substr(startPos, endPos - startPos); // Wyodr�bnienie nazwy pliku
                diceValue.setString("Wartosc: " + imageName); // Ustawienie tekstu warto�ci kostki
                diceValue.setPosition(windowWidth - 200, 150); // Ustawienie pozycji tekstu warto�ci kostki
                diceSprite.setPosition(666, 91); // Ustawienie pozycji sprite'a kostki
                if (!extraRoll) {
                    if (currentPlayer->getPosition() == boardSize - 1) {
                        displayWinMessage(window, currentPlayerIndex + 1); // Wy�wietlenie komunikatu o wygranej
                        window.close(); // Zamkni�cie okna
                        return;
                    }
                    currentPlayerIndex = (currentPlayerIndex + 1) % players.size(); // Zmiana aktualnego gracza
                }
                else {
                    extraRoll = false; // Reset flagi dodatkowego rzutu
                }
            }

            if (speed > stopSpeed) {
                float faceChangeInterval = std::max(0.01f, 0.1f * (initialSpeed / speed)); // Obliczenie interwa�u zmiany strony kostki

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

        float deltaTime = clock.restart().asSeconds(); // Pobranie up�ywu czasu
        for (auto& player : players) {
            player->update(deltaTime); // Aktualizacja pozycji graczy
        }

        std::string positions = "Player Positions:\n"; // Tekst pozycji graczy
        std::map<int, int> positionCount; // Mapa licznik�w pozycji
        for (const auto& player : players) {
            positionCount[player->getPosition()]++; // Zliczanie graczy na poszczeg�lnych pozycjach
        }

        for (const auto& player : players) {
            positions += player->getName() + ": " + std::to_string(player->getPosition()) + "\n"; // Dodawanie pozycji graczy do tekstu
        }
        playerPositions.setString(positions); // Ustawienie tekstu pozycji graczy

        window.clear(); // Czyszczenie okna
        board.draw(window); // Rysowanie planszy w oknie

        std::map<int, int> drawnCount; // Mapa licznik�w rysowanych graczy
        for (const auto& player : players) {
            int pos = player->getPosition(); // Pozycja gracza
            sf::Vector2f offset(0.f, 0.f); // Offset pozycji gracza
            if (positionCount[pos] > 1) {
                int count = drawnCount[pos]++; // Licznik rysowanych graczy na tej samej pozycji
                float angle = 2 * 3.14159f * count / positionCount[pos]; // Obliczenie k�ta przesuni�cia gracza
                offset = sf::Vector2f(std::cos(angle) * 10.f, std::sin(angle) * 10.f); // Obliczenie offsetu pozycji gracza
            }
            player->draw(window, offset); // Rysowanie gracza w oknie z offsetem
        }

        window.draw(logoSprite); // Rysowanie logo w oknie
        window.draw(rollButton); // Rysowanie przycisku rzutu kostk� w oknie
        window.draw(stopButton); // Rysowanie przycisku stop w oknie
        window.draw(playerPositions); // Rysowanie pozycji graczy w oknie
        window.draw(diceSprite); // Rysowanie sprite'a kostki w oknie
        if (!isRolling && !diceValue.getString().isEmpty()) {
            window.draw(diceValue); // Rysowanie tekstu warto�ci kostki w oknie
        }
        window.display(); // Wy�wietlenie zawarto�ci okna
    }
}

void Game::displayWinMessage(sf::RenderWindow& window, int playerNumber) {
    sf::Font font;
    font.loadFromFile("assets/arial.ttf"); // �adowanie czcionki
    sf::Text winText("Gracz " + std::to_string(playerNumber) + " Wygral!", font, 50); // Tekst komunikatu o wygranej
    winText.setPosition(windowWidth / 2 - winText.getGlobalBounds().width / 2, windowHeight / 2 - winText.getGlobalBounds().height / 2); // Pozycja tekstu komunikatu o wygranej
    winText.setFillColor(sf::Color::Red); // Kolor tekstu komunikatu o wygranej

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed) {
                window.close(); // Zamkni�cie okna
                return;
            }
        }

        window.clear(); // Czyszczenie okna
        window.draw(winText); // Rysowanie tekstu komunikatu o wygranej w oknie
        window.display(); // Wy�wietlenie zawarto�ci okna
    }
}

void Game::showRules(sf::RenderWindow& window, sf::Font& font, sf::Music& music) {
    std::ifstream file("assets/zasady.txt");
    if (!file.is_open()) {
        std::cerr << "Error opening file: zasady.txt" << std::endl; // Informacja o b��dzie otwarcia pliku
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf(); // Wczytanie zawarto�ci pliku do stringstream
    std::string rulesTextString = buffer.str(); // Konwersja zawarto�ci pliku na string

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
                window.close(); // Zamkni�cie okna
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        return; // Powr�t do menu
                    }
                }
            }
        }

        window.clear(sf::Color::White); // Czyszczenie okna na bia�o
        window.draw(rulesText); // Rysowanie tekstu zasad gry w oknie
        window.draw(backButton); // Rysowanie przycisku powrotu do menu w oknie
        window.display(); // Wy�wietlenie zawarto�ci okna
    }
}

void Game::showMenu(sf::RenderWindow& window, sf::Music& music) {
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("assets/background.png")) {
        std::cerr << "Failed to load background texture\n"; // Informacja o b��dzie �adowania tekstury t�a
    }
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture); // Ustawienie tekstury t�a

    sf::Font font;
    font.loadFromFile("assets/arial.ttf"); // �adowanie czcionki

    sf::Text title("Weze i drabiny", font, 50); // Tekst tytu�u gry
    title.setPosition(windowWidth / 2 - title.getGlobalBounds().width / 2, 20); // Pozycja tekstu tytu�u gry
    title.setFillColor(sf::Color::Black); // Kolor tekstu tytu�u gry

    sf::Text newGameButton("Nowa gra", font, 30); // Tekst przycisku nowej gry
    newGameButton.setPosition(windowWidth / 2 - newGameButton.getGlobalBounds().width / 2, 100); // Pozycja przycisku nowej gry
    newGameButton.setFillColor(sf::Color::Black); // Kolor tekstu przycisku nowej gry

    sf::Text rulesButton("Zasady", font, 30); // Tekst przycisku zasad gry
    rulesButton.setPosition(windowWidth / 2 - rulesButton.getGlobalBounds().width / 2, 150); // Pozycja przycisku zasad gry
    rulesButton.setFillColor(sf::Color::Black); // Kolor tekstu przycisku zasad gry

    sf::Text difficultyText("Wybor trudnosci:", font, 20); // Tekst wyboru trudno�ci
    difficultyText.setPosition(windowWidth / 2 - difficultyText.getGlobalBounds().width / 2, 200); // Pozycja tekstu wyboru trudno�ci
    difficultyText.setFillColor(sf::Color::Black); // Kolor tekstu wyboru trudno�ci

    sf::Text easyOption("Latwy", font, 20); // Tekst opcji �atwego poziomu
    easyOption.setPosition(windowWidth / 2 - 110, 250); // Pozycja tekstu �atwego poziomu

    sf::Text mediumOption("Sredni", font, 20); // Tekst opcji �redniego poziomu
    mediumOption.setPosition(windowWidth / 2 - 30, 250); // Pozycja tekstu �redniego poziomu

    sf::Text hardOption("Trudny", font, 20); // Tekst opcji trudnego poziomu
    hardOption.setPosition(windowWidth / 2 + 50, 250); // Pozycja tekstu trudnego poziomu

    sf::Text playersText("Ilosc graczy", font, 20); // Tekst ilo�ci graczy
    playersText.setPosition(windowWidth / 2 - playersText.getGlobalBounds().width / 2, 300); // Pozycja tekstu ilo�ci graczy
    playersText.setFillColor(sf::Color::Black); // Kolor tekstu ilo�ci graczy

    sf::Text twoPlayers("2", font, 20); // Tekst opcji dw�ch graczy
    twoPlayers.setPosition(windowWidth / 2 - 50, 350); // Pozycja tekstu dw�ch graczy

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

    Difficulty selectedDifficulty = EASY; // Wybrany poziom trudno�ci
    int selectedPlayers = 2; // Wybrana ilo�� graczy
    std::string selectedBoardTexture = "assets/lightBoard.png"; // Wybrana tekstura planszy

    if (music.getStatus() != sf::Music::Playing) {
        music.play(); // Odtwarzanie muzyki
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close(); // Zamkni�cie okna
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    if (newGameButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        startGame(window, selectedDifficulty, selectedPlayers, music, selectedBoardTexture); // Rozpocz�cie nowej gry
                    }
                    else if (rulesButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        showRules(window, font, music); // Wy�wietlenie zasad gry
                    }
                    else if (easyOption.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        selectedDifficulty = EASY; // Wybranie �atwego poziomu trudno�ci
                    }
                    else if (mediumOption.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        selectedDifficulty = MEDIUM; // Wybranie �redniego poziomu trudno�ci
                    }
                    else if (hardOption.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        selectedDifficulty = HARD; // Wybranie trudnego poziomu trudno�ci
                    }
                    else if (twoPlayers.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        selectedPlayers = 2; // Wybranie dw�ch graczy
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

        easyOption.setFillColor(selectedDifficulty == EASY ? sf::Color::Red : sf::Color::Black); // Ustawienie koloru tekstu �atwego poziomu trudno�ci
        mediumOption.setFillColor(selectedDifficulty == MEDIUM ? sf::Color::Red : sf::Color::Black); // Ustawienie koloru tekstu �redniego poziomu trudno�ci
        hardOption.setFillColor(selectedDifficulty == HARD ? sf::Color::Red : sf::Color::Black); // Ustawienie koloru tekstu trudnego poziomu trudno�ci
        twoPlayers.setFillColor(selectedPlayers == 2 ? sf::Color::Red : sf::Color::Black); // Ustawienie koloru tekstu opcji dw�ch graczy
        threePlayers.setFillColor(selectedPlayers == 3 ? sf::Color::Red : sf::Color::Black); // Ustawienie koloru tekstu opcji trzech graczy
        fourPlayers.setFillColor(selectedPlayers == 4 ? sf::Color::Red : sf::Color::Black); // Ustawienie koloru tekstu opcji czterech graczy
        lightBoardOption.setFillColor(selectedBoardTexture == "assets/lightBoard.png" ? sf::Color::Red : sf::Color::Black); // Ustawienie koloru tekstu jasnego koloru planszy
        darkBoardOption.setFillColor(selectedBoardTexture == "assets/darkBoard.png" ? sf::Color::Red : sf::Color::Black); // Ustawienie koloru tekstu ciemnego koloru planszy

        window.clear(); // Czyszczenie okna
        window.draw(backgroundSprite); // Rysowanie t�a w oknie
        window.draw(title); // Rysowanie tytu�u gry w oknie
        window.draw(newGameButton); // Rysowanie przycisku nowej gry w oknie
        window.draw(rulesButton); // Rysowanie przycisku zasad gry w oknie
        window.draw(difficultyText); // Rysowanie tekstu wyboru trudno�ci w oknie
        window.draw(easyOption); // Rysowanie opcji �atwego poziomu w oknie
        window.draw(mediumOption); // Rysowanie opcji �redniego poziomu w oknie
        window.draw(hardOption); // Rysowanie opcji trudnego poziomu w oknie
        window.draw(playersText); // Rysowanie tekstu ilo�ci graczy w oknie
        window.draw(twoPlayers); // Rysowanie opcji dw�ch graczy w oknie
        window.draw(threePlayers); // Rysowanie opcji trzech graczy w oknie
        window.draw(fourPlayers); // Rysowanie opcji czterech graczy w oknie
        window.draw(boardColorText); // Rysowanie tekstu wyboru koloru planszy w oknie
        window.draw(lightBoardOption); // Rysowanie opcji jasnego koloru planszy w oknie
        window.draw(darkBoardOption); // Rysowanie opcji ciemnego koloru planszy w oknie

        window.display(); // Wy�wietlenie zawarto�ci okna
    }
}

void Game::loadTextures() {
    for (int i = 0; i < 6; ++i) {
        diceTextures[i].loadFromFile(diceImageFiles[i]); // �adowanie tekstur kostki
    }
    if (!bonusTexture.loadFromFile("assets/extraroll.png")) {
        std::cerr << "Failed to load bonus texture\n"; // Informacja o b��dzie �adowania tekstury bonusu
    }
    if (!logoTexture.loadFromFile("assets/coolLogo.png")) {
        std::cerr << "Failed to load logo texture\n"; // Informacja o b��dzie �adowania tekstury logo
    }
    else {
        logoSprite.setTexture(logoTexture); // Ustawienie tekstury logo
        logoSprite.setPosition(604, 338); // Ustawienie pozycji logo
        logoSprite.setScale(0.38f, 0.27f); // Skalowanie logo
    }
}

void Game::loadSounds() {
    if (!snakeHissBuffer.loadFromFile("assets/snakeHiss.ogg")) {
        std::cerr << "Failed to load snake sound\n"; // Informacja o b��dzie �adowania d�wi�ku w�a
    }
    if (!bonusBuffer.loadFromFile("assets/bonus.ogg")) {
        std::cerr << "Failed to load bonus sound\n"; // Informacja o b��dzie �adowania d�wi�ku bonusu
    }
    if (!ladderKnockBuffer.loadFromFile("assets/ladderKnock.ogg")) {
        std::cerr << "Failed to load ladder sound\n"; // Informacja o b��dzie �adowania d�wi�ku drabiny
    }
    if (!clickInGameBuffer.loadFromFile("assets/clickInGame.ogg")) {
        std::cerr << "Failed to load click sound\n"; // Informacja o b��dzie �adowania d�wi�ku klikni�cia
    }
    if (!diceStartBuffer.loadFromFile("assets/diceStart.ogg")) {
        std::cerr << "Failed to load dice start sound\n"; // Informacja o b��dzie �adowania d�wi�ku rzutu kostk�
    }

    snakeHissSound.setBuffer(snakeHissBuffer); // Ustawienie bufora d�wi�ku w�a
    bonusSound.setBuffer(bonusBuffer); // Ustawienie bufora d�wi�ku bonusu
    ladderKnockSound.setBuffer(ladderKnockBuffer); // Ustawienie bufora d�wi�ku drabiny
    clickInGameSound.setBuffer(clickInGameBuffer); // Ustawienie bufora d�wi�ku klikni�cia
    diceStartSound.setBuffer(diceStartBuffer); // Ustawienie bufora d�wi�ku rzutu kostk�
}
