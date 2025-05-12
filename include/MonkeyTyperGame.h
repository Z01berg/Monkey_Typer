#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include <string>
#include <memory>
#include <deque>
#include <set>
#include "Constants.h"
#include "Word.h"

class FontManager;
class WordDatabaseManager;
class SettingsManager;
class HighScoreManager;
class UIManager;
struct ScorePoint;

class MonkeyTyperGame {
public:
    enum class GameState {
        MENU,
        PLAYING,
        GAME_OVER,
        HIGH_SCORES,
        SETTINGS,
        FONT_SELECTION,
        FONT_RANDOMIZATION,
        WORD_SET_SELECTION,
        GRAPH_VIEW
    };

    MonkeyTyperGame();
    ~MonkeyTyperGame();
    void run();

    void setGameState(GameState newState);
    GameState getGameState() const;
    void startGame();
    void endGame();
    void submitHighScore(const std::string& playerName);
    bool isNewHighScoreEligible(int currentScore) const;

    std::shared_ptr<sf::Font> getCurrentFont() const;
    void setCurrentFont(std::shared_ptr<sf::Font> font);
    void updateCurrentFontDependentObjects();

    sf::RenderWindow& getWindow();
    const GameConstants::GameStats& getGameStats() const;
    float getGameTime() const;
    int getScore() const;
    const std::deque<ScorePoint>& getScoreHistory() const;

    SettingsManager& getSettingsManager() const;

    bool getHighScoreNamePromptAttemptedFlag() const;
    void setHighScoreNamePromptAttemptedFlag(bool attempted);

private:
    void init();
    void processEvents();
    void update(float deltaTime);
    void render();

    void updateGame(float deltaTime);
    void spawnWord();
    void processPlayingEvents(sf::Event& event);
    void renderGame();

    sf::RenderWindow window;
    GameState gameState;

    std::shared_ptr<sf::Font> currentGlobalFont;

    std::unique_ptr<FontManager> fontManager;
    std::unique_ptr<WordDatabaseManager> wordDbManager;
    std::unique_ptr<SettingsManager> settingsManager;
    std::unique_ptr<HighScoreManager> highScoreManager;
    std::unique_ptr<UIManager> uiManager;

    std::vector<std::unique_ptr<Word>> activeWords;
    int score;
    float gameTime;
    float lastScoreRecordTime;
    float wordSpawnTimer;
    std::deque<ScorePoint> scoreHistory;
    GameConstants::GameStats gameStats;

    bool newHighScoreEligibleFlag;
    bool highScoreNamePromptAttempted;
};