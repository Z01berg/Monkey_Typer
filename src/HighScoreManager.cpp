#include "../include/HighScoreManager.h"
#include <fstream>
#include <fmt/core.h>
#include <sstream> // For parsing score history lines
#include <algorithm> // For std::sort
#include <tuple>     // For std::tuple

HighScoreManager::HighScoreManager() {}

void HighScoreManager::loadHighScores(const std::string& filePath) {
    highScores.clear();
    highScoreHistories.clear();
    std::ifstream file(filePath);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) { // Read score line
            std::stringstream ss(line);
            std::string namePart;
            std::string fullName;
            int scoreVal = -1; // Initialize to detect if not found

            // Attempt to parse name and score from the line
            std::string tempNameBuilder;
            std::vector<std::string> parts;
            std::string segment;
            while(ss >> segment) {
                parts.push_back(segment);
            }

            if (parts.empty()) continue; // Skip empty lines

            // Try to parse the last part as score
            try {
                scoreVal = std::stoi(parts.back());
                parts.pop_back();
                for(const auto& p : parts) {
                    if (!tempNameBuilder.empty()) tempNameBuilder += " ";
                    tempNameBuilder += p;
                }
                fullName = tempNameBuilder;
            } catch (const std::invalid_argument& ia) {
                fmt::print(stderr, "Warning: Malformed high score line (could not parse score): {}\n", line);
                continue;
            } catch (const std::out_of_range& oor) {
                fmt::print(stderr, "Warning: Malformed high score line (score out of range): {}\n", line);
                continue;
            }

            if (fullName.empty() || scoreVal == -1) {
                fmt::print(stderr, "Warning: Malformed high score line (empty name or score not found): {}\n", line);
                continue;
            }

            highScores.emplace_back(fullName, scoreVal);

            // Read score history for this entry (next line)
            std::vector<ScorePoint> history;
            if (std::getline(file, line)) {
                std::stringstream history_ss(line);
                float time;
                int score_val_hist;
                char colon_char;

                // Read timestamp:score pairs
                while(history_ss >> time >> colon_char >> score_val_hist) {
                    if (colon_char == ':') {
                        history.push_back({time, score_val_hist});
                    } else {
                        fmt::print(stderr, "Warning: Malformed score history data for '{}'. Expected 'time:score'. Line: {}\n", fullName, line);
                        // history.clear(); // Optionally clear?
                        break;
                    }
                }
            } else {
                fmt::print(stderr, "Warning: Missing score history line for '{}'.\n", fullName);
            }
            highScoreHistories.push_back(history);
        }
        file.close();

    } else {
        fmt::print("High scores file '{}' not found. No scores loaded.\n", filePath);
    }

    // Sort scores and their corresponding histories together
    std::vector<std::tuple<int, std::string, std::vector<ScorePoint>>> sortableScores;
    for(size_t i = 0; i < highScores.size(); ++i) {
        if (i < highScoreHistories.size()) {
            sortableScores.emplace_back(highScores[i].second, highScores[i].first, highScoreHistories[i]);
        } else {
            sortableScores.emplace_back(highScores[i].second, highScores[i].first, std::vector<ScorePoint>());
            fmt::print(stderr, "Warning: Mismatch between scores and histories count during loading. Score '{}' assigned empty history.\n", highScores[i].first);
        }
    }

    // Sort by score descending (std::get<0> is the score)
    std::sort(sortableScores.begin(), sortableScores.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) > std::get<0>(b);
    });

    // Rebuild the primary vectors from the sorted data
    highScores.clear();
    highScoreHistories.clear();
    for(const auto& entry : sortableScores) {
        highScores.emplace_back(std::get<1>(entry), std::get<0>(entry)); // name, score
        highScoreHistories.push_back(std::get<2>(entry));                       // history
    }

    // Trim to MAX_HIGH_SCORES
    if (highScores.size() > MAX_HIGH_SCORES) {
        highScores.resize(MAX_HIGH_SCORES);
        highScoreHistories.resize(MAX_HIGH_SCORES);
    }
}

void HighScoreManager::saveHighScores(const std::string& filePath) {
    std::ofstream file(filePath);
    if (file.is_open()) {
        for (size_t i = 0; i < highScores.size(); ++i) {
            file << highScores[i].first << " " << highScores[i].second << "\n";
            // Save score history for this entry
            if (i < highScoreHistories.size()) {
                for(const auto& point : highScoreHistories[i]) {
                    file << point.timeStamp << ":" << point.score << " ";
                }
            } else {
                fmt::print(stderr, "Warning: Missing history for score '{}' during save.\n", highScores[i].first);
            }
            file << "\n";
        }
        file.close();
    } else {
        fmt::print(stderr, "Error: Could not open high scores file '{}' for saving.\n", filePath);
    }
}

void HighScoreManager::addHighScore(const std::string& name, int score, const std::deque<ScorePoint>& currentScoreHistory) {
    std::vector<ScorePoint> historyVec(currentScoreHistory.begin(), currentScoreHistory.end());

    // Don't add directly to highScores but create a tuple for the new entry and add it to a temporary list with existing scores.
    std::vector<std::tuple<int, std::string, std::vector<ScorePoint>>> allEntries;
    for (size_t i = 0; i < highScores.size(); ++i) {
        if (i < highScoreHistories.size()) {
            allEntries.emplace_back(highScores[i].second, highScores[i].first, highScoreHistories[i]);
        } else {
            allEntries.emplace_back(highScores[i].second, highScores[i].first, std::vector<ScorePoint>());
        }
    }
    // Add the new entry
    allEntries.emplace_back(score, name, historyVec);


    std::sort(allEntries.begin(), allEntries.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) > std::get<0>(b);
    });

    // Rebuild the main vectors from the sorted temporary structure
    highScores.clear();
    highScoreHistories.clear();
    for (const auto& entry : allEntries) {
        highScores.emplace_back(std::get<1>(entry), std::get<0>(entry)); // name, score
        highScoreHistories.push_back(std::get<2>(entry));                       // history
    }

    // Keep only top N
    if (highScores.size() > MAX_HIGH_SCORES) {
        highScores.resize(MAX_HIGH_SCORES);
        highScoreHistories.resize(MAX_HIGH_SCORES);
    }
}

const std::vector<std::pair<std::string, int>>& HighScoreManager::getHighScores() const {
    return highScores;
}

const std::vector<ScorePoint>& HighScoreManager::getScoreHistory(size_t index) const {
    static const std::vector<ScorePoint> emptyHistory;
    if (index < highScoreHistories.size()) {
        return highScoreHistories[index];
    }
    fmt::print(stderr, "Warning: Requested score history for out-of-bounds index {}.\n", index);
    return emptyHistory;
}