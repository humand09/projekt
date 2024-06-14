#include "Board.h"
#include <iostream>
#include <fstream>
#include <sstream>

Board::Board(Difficulty difficulty, const std::string& textureFile, Game& game) {
    if (!mBoardTexture.loadFromFile(textureFile)) {
        std::cerr << "Failed to load board texture\n"; // Informacja o b³êdzie ³adowania tekstury planszy
    }
    mBoardSprite.setTexture(mBoardTexture); // Ustawienie tekstury planszy
    mBoardSprite.setPosition(10, 10); // Ustawienie pozycji planszy

    mSquares.resize(boardSize); // Zmiana rozmiaru wektora kwadratów planszy
    loadFromFile(getFilenameForDifficulty(difficulty), game); // Wczytanie kwadratów z pliku
}

void Board::draw(sf::RenderWindow& window) const {
    window.draw(mBoardSprite); // Rysowanie planszy w oknie
    for (const auto& square : mSquares) {
        if (square) {
            // Dynamiczne rzutowanie i specyficzne rysowanie ró¿nych typów pól
            if (auto snake = dynamic_cast<Snake*>(square.get())) {
                snake->draw(window); // Rysowanie wê¿a
            }
            else if (auto ladder = dynamic_cast<Ladder*>(square.get())) {
                ladder->draw(window); // Rysowanie drabiny
            }
            else if (auto bonus = dynamic_cast<Bonus*>(square.get())) {
                bonus->draw(window); // Rysowanie bonusu
            }
            else {
                square->draw(window); // Rysowanie innych pól (jeœli istniej¹)
            }
        }
    }
}

void Board::triggerEvent(Player& player, int& diceResult, bool& extraRoll) const {
    if (mSquares[player.getPosition()]) {
        if (auto snake = dynamic_cast<Snake*>(mSquares[player.getPosition()].get())) {
            snake->triggerEvent(player, diceResult, extraRoll); // Wywo³anie zdarzenia wê¿a
        }
        else if (auto ladder = dynamic_cast<Ladder*>(mSquares[player.getPosition()].get())) {
            ladder->triggerEvent(player, diceResult, extraRoll); // Wywo³anie zdarzenia drabiny
        }
        else if (auto bonus = dynamic_cast<Bonus*>(mSquares[player.getPosition()].get())) {
            bonus->triggerEvent(player, diceResult, extraRoll); // Wywo³anie zdarzenia bonusu
        }
    }
}

void Board::setBoardTexture(const std::string& textureFile) {
    if (!mBoardTexture.loadFromFile(textureFile)) {
        std::cerr << "Failed to load board texture: " << textureFile << "\n"; // Informacja o b³êdzie ³adowania tekstury planszy
    }
    mBoardSprite.setTexture(mBoardTexture); // Ustawienie tekstury planszy
}

Board::~Board() = default; // Destruktor

std::string Board::getFilenameForDifficulty(Difficulty difficulty) {
    switch (difficulty) {
    case EASY:
        return "assets/easy.txt"; // Plik z plansz¹ dla ³atwego poziomu
    case MEDIUM:
        return "assets/medium.txt"; // Plik z plansz¹ dla œredniego poziomu
    case HARD:
        return "assets/hard.txt"; // Plik z plansz¹ dla trudnego poziomu
    default:
        return "assets/easy.txt"; // Domyœlnie plik z plansz¹ dla ³atwego poziomu
    }
}

void Board::loadFromFile(const std::string& filename, Game& game) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl; // Informacja o b³êdzie otwarcia pliku
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
            mSquares[start] = std::make_unique<Snake>(start, end, game.snakeHissSound); // Utworzenie wê¿a
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
