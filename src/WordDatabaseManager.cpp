#include "../include/WordDatabaseManager.h"
#include <fmt/core.h>
#include <filesystem>
#include <fstream>
#include <algorithm> // std::find

namespace fs = std::filesystem;

WordDatabaseManager::WordDatabaseManager() : randomGenerator(std::random_device{}()) {}

void WordDatabaseManager::loadWordSets(const std::string& dictionaryDirectory) {
    wordSets.clear();
    wordSetNames.clear();
    currentWordDatabase.clear();

    try {
        if (fs::exists(dictionaryDirectory) && fs::is_directory(dictionaryDirectory)) {
            for (const auto& entry : fs::directory_iterator(dictionaryDirectory)) {
                if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                    std::vector<std::string> words;
                    std::ifstream file(entry.path());
                    if (file.is_open()) {
                        std::string word;
                        while (file >> word) {
                            words.push_back(word);
                        }
                        if (!words.empty()) {
                            wordSets.push_back(words);
                            wordSetNames.push_back(entry.path().filename().string());
                            fmt::print("Loaded word set: {}\n", entry.path().filename().string());
                        }
                    }
                }
            }
        } else {
            fs::create_directory(dictionaryDirectory);
            fmt::print("Created '{}' directory for dictionary files.\n", dictionaryDirectory);
        }
    } catch (const std::exception& e) {
        fmt::print(stderr, "Error loading word sets from '{}': {}\n", dictionaryDirectory, e.what());
    }

    if (wordSets.empty()) {
        // Default words if no files found or all files were empty
        currentWordDatabase = {"hello", "world", "game", "typing", "monkey"};
        wordSets.push_back(currentWordDatabase);
        wordSetNames.push_back("default.txt");
        fmt::print("No word sets found or all were empty. Using default words.\n");
    } else {
        currentWordDatabase = wordSets[0]; // Default to the first loaded set
    }
}

const std::vector<std::vector<std::string>>& WordDatabaseManager::getAllWordSets() const {
    return wordSets;
}

std::vector<std::string> WordDatabaseManager::getWordSetNames() const {
    return wordSetNames;
}

void WordDatabaseManager::setCurrentWordSet(const std::string& setName) {
    auto it = std::find(wordSetNames.begin(), wordSetNames.end(), setName);
    if (it != wordSetNames.end()) {
        size_t index = std::distance(wordSetNames.begin(), it);
        if (index < wordSets.size()) {
            currentWordDatabase = wordSets[index];
            return;
        }
    }

    if (!wordSets.empty()) {
        currentWordDatabase = wordSets[0];
    }
}

void WordDatabaseManager::setCurrentWordSetByIndex(size_t index) {
    if (index < wordSets.size()) {
        currentWordDatabase = wordSets[index];
        fmt::print("WordDatabaseManager: Set current word set to index {}, name: {}\n", index, (index < wordSetNames.size() ? wordSetNames[index] : "N/A"));
    } else if (!wordSets.empty()) {
        currentWordDatabase = wordSets[0];
        fmt::print("WordDatabaseManager: Index {} out of bounds, defaulting to set 0\n", index);
    } else {
        fmt::print("WordDatabaseManager: No word sets loaded, cannot set by index.\n");
    }
}

const std::vector<std::string>* WordDatabaseManager::getWordSetByIndexPtr(size_t setIndex) const {
    if (setIndex < wordSets.size()) {
        return &wordSets[setIndex];
    }
    if (!wordSets.empty()) return &wordSets[0];
    return nullptr;
}

std::string WordDatabaseManager::getRandomWordFromSet(size_t setIndex) const {
    const std::vector<std::string>* selectedSet = getWordSetByIndexPtr(setIndex);

    if (selectedSet && !selectedSet->empty()) {
        std::uniform_int_distribution<size_t> dist(0, selectedSet->size() - 1);
        return (*selectedSet)[dist(randomGenerator)];
    }

    if (!currentWordDatabase.empty()){
        std::uniform_int_distribution<size_t> dist(0, currentWordDatabase.size() - 1);
        return currentWordDatabase[dist(randomGenerator)];
    }
    return "error";
}

std::string WordDatabaseManager::getRandomWord() const {
    if (currentWordDatabase.empty()) {
        return "error";
    }
    std::uniform_int_distribution<size_t> dist(0, currentWordDatabase.size() - 1);
    return currentWordDatabase[dist(randomGenerator)];
}

const std::vector<std::string>& WordDatabaseManager::getCurrentWordDatabase() const {
    return currentWordDatabase;
}