//
// Created by sammi on 13/01/2025.
//
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>

// --- Constants ---
const int ROWS = 5;
const int COLUMNS = 7;
const int PLAYER_START_HEALTH = 100;

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
    std::string description;
    int healthEffect; // Health impact of the room
    bool isTrapRoom;

public:
    // Default constructor
    Room()
        : row(0), col(0), description("A plain room."), healthEffect(0), isTrapRoom(false) {}

    // Parameterized constructor
    Room(int row, int col, const std::string& description, int healthEffect, bool isTrapRoom)
        : row(row), col(col), description(description), healthEffect(healthEffect), isTrapRoom(isTrapRoom) {}

    void enter(Player& player) {
        std::cout << "You entered Room (" << row << ", " << col << "): " << description << std::endl;
        if (isTrapRoom) {
            std::cout << "It's a trap room! You lose one move and 20% of your health.\n";
            player.updateHealth(-std::ceil(player.getHealth() * 0.2));
            player.decrementMove();
        } else if (healthEffect != 0) {
            if (healthEffect > 0) {
                std::cout << "It's a cure room! Health increased by " << healthEffect << " points.\n";
            } else {
                std::cout << "It's a poison room! Health reduced by " << -healthEffect << " points.\n";
            }
            player.updateHealth(healthEffect);
        }
    }
};

// --- Helper Functions ---
bool isAdjacent(int currentRow, int currentCol, int newRow, int newCol) {
    int rowDiff = std::abs(currentRow - newRow);
    int colDiff = std::abs(currentCol - newCol);
    return (rowDiff + colDiff == 1); // Manhattan distance = 1
}

// --- Game Logic ---
int main() {
    // Initialize player and monster
    Player player(0, 0);  // Top-left corner
    Monster monster(4, 6); // Bottom-right corner

    // Initialize rooms
    std::vector<std::vector<Room>> map(ROWS, std::vector<Room>(COLUMNS));
    srand(time(nullptr)); // Seed for randomization

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

            map[r][c] = Room(r, c, "A mysterious room.", effect, isTrap);
        }
    }

    // Game loop
    bool gameRunning = true;
    while (gameRunning) {
        player.displayStatus();

        // Player's turn
        while (player.getMovesLeft() > 0) {
            std::cout << "Enter your move (row col, -1 to exit): ";
            int newRow, newCol;
            std::cin >> newRow;
            if (newRow == -1) {
                std::cout << "You chose to exit the game. Goodbye!\n";
                gameRunning = false;
                break;
            }
            std::cin >> newCol;

            // Validate input
            if (!isAdjacent(player.getRow(), player.getCol(), newRow, newCol)) {
                std::cout << "You can only move to adjacent rooms! Try again.\n";
                continue;
            }
            if (newRow < 0 || newRow >= ROWS || newCol < 0 || newCol >= COLUMNS) {
                std::cout << "Invalid position. Try again.\n";
                continue;
            }

            // Move player
            player.setPosition(newRow, newCol);
            map[newRow][newCol].enter(player);

            if (player.getHealth() <= 0) {
                std::cout << "You have died! Game over.\n";
                gameRunning = false;
                break;
            }

            player.decrementMove();
        }

        if (!gameRunning) break;

        // Monster's turn
        monster.moveTowardPlayer(player.getRow(), player.getCol());
        std::cout << "Monster moved to (" << monster.getRow() << ", " << monster.getCol() << ").\n";

        if (monster.sensePlayer(player.getRow(), player.getCol())) {
            monster.attack(player);
            if (player.getHealth() <= 0) {
                std::cout << "You have died! Game over.\n";
                gameRunning = false;
            }
        }

        // Check win condition
        if (player.getRow() == 4 && player.getCol() == 6 && gameRunning) {
            std::cout << "Congratulations! You have reached the exit and escaped the cave!\n";
            gameRunning = false;
        }

        player.resetMoves();
    }

    std::cout << "Thank you for playing Cave Escape!\n";
    return 0;
}
