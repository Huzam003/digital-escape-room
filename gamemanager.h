#pragma once
#include <vector>
#include <string>
#include <memory>
#include "user.h"
#include "puzzle.h"

// Room structure to organize puzzles
struct Room {
    std::string name;
    std::vector<std::unique_ptr<Puzzle>> puzzles;
    
    Room(std::string n) : name(n) {}
    void addPuzzle(std::unique_ptr<Puzzle> p) { puzzles.push_back(std::move(p)); }
};

class GameManager {
private:
    User currentUser;
    std::vector<Room> rooms;
    std::vector<std::string> inventory;
    bool isRunning;

    void setupGame();
    void clearInputBuffer();
    std::string toLowerCase(const std::string& str);
    std::string readPassword();
    void set_color(int color);

    // Flow control
    bool loginFlow();
    void playLoop();

public:
    GameManager();
    ~GameManager();

    void start();
};
