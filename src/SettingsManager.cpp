#include "../include/SettingsManager.h"
#include "../include/FontManager.h"
#include <fstream>
#include <fmt/core.h>
#include <sstream>

SettingsManager::SettingsManager()
        : wordSpeedMultiplier(1.0f),
          spawnRateMultiplier(1.0f),
          highlightTyping(true),
          currentFontSize(GameConstants::DEFAULT_FONT_SIZE),
          currentInputStyle(GameConstants::InputDisplayStyle::UNDER_CURSOR),
          useLinkedWordsEnabled(true),
          randomizeFontsEnabled(false),
          explicitlySelectedFontName("PixelTCG.ttf"), // Default
          randomizeWordSetsEnabled(false) {}

void SettingsManager::loadSettings(const std::string& filePath, FontManager& fontManager, std::shared_ptr<sf::Font>& currentFontRef) {
    std::ifstream file(filePath);
    if (file.is_open()) {
        std::string line;

        // Helper for robust reading
        auto readLineAndParse = [&](auto& var, const std::string& varName) {
            if (std::getline(file, line)) {
                std::stringstream ss(line);
                if (!(ss >> var)) {
                    fmt::print(stderr, "Warning: Failed to parse '{}' from settings, using default.\n", varName);
                }
            } else {
                fmt::print(stderr, "Warning: Missing line for '{}' in settings, using default.\n", varName);
            }
        };

        auto readBool = [&](bool& var, const std::string& varName){
            int tempInt;
            if (std::getline(file, line)) {
                std::stringstream ss(line);
                if (!(ss >> tempInt) || (tempInt !=0 && tempInt !=1) ) {
                    fmt::print(stderr, "Warning: Failed to parse bool '{}' from settings, using default.\n", varName);
                } else {
                    var = (tempInt == 1);
                }
            } else {
                fmt::print(stderr, "Warning: Missing line for bool '{}' in settings, using default.\n", varName);
            }
        };


        readLineAndParse(wordSpeedMultiplier, "wordSpeedMultiplier");
        readLineAndParse(spawnRateMultiplier, "spawnRateMultiplier");

        readBool(highlightTyping, "highlightTyping");
        readLineAndParse(currentFontSize, "currentFontSize");

        int inputStyleInt = static_cast<int>(GameConstants::InputDisplayStyle::UNDER_CURSOR); // Default
        readLineAndParse(inputStyleInt, "inputStyleInt");
        if (inputStyleInt >=0 && inputStyleInt < GameConstants::INPUT_DISPLAY_NAMES.size()) {
            currentInputStyle = static_cast<GameConstants::InputDisplayStyle>(inputStyleInt);
        } else {
            fmt::print(stderr, "Warning: Invalid input style from settings, using default.\n");
        }


        std::string fontNameFromFile;
        if (std::getline(file, fontNameFromFile) && !fontNameFromFile.empty()) {
            currentFontRef = fontManager.getFont(fontNameFromFile);
            if (!currentFontRef) {
                fmt::print(stderr, "Warning: Font '{}' not found from settings, using default font.\n", fontNameFromFile);
                currentFontRef = fontManager.getDefaultFont();
            }
        } else {
            fmt::print(stderr, "Warning: Failed to read font name from settings or name is empty, using default.\n");
            currentFontRef = fontManager.getDefaultFont();
        }

        readBool(randomizeFontsEnabled, "randomizeFontsEnabled");

        if (std::getline(file, line)) {
            if (!line.empty()) {
                explicitlySelectedFontName = line;
            } else {
                fmt::print("Warning: explicitlySelectedFontName line empty in settings, using default '{}'.\n", explicitlySelectedFontName);
            }
        } else {
            fmt::print("Warning: Missing line for explicitlySelectedFontName in settings, using default '{}'.\n", explicitlySelectedFontName);
        }


        readBool(randomizeWordSetsEnabled, "randomizeWordSetsEnabled");

        randomizedFontNames.clear();
        if (std::getline(file, line)) {
            std::stringstream ssFonts(line);
            std::string name;
            while (ssFonts >> name) {
                randomizedFontNames.insert(name);
            }
        }  else {
            fmt::print("Warning: Missing line for randomized font names in settings.\n");
        }

        // Load randomized word sets (indices)
        randomizedWordSetIndices.clear();
        if (std::getline(file, line)) {
            std::stringstream ssSets(line);
            size_t index;
            while (ssSets >> index) {
                randomizedWordSetIndices.insert(index);
            }
        } else {
            fmt::print("Warning: Missing line for randomized word set indices in settings.\n");
        }

        file.close();

        // After loading all, if randomizeFontsEnabled is OFF, ensure currentFontRef matches explicitlySelectedFontName
        if (!randomizeFontsEnabled) {
            currentFontRef = fontManager.getFont(explicitlySelectedFontName);
            if(!currentFontRef) currentFontRef = fontManager.getDefaultFont();
        }


    } else {
        fmt::print("Settings file '{}' not found. Using default settings.\n", filePath);
        currentFontRef = fontManager.getDefaultFont();
    }
}

