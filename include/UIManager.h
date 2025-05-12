#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <set>
#include <memory>
#include "../include/Button.h"
#include "../include/TextAnimation.h"
#include "../include/Constants.h"
#include "../include/HighScoreManager.h" // For ScorePoint definition

// Forward declarations of classes UIManager interacts with
class MonkeyTyperGame;
class FontManager;
class SettingsManager;
// HighScoreManager is included above
class WordDatabaseManager;

class UIManager {
public:
    UIManager(sf::RenderWindow& window, MonkeyTyperGame& game, FontManager& fontManager,
              SettingsManager& settingsManager, HighScoreManager& highScoreManager, WordDatabaseManager& wordDBManager);

    void initUI();
    void update(const sf::Vector2f& mousePos, float deltaTime);

    // Event processing methods for different game states
    void processMenuEvent(sf::Event& event);
    void processGameOverEvent(sf::Event& event);
    void processHighScoresEvent(sf::Event& event);
    void processSettingsEvent(sf::Event& event);
    void processFontSelectionEvent(sf::Event& event);
    void processFontRandomizationEvent(sf::Event& event);
    void processWordSetSelectionEvent(sf::Event& event);
    void processGraphViewEvent(sf::Event& event);

    // Rendering methods for different game states/screens
    void renderMenu();
    void renderGameOver(const GameConstants::GameStats& gameStats, int score, const std::deque<ScorePoint>& scoreHistory, const std::shared_ptr<sf::Font>& currentFont);
    void renderScoreGraph(const std::deque<ScorePoint>& scoreHistory, const sf::FloatRect& graphBounds, const std::shared_ptr<sf::Font>& currentFont, const std::string& title);
    void renderHighScores();
    void renderSettings();
    void renderFontSelection();
    void renderFontRandomization();
    void renderWordSetSelection();
    void renderGraphView();

    void updateFonts(); // Called when the global game font changes
    void refreshSettingsButtons(); // Called to update settings button labels/structure

    // Publicly accessible sf::Text objects for MonkeyTyperGame to update during gameplay
    sf::Text scoreTextDisplay;
    sf::Text gameTimeStatsTextDisplay; // Displays WPM, Accuracy, Time

private:
    // Helper methods for creating button sets
    void createMenuButtons();
    void createGameOverButtons();
    void createSettingsButtons(); // This handles dynamic labels based on settings
    void createHighScoresButtons();
    void createFontSelectionRelatedButtons(); // Back buttons for font/word set screens, graph view

    void initSettingsAnimations(); // Initializes example animations for the settings screen

    // References to core game components
    sf::RenderWindow& window;
    MonkeyTyperGame& game;
    FontManager& fontManager;
    SettingsManager& settingsManager;
    HighScoreManager& highScoreManager;
    WordDatabaseManager& wordDBManager;

    // UI elements specific to certain screens or states
    sf::Text gameOverHeaderText;
    sf::Text finalScoreText;
    bool enteringHighScoreName; // True if the high score name input prompt is active
    std::string playerNameInput; // Stores the player's name as it's typed

    // Collections of buttons for different screens
    std::vector<Button> menuButtons;
    std::vector<Button> gameOverButtons;
    std::vector<Button> highScoresNavigationButtons; // e.g., "Back", "View Graph" on high scores screen
    int selectedScoreIndex; // Index of the currently selected high score for viewing details/graph

    std::vector<Button> settingsButtons;
    std::vector<TextAnimation> settingsAnimations; // Animations for the settings screen examples

    // Individual buttons for specific screens (often "Back" buttons)
    Button fontScreenBackButton;
    Button wordSetScreenBackButton;
    Button graphViewBackButton;
};