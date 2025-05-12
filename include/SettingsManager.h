#pragma once

#include <string>
#include <set>
#include <memory>
#include <vector> // For vector of string in load/save
#include <SFML/Graphics/Font.hpp>
#include "../include/Constants.h"

class FontManager;

class SettingsManager {
public:
    SettingsManager();

    void loadSettings(const std::string& filePath, FontManager& fontManager, std::shared_ptr<sf::Font>& currentFontRef);
    void saveSettings(const std::string& filePath, const std::shared_ptr<sf::Font>& currentFont, const FontManager& fontManager);

    float getWordSpeedMultiplier() const;
    float getSpawnRateMultiplier() const;
    bool isHighlightTypingEnabled() const;
    unsigned int getCurrentFontSize() const;
    GameConstants::InputDisplayStyle getCurrentInputStyle() const;
    bool useLinkedWords() const; // From useLinkedWordsEnabled
    bool randomizeFonts() const; // From randomizeFontsEnabled
    const std::set<std::string>& getRandomizedFontNames() const;
    const std::set<size_t>& getRandomizedWordSetIndices() const;
    bool randomizeWordSets() const; // From randomizeWordSetsEnabled


    void setWordSpeedMultiplier(float val);
    void setSpawnRateMultiplier(float val);
    void setHighlightTypingEnabled(bool val);
    void setCurrentFontSize(unsigned int val);
    void setCurrentInputStyle(GameConstants::InputDisplayStyle val);
    void setUseLinkedWords(bool val);
    void setRandomizeFonts(bool val);
    void toggleRandomizedFont(const std::string& fontName);
    void toggleRandomizedWordSet(size_t setIndex);
    void clearRandomizedFonts();
    void clearRandomizedWordSets();
    void setRandomizeWordSets(bool val);


    // Public for easier access by UIManager if needed, or direct modification by game logic.
    // Consider making these private with more setters if stricter control is needed.
    float wordSpeedMultiplier;
    float spawnRateMultiplier;
    bool highlightTyping;
    unsigned int currentFontSize;
    GameConstants::InputDisplayStyle currentInputStyle;
    bool useLinkedWordsEnabled;
    bool randomizeFontsEnabled;
    std::string explicitlySelectedFontName; // Font selected when randomizeFonts is OFF
    std::set<std::string> randomizedFontNames;

    bool randomizeWordSetsEnabled; // NEW: Flag for word set randomization strategy
    std::set<size_t> randomizedWordSetIndices;
};