void SettingsManager::saveSettings(const std::string& filePath, const std::shared_ptr<sf::Font>& currentFont, const FontManager& fontManager) {
    std::ofstream file(filePath);
    if (file.is_open()) {
        std::string activeFontName = "PixelTCG.ttf"; // Default
        for (const auto& [name, fontPtr] : fontManager.getAvailableFonts()) {
            if (fontPtr == currentFont) {
                activeFontName = name;
                break;
            }
        }

        file << wordSpeedMultiplier << "\n"
             << spawnRateMultiplier << "\n"
             << (highlightTyping ? 1 : 0) << "\n"
             << currentFontSize << "\n"
             << static_cast<int>(currentInputStyle) << "\n"
             << activeFontName << "\n" // Font that was active when settings were saved (could be random if randomize ON)
             << (randomizeFontsEnabled ? 1 : 0) << "\n"
             << explicitlySelectedFontName << "\n" // User's choice for non-random mode
             << (randomizeWordSetsEnabled ? 1 : 0) << "\n";

        for (const auto& name : randomizedFontNames) {
            file << name << " ";
        }
        file << "\n";

        for (const auto& index : randomizedWordSetIndices) {
            file << index << " ";
        }
        file << "\n";

        file.close();
    } else {
        fmt::print(stderr, "Error: Could not open settings file '{}' for saving.\n", filePath);
    }
}

float SettingsManager::getWordSpeedMultiplier() const { return wordSpeedMultiplier; }
float SettingsManager::getSpawnRateMultiplier() const { return spawnRateMultiplier; }
bool SettingsManager::isHighlightTypingEnabled() const { return highlightTyping; }
unsigned int SettingsManager::getCurrentFontSize() const { return currentFontSize; }
GameConstants::InputDisplayStyle SettingsManager::getCurrentInputStyle() const { return currentInputStyle; }
bool SettingsManager::useLinkedWords() const { return useLinkedWordsEnabled; }
bool SettingsManager::randomizeFonts() const { return randomizeFontsEnabled; }
const std::set<std::string>& SettingsManager::getRandomizedFontNames() const { return randomizedFontNames; }
const std::set<size_t>& SettingsManager::getRandomizedWordSetIndices() const { return randomizedWordSetIndices; }
bool SettingsManager::randomizeWordSets() const { return randomizeWordSetsEnabled; }


void SettingsManager::setWordSpeedMultiplier(float val) { wordSpeedMultiplier = val; }
void SettingsManager::setSpawnRateMultiplier(float val) { spawnRateMultiplier = val; }
void SettingsManager::setHighlightTypingEnabled(bool val) { highlightTyping = val; }
void SettingsManager::setCurrentFontSize(unsigned int val) { currentFontSize = val; }
void SettingsManager::setCurrentInputStyle(GameConstants::InputDisplayStyle val) { currentInputStyle = val; }
void SettingsManager::setUseLinkedWords(bool val) { useLinkedWordsEnabled = val; }
void SettingsManager::setRandomizeFonts(bool val) { randomizeFontsEnabled = val; }
void SettingsManager::setRandomizeWordSets(bool val) { randomizeWordSetsEnabled = val; }

void SettingsManager::toggleRandomizedFont(const std::string& fontName) {
    if (randomizedFontNames.count(fontName)) {
        randomizedFontNames.erase(fontName);
    } else {
        randomizedFontNames.insert(fontName);
    }
}

void SettingsManager::toggleRandomizedWordSet(size_t setIndex) {
    if (randomizedWordSetIndices.count(setIndex)) {
        randomizedWordSetIndices.erase(setIndex);
    } else {
        randomizedWordSetIndices.insert(setIndex);
    }
}
void SettingsManager::clearRandomizedFonts() { randomizedFontNames.clear(); }
void SettingsManager::clearRandomizedWordSets() { randomizedWordSetIndices.clear(); }