//
// Created by sammi on 13/01/2025.
//
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

// --- Constants ---
const int ROWS = 5;
const int COLUMNS = 7;
const int CELL_SIZE = 100;
const int PLAYER_START_HEALTH = 100;

// Colors
sf::Color TRAP_COLOR = sf::Color::Red;
sf::Color CURE_COLOR = sf::Color::Green;
sf::Color POISON_COLOR = sf::Color::Blue;
sf::Color NORMAL_COLOR = sf::Color::White;

// --- Class Definitions ---
class Player {
private:
    int health;
    int row, col; // Position on the map
    int movesLeft;

public:
    Player(int startRow, int startCol)
        : health(PLAYER_START_HEALTH), row(startRow), col(startCol), movesLeft(2) {}

    int getHealth() const { return health; }
    int getRow() const { return row; }
    int getCol() const { return col; }
    int getMovesLeft() const { return movesLeft; }

    void setPosition(int newRow, int newCol) {
        row = newRow;
        col = newCol;
    }

    void updateHealth(int value) {
        health += value;
        if (health < 0) health = 0; // Prevent negative health
    }

    void decrementMove() { movesLeft--; }
    void resetMoves() { movesLeft = 2; }

    void displayStatus() const {
        std::cout << "Player | Health: " << health
                  << " | Position: (" << row << ", " << col << ")"
                  << " | Moves Left: " << movesLeft << std::endl;
    }
};

class Monster {
private:
    int row, col; // Position on the map

public:
    Monster(int startRow, int startCol) : row(startRow), col(startCol) {}

    int getRow() const { return row; }
    int getCol() const { return col; }

    void moveTowardPlayer(int playerRow, int playerCol) {
        if (row < playerRow) row++;
        else if (row > playerRow) row--;

        if (col < playerCol) col++;
        else if (col > playerCol) col--;
    }

    bool sensePlayer(int playerRow, int playerCol) const {
        return row == playerRow && col == playerCol;
    }

    void attack(Player& player) const {
        std::cout << "Monster attacks! Player loses 20 health points.\n";
        player.updateHealth(-20);
    }
};

class Room {
private:
    int row, col; // Room's position on the map
    int healthEffect; // Health impact of the room
    bool isTrapRoom;

public:
    Room() : row(0), col(0), healthEffect(0), isTrapRoom(false) {}
    Room(int r, int c, int effect, bool isTrap)
        : row(r), col(c), healthEffect(effect), isTrapRoom(isTrap) {}

    void enter(Player& player) {
        if (isTrapRoom) {
            std::cout << "You entered a trap room! Health reduced by 20%, and you lose one move.\n";
            player.updateHealth(-std::ceil(player.getHealth() * 0.2));
            player.decrementMove();
        } else if (healthEffect != 0) {
            if (healthEffect > 0) {
                std::cout << "You entered a cure room! Health increased by " << healthEffect << " points.\n";
            } else {
                std::cout << "You entered a poison room! Health reduced by " << -healthEffect << " points.\n";
            }
            player.updateHealth(healthEffect);
        }
    }

    sf::Color getColor() const {
        if (isTrapRoom) return TRAP_COLOR;
        if (healthEffect > 0) return CURE_COLOR;
        if (healthEffect < 0) return POISON_COLOR;
        return NORMAL_COLOR;
    }
};

// --- Game Logic ---
int main() {
    srand(time(nullptr)); // Seed for randomization

    // Initialize player and monster
    Player player(0, 0);  // Top-left corner
    Monster monster(4, 6); // Bottom-right corner

    // Initialize rooms
    std::vector<std::vector<Room>> map(ROWS, std::vector<Room>(COLUMNS));
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLUMNS; c++) {
            int effect = 0;
            bool isTrap = false;

            if ((rand() % 10) < 3) { // 30% chance of a trap
                isTrap = true;
            } else if ((rand() % 10) < 2) { // 20% chance of a poison room
                effect = -10;
            } else if ((rand() % 10) < 2) { // 20% chance of a cure room
                effect = 25;
            }

            map[r][c] = Room(r, c, effect, isTrap);
        }
    }

    // Set up SFML window
    sf::RenderWindow window(sf::VideoMode(COLUMNS * CELL_SIZE, ROWS * CELL_SIZE), "Cave Escape");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Draw grid and rooms
        window.clear();
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLUMNS; c++) {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                cell.setPosition(c * CELL_SIZE, r * CELL_SIZE);
                cell.setFillColor(map[r][c].getColor());
                cell.setOutlineColor(sf::Color::Black);
                cell.setOutlineThickness(-1);
                window.draw(cell);
            }
        }

        // Draw player and monster
        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading font.\n";
            return -1;
        }
        sf::Text playerText("P", font, CELL_SIZE / 2);
        playerText.setFillColor(sf::Color::Yellow);
        playerText.setPosition(player.getCol() * CELL_SIZE + CELL_SIZE / 4, player.getRow() * CELL_SIZE);
        window.draw(playerText);

        sf::Text monsterText("M", font, CELL_SIZE / 2);
        monsterText.setFillColor(sf::Color::Red);
        monsterText.setPosition(monster.getCol() * CELL_SIZE + CELL_SIZE / 4, monster.getRow() * CELL_SIZE);
        window.draw(monsterText);

        window.display();
    }

    return 0;
}
