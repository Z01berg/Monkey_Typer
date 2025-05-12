#pragma once

#include <string>
#include <vector>
#include <random>

class WordDatabaseManager {
public:
    WordDatabaseManager();

    // Loads all word sets from .txt files in the specified directory.
    void loadWordSets(const std::string& dictionaryDirectory);

    // Returns the list of all loaded word sets.
    const std::vector<std::vector<std::string>>& getAllWordSets() const;

    // Returns a list of filenames for the loaded word sets.
    std::vector<std::string> getWordSetNames() const;


    // Selects a word set to be the current one for fetching random words.
    // If a set name is not found, it defaults to the first loaded set.
    void setCurrentWordSet(const std::string& setName);
    void setCurrentWordSetByIndex(size_t index);


    // Returns a random word from the currently selected word set.
    std::string getRandomWord() const;

    // Returns the currently active word database (vector of strings).
    const std::vector<std::string>& getCurrentWordDatabase() const;

    std::string getRandomWordFromSet(size_t setIndex) const;
    const std::vector<std::string>* getWordSetByIndexPtr(size_t setIndex) const; // Helper


private:
    std::vector<std::vector<std::string>> wordSets;
    std::vector<std::string> wordSetNames; // Stores filenames
    std::vector<std::string> currentWordDatabase; // The active set of words
    mutable std::mt19937 randomGenerator; // For random word selection
};