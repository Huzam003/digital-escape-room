#pragma once
#include <string>
#include <vector>
#include <stdexcept>

class User {
private:
    std::string username;
    std::string passwordHash;
    int progress;

    // Deterministic FNV-1a hash to ensure stability across program runs
    std::string hashPassword(const std::string& password, const std::string& salt) const;

public:
    User();

    // Core verification
    bool authenticate(const std::string& username, const std::string& password);
    
    // Persistence
    void saveProgress();
    bool exists(const std::string& username) const; // Check if user already exists

    // Getters & Setters
    void setUsername(const std::string& newUsername);
    std::string getUsername() const;
    void setPassword(const std::string& newPassword);
    int getProgress() const;
    void setProgress(int newProgress);
};
