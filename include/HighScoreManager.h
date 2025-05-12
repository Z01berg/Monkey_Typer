#pragma once

#include <string>
#include <vector>
#include <deque> // For ScorePoint history
#include <algorithm> // For std::sort

// ScorePoint struct to store score at a specific time
struct ScorePoint {
    float timeStamp;
    int score;
};

class HighScoreManager {
public:
    HighScoreManager();

    // Loads high scores from a file.
    void loadHighScores(const std::string& filePath);

    // Saves current high scores to a file.
    void saveHighScores(const std::string& filePath);

    // Adds a new high score, sorts, and trims the list.
    void addHighScore(const std::string& name, int score, const std::deque<ScorePoint>& currentScoreHistory);

    // Returns the list of high scores (name, score pairs).
    const std::vector<std::pair<std::string, int>>& getHighScores() const;

    // Returns the score history for a specific high score entry.
    const std::vector<ScorePoint>& getScoreHistory(size_t index) const;


    const size_t MAX_HIGH_SCORES = 10;
private:
    std::vector<std::pair<std::string, int>> highScores;
    std::vector<std::vector<ScorePoint>> highScoreHistories; // Stores history for each high score
};