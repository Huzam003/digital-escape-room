#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

#include "user.h"

using namespace std;

User::User() : username(""), passwordHash(""), progress(0) {}

string User::hashPassword(const string& password, const string& salt) const {
    // Deterministic FNV-1a Hash Algorithm (Deterministic & Stable)
    // Augmented with salt for Cybersecurity (Password uniqueness)
    unsigned long long hash = 0xcbf29ce484222325ULL;
    
    // Mix in the salt
    string saltedInput = salt + password;

    for (char c : saltedInput) {
        hash ^= (unsigned char)c;
        hash *= 0x100000001b3ULL;
    }
    return to_string(hash);
}

bool User::exists(const string& user) const {
    ifstream inFile("users.txt");
    if (!inFile.is_open()) return false;

    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string fileUser;
        getline(ss, fileUser, ':');
        if (fileUser == user) {
            inFile.close();
            return true;
        }
    }
    inFile.close();
    return false;
}

bool User::authenticate(const string& user, const string& pass) {
    string hashedPass = hashPassword(pass, user);
    ifstream inFile("users.txt");
    if (!inFile.is_open()) return false;

    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string fileUser, fileHash;
        int fileProgress = 0;

        if (getline(ss, fileUser, ':') && getline(ss, fileHash, ':')) {
            ss >> fileProgress;
            if (fileUser == user && fileHash == hashedPass) {
                username = user;
                passwordHash = hashedPass;
                progress = fileProgress;
                inFile.close();
                return true;
            }
        }
    }
    inFile.close();
    return false;
}

void User::saveProgress() {
    vector<string> lines;
    ifstream inFile("users.txt");
    bool found = false;

    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string fileUser, fileHash;
            int fileProgress;

            if (getline(ss, fileUser, ':') && getline(ss, fileHash, ':')) {
                ss >> fileProgress;
                if (fileUser == username) {
                    lines.push_back(username + ":" + passwordHash + ":" + to_string(progress));
                    found = true;
                } else {
                    lines.push_back(line);
                }
            }
        }
        inFile.close();
    }

    if (!found && !username.empty()) {
        lines.push_back(username + ":" + passwordHash + ":" + to_string(progress));
    }

    ofstream outFile("users.txt");
    for (const auto& l : lines) {
        outFile << l << "\n";
    }
    outFile.close();
}

void User::setUsername(const string& u) { username = u; }
string User::getUsername() const { return username; }
void User::setPassword(const string& p) { 
    // Cybersecurity: Password Strength Enforcement
    if (p.length() < 6) {
        throw std::runtime_error("Password too weak (min 6 characters required)");
    }
    passwordHash = hashPassword(p, username); 
}
int User::getProgress() const { return progress; }
void User::setProgress(int p) { progress = p; }
