#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <string>
#include <memory>
#include <conio.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "gamemanager.h"

using namespace std;

GameManager::GameManager() {
  isRunning = true;
  setupGame();
}

GameManager::~GameManager() {
  // Puzzles are automatically deleted via std::unique_ptr in Room structs
}

string GameManager::readPassword() {
  string password = "";
  char ch;
  while ((ch = _getch()) != '\r') { // '\r' is Enter key
    if (ch == '\b') {              // Backspace
      if (password.length() > 0) {
        password.pop_back();
        cout << "\b \b";
      }
    } else if (ch >= 32 && ch <= 126) { // Printable characters
      password += ch;
      cout << '*';
    }
  }
  cout << endl;
  return password;
}

void GameManager::set_color(int color) {
#ifdef _WIN32
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hConsole != INVALID_HANDLE_VALUE) {
    SetConsoleTextAttribute(hConsole, (WORD)color);
  }
#endif
}

void GameManager::clearInputBuffer() {
  cin.clear();
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string GameManager::toLowerCase(const string &str) {
  string low = str;
  transform(low.begin(), low.end(), low.begin(), ::tolower);
  return low;
}

void GameManager::setupGame() {
  // Room 1: The Whispering Hall
  Room r1("The Whispering Hall");
  r1.addPuzzle(make_unique<TextPuzzle>(
      "I speak without a mouth and hear without ears. What am I?", "echo",
      "Ancient Echo Stone"));
  r1.addPuzzle(make_unique<TextPuzzle>("What has keys but no locks?", "keyboard",
                              "Rusty Keycap"));
  r1.addPuzzle(make_unique<TextPuzzle>(
      "I grow but am not alive; air is my life, water is my death. What am I?",
      "fire", "Flame Shard"));
  rooms.push_back(move(r1));

  // Room 2: The Cryptic Cave
  Room r2("The Cryptic Cave");
  r2.addPuzzle(make_unique<CaesarPuzzle>("wkh frgh lv 123 (Shift 3 Left)",
                                "the code is 123", "Cipher Scroll"));
  rooms.push_back(move(r2));

  // Room 3: The Analytical Chamber
  Room r3("The Analytical Chamber");
  string logicText =
      "Ali, Hamza, Ayesha. Dog, Cat, Parrot. Red, Blue, Green.\n1. Ali no Dog. "
      "2. Cat owner likes Blue. 3. Hamza not Green. 4. Ayesha has Parrot.";
  r3.addPuzzle(make_unique<LogicGridPuzzle>(
      logicText, "ali:cat,blue;hamza:dog,red;ayesha:parrot,green",
      "Logic Badge"));
  rooms.push_back(move(r3));

  // Room 4: The Ancient Floor
  Room r4("The Ancient Floor");
  r4.addPuzzle(make_unique<SymbolPuzzle>("I, III, VI, X", "XV", "Jeweled Compass"));
  rooms.push_back(move(r4));
}

bool GameManager::loginFlow() {
  set_color(14); // Yellow
  cout << "\n==== DIGITAL ESCAPE ROOM ACCESS ====\n";
  set_color(15); // White

  while (true) {
    cout << "1. Login\n2. Create Account\n3. Exit\n>> ";
    int choice;
    if (!(cin >> choice)) {
      clearInputBuffer();
      continue;
    }

    if (choice == 3)
      return false;

    string u, p;
    cout << "Username: ";
    cin >> u;
    cout << "Password: ";
    p = readPassword();

    if (choice == 1) {
      if (currentUser.authenticate(u, p)) {
        set_color(10); // Green
        cout << "Access Granted! Welcome back, " << u << ".\n";
        set_color(15);
        return true;
      } else {
        set_color(12); // Red
        cout << "Invalid credentials. Try again.\n";
        set_color(15);
      }
    } else if (choice == 2) {
      if (u.find(':') != string::npos) {
        set_color(12);
        cout << "Security Error: Username cannot contain ':' character.\n";
        set_color(15);
        continue;
      }
      if (currentUser.exists(u)) {
        set_color(12);
        cout << "Username already exists. Choose another.\n";
        set_color(15);
      } else {
        try {
          currentUser.setUsername(u);
          currentUser.setPassword(p);
          currentUser.setProgress(0);
          currentUser.saveProgress();
          set_color(11); // Cyan
          cout << "Account created successfully! Please login.\n";
          set_color(15);
        } catch (const std::exception &e) {
          set_color(12);
          cout << "Robustness Error: " << e.what() << "\n";
          set_color(15);
        }
      }
    }
  }
}

void GameManager::playLoop() {
  int totalPuzzles = 0;
  for (const auto &r : rooms)
    totalPuzzles += r.puzzles.size();

  // Restore inventory based on saved progress
  inventory.clear();
  int count = 0;
  int target = currentUser.getProgress();
  for (auto &room : rooms) {
    for (const auto &p : room.puzzles) {
      if (count < target) {
        inventory.push_back(p->getRewardItem());
        count++;
      }
    }
  }

  while (isRunning) {
    int currentProg = currentUser.getProgress();

    if (currentProg >= totalPuzzles) {
      set_color(11);
      cout << "\n*** CONGRATULATIONS! YOU HAVE ESCAPED! ***\n";
      cout << "Items collected: ";
      for (size_t i = 0; i < inventory.size(); ++i) {
        cout << inventory[i] << (i == inventory.size() - 1 ? "" : ", ");
      }
      cout << "\n";
      set_color(15);

      cout << "\nWould you like to reset and play again? (y/n): ";
      char reset;
      cin >> reset;
      if (reset == 'y' || reset == 'Y') {
        currentUser.setProgress(0);
        currentUser.saveProgress();
        inventory.clear();
        continue;
      } else {
        isRunning = false;
        break;
      }
    }

    // Find current room and puzzle
    int count = 0;
    size_t roomIdx = 0;
    size_t puzzleIdx = 0;
    bool found = false;

    for (size_t i = 0; i < rooms.size(); ++i) {
      if (currentProg < count + (int)rooms[i].puzzles.size()) {
        roomIdx = i;
        puzzleIdx = currentProg - count;
        found = true;
        break;
      }
      count += rooms[i].puzzles.size();
    }

    if (!found)
      break;

    set_color(13); // Magenta
    cout << "\n--- Current Room: " << rooms[roomIdx].name << " ---\n";
    set_color(15);

    rooms[roomIdx].puzzles[puzzleIdx]->present();

    string input;
    getline(cin, input);

    if (rooms[roomIdx].puzzles[puzzleIdx]->checkAnswer(input)) {
      set_color(10);
      cout << "Correct! You found: "
           << rooms[roomIdx].puzzles[puzzleIdx]->getRewardItem() << "\n";
      set_color(15);
      inventory.push_back(rooms[roomIdx].puzzles[puzzleIdx]->getRewardItem());
      currentUser.setProgress(currentProg + 1);
      currentUser.saveProgress();
    } else {
      set_color(12);
      cout << "That's not it. Look closer.\n";
      set_color(15);
    }
  }
}

void GameManager::start() {
  if (loginFlow()) {
    clearInputBuffer(); // Clear any leftover whitespace from login inputs
    playLoop();
  }
  cout << "Thank you for playing Digital Escape Room!\n";
}
