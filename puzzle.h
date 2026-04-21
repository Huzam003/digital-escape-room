#pragma once
#include <algorithm>
#include <iostream>
#include <string>
#include <cctype>
#include <vector>

// Base class for all puzzles - demonstrate polymorphism
class Puzzle {
public:
  virtual ~Puzzle() {}
  virtual void present() const = 0;
  virtual bool checkAnswer(const std::string &input) const = 0;
  virtual std::string getRewardItem() const = 0;
};

// Simple text-based riddles
class TextPuzzle : public Puzzle {
private:
  std::string question;
  std::string correctAnswer;
  std::string rewardItem;

public:
  TextPuzzle(const std::string &q, const std::string &ans,
             const std::string &item)
      : question(q), correctAnswer(ans), rewardItem(item) {}

  void present() const override { std::cout << question << "\nYour answer: "; }

  bool checkAnswer(const std::string &input) const override {
    // Case-insensitive check, allowing for some flexibility
    std::string lowInput = input;
    std::transform(lowInput.begin(), lowInput.end(), lowInput.begin(),
                   ::tolower);
    return (lowInput == correctAnswer ||
            lowInput.find(correctAnswer) != std::string::npos);
  }

  std::string getRewardItem() const override { return rewardItem; }
};

// Caesar Cipher puzzles
class CaesarPuzzle : public Puzzle {
private:
  std::string cipherText;
  std::string clearText;
  std::string rewardItem;

public:
  CaesarPuzzle(const std::string &cipher, const std::string &clear,
               const std::string &item)
      : cipherText(cipher), clearText(clear), rewardItem(item) {}

  void present() const override {
    std::cout << "Decrypt this message: " << cipherText << "\n>> ";
  }

  bool checkAnswer(const std::string &input) const override {
    std::string lowInput = input;
    std::transform(lowInput.begin(), lowInput.end(), lowInput.begin(),
                   ::tolower);
    return (lowInput == clearText);
  }

  std::string getRewardItem() const override { return rewardItem; }
};

// Complex logic puzzle involving a grid of assignments
class LogicGridPuzzle : public Puzzle {
private:
  std::string instructions;
  std::string target;
  std::string rewardItem;

public:
  LogicGridPuzzle(const std::string &instr, const std::string &t,
                  const std::string &item)
      : instructions(instr), target(t), rewardItem(item) {}

  void present() const override {
    std::cout << instructions
              << "\nFormat: Name:Pet,Color;Name:Pet,Color;...\n>> ";
  }

  bool checkAnswer(const std::string &input) const override {
    std::string clean;
    for (char c : input) {
      if (c != ' ')
        clean += (char)std::tolower(c);
    }
    return (clean == target);
  }

  std::string getRewardItem() const override { return rewardItem; }
};

// Symbol / Pattern sequence puzzle
class SymbolPuzzle : public Puzzle {
private:
  std::string sequence;
  std::string answer;
  std::string rewardItem;

public:
  SymbolPuzzle(const std::string &s, const std::string &a,
               const std::string &item)
      : sequence(s), answer(a), rewardItem(item) {}

  void present() const override {
    std::cout << "Analyze the sequence: " << sequence
              << " ... what comes next?\n>> ";
  }

  bool checkAnswer(const std::string &input) const override {
    std::string upInput = input;
    std::transform(upInput.begin(), upInput.end(), upInput.begin(), ::toupper);
    std::string clean;
    for (char c : upInput)
      if (c != ' ')
        clean += c;
    return (clean == answer);
  }

  std::string getRewardItem() const override { return rewardItem; }
};
