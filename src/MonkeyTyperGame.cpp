#include "../include/MonkeyTyperGame.h"
#include "../include/FontManager.h"
#include "../include/WordDatabaseManager.h"
#include "../include/SettingsManager.h"
#include "../include/HighScoreManager.h"
#include "../include/UIManager.h"
#include "../include/LinkedWord.h"
#include <fmt/core.h>
#include <random>
#include <algorithm>
#include <vector>

MonkeyTyperGame::MonkeyTyperGame()
        : window(sf::VideoMode(GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT), "Monkey Typer"),
          gameState(GameState::MENU),
          currentGlobalFont(nullptr),
          score(0),
          gameTime(0),
          lastScoreRecordTime(0),
          wordSpawnTimer(0),
          gameStats{},
          newHighScoreEligibleFlag(false),
          highScoreNamePromptAttempted(false) {
    init();
}

MonkeyTyperGame::~MonkeyTyperGame() {
}

void MonkeyTyperGame::init() {
    fontManager = std::make_unique<FontManager>();
    fontManager->loadFonts("PixelTCG.ttf", "../fonts/PixelTCG.ttf", "../fonts");
    currentGlobalFont = fontManager->getDefaultFont();

    wordDbManager = std::make_unique<WordDatabaseManager>();
    wordDbManager->loadWordSets("../dictionary");

    settingsManager = std::make_unique<SettingsManager>();
    settingsManager->loadSettings("../util/settings.cfg", *fontManager, currentGlobalFont);

    if (settingsManager->randomizeFontsEnabled) {
        if (!settingsManager->getRandomizedFontNames().empty()) {
            auto& rFonts = settingsManager->getRandomizedFontNames();
            auto it = rFonts.begin();
            static std::mt19937 init_font_gen(std::random_device{}());
            std::uniform_int_distribution<size_t> dist(0, rFonts.size() - 1);
            std::advance(it, dist(init_font_gen));
            currentGlobalFont = fontManager->getFont(*it);
        } else if (!fontManager->getAvailableFonts().empty()){
            currentGlobalFont = fontManager->getRandomFont();
        } else {
            currentGlobalFont = fontManager->getDefaultFont();
        }
    } else {
        currentGlobalFont = fontManager->getFont(settingsManager->explicitlySelectedFontName);
        if (!currentGlobalFont) {
            currentGlobalFont = fontManager->getDefaultFont();
        }
    }

    if (settingsManager->randomizeWordSets()) {
        const auto& rSetsIndices = settingsManager->getRandomizedWordSetIndices();
        if (!rSetsIndices.empty()) {
            auto it = rSetsIndices.begin();
            static std::mt19937 init_set_gen(std::random_device{}());
            std::uniform_int_distribution<size_t> dist(0, rSetsIndices.size() - 1);
            std::advance(it, dist(init_set_gen));
            wordDbManager->setCurrentWordSetByIndex(*it);
        } else if (!wordDbManager->getAllWordSets().empty()) {
            static std::mt19937 init_any_set_gen(std::random_device{}());
            std::uniform_int_distribution<size_t> dist(0, wordDbManager->getAllWordSets().size() - 1);
            wordDbManager->setCurrentWordSetByIndex(dist(init_any_set_gen));
        }
    } else {
        const auto& selectedIndices = settingsManager->getRandomizedWordSetIndices();
        if (!selectedIndices.empty()) {
            wordDbManager->setCurrentWordSetByIndex(*selectedIndices.begin());
        } else if (!wordDbManager->getAllWordSets().empty()){
            wordDbManager->setCurrentWordSetByIndex(0);
        }
    }

    highScoreManager = std::make_unique<HighScoreManager>();
    highScoreManager->loadHighScores("../util/highscores.txt");

    uiManager = std::make_unique<UIManager>(window, *this, *fontManager, *settingsManager, *highScoreManager, *wordDbManager);
    uiManager->initUI();
}

void MonkeyTyperGame::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}

void MonkeyTyperGame::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            if(settingsManager && fontManager) { // Ensure managers exist before saving
                settingsManager->saveSettings("../util/settings.cfg", currentGlobalFont, *fontManager);
            }
            window.close();
        }

        switch (gameState) {
            case GameState::MENU:         if(uiManager) uiManager->processMenuEvent(event); break;
            case GameState::PLAYING:      processPlayingEvents(event); break;
            case GameState::GAME_OVER:    if(uiManager) uiManager->processGameOverEvent(event); break;
            case GameState::HIGH_SCORES:  if(uiManager) uiManager->processHighScoresEvent(event); break;
            case GameState::SETTINGS:     if(uiManager) uiManager->processSettingsEvent(event); break;
            case GameState::FONT_SELECTION: if(uiManager) uiManager->processFontSelectionEvent(event); break;
            case GameState::FONT_RANDOMIZATION: if(uiManager) uiManager->processFontRandomizationEvent(event); break;
            case GameState::WORD_SET_SELECTION: if(uiManager) uiManager->processWordSetSelectionEvent(event); break;
            case GameState::GRAPH_VIEW:   if(uiManager) uiManager->processGraphViewEvent(event); break;
        }
    }
}

void MonkeyTyperGame::update(float deltaTime) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (uiManager) {
        uiManager->update(mousePos, deltaTime);
    }

    if (gameState == GameState::PLAYING) {
        updateGame(deltaTime);
    }
}

void MonkeyTyperGame::render() {
    window.clear(sf::Color(30, 30, 40));
    if (!uiManager) return;

    switch (gameState) {
        case GameState::MENU:         uiManager->renderMenu(); break;
        case GameState::PLAYING:      renderGame(); break;
        case GameState::GAME_OVER:    uiManager->renderGameOver(gameStats, score, scoreHistory, currentGlobalFont); break;
        case GameState::HIGH_SCORES:  uiManager->renderHighScores(); break;
        case GameState::SETTINGS:     uiManager->renderSettings(); break;
        case GameState::FONT_SELECTION: uiManager->renderFontSelection(); break;
        case GameState::FONT_RANDOMIZATION: uiManager->renderFontRandomization(); break;
        case GameState::WORD_SET_SELECTION: uiManager->renderWordSetSelection(); break;
        case GameState::GRAPH_VIEW:   uiManager->renderGraphView(); break;
    }

    window.display();
}

void MonkeyTyperGame::setGameState(GameState newState) {
    GameState oldState = gameState;
    gameState = newState;

    if (newState == GameState::GAME_OVER && oldState == GameState::PLAYING) {
        newHighScoreEligibleFlag = isNewHighScoreEligible(score);
        highScoreNamePromptAttempted = false;
    } else if (newState == GameState::MENU || (newState == GameState::PLAYING && oldState != GameState::PLAYING)) {
        newHighScoreEligibleFlag = false;
        highScoreNamePromptAttempted = false;
    }
}

MonkeyTyperGame::GameState MonkeyTyperGame::getGameState() const {
    return gameState;
}

void MonkeyTyperGame::startGame() {
    activeWords.clear();
    score = 0;
    gameTime = 0;
    lastScoreRecordTime = 0;
    wordSpawnTimer = 0;
    scoreHistory.clear();
    gameStats = GameConstants::GameStats{};

    if (settingsManager->randomizeFontsEnabled) {
        if (!settingsManager->getRandomizedFontNames().empty()) {
            auto& rFonts = settingsManager->getRandomizedFontNames();
            auto it = rFonts.begin();
            static std::mt19937 game_start_font_gen(std::random_device{}());
            std::uniform_int_distribution<size_t> dist(0, rFonts.size() - 1);
            std::advance(it, dist(game_start_font_gen));
            setCurrentFont(fontManager->getFont(*it));
        } else if (!fontManager->getAvailableFonts().empty()){
            setCurrentFont(fontManager->getRandomFont());
        } else {
            setCurrentFont(fontManager->getDefaultFont());
        }
    } else {
        setCurrentFont(fontManager->getFont(settingsManager->explicitlySelectedFontName));
    }

    if (!settingsManager->randomizeWordSets()) {
        const auto& selectedIndices = settingsManager->getRandomizedWordSetIndices();
        if (!selectedIndices.empty()) {
            wordDbManager->setCurrentWordSetByIndex(*selectedIndices.begin());
        } else if (!wordDbManager->getAllWordSets().empty()){
            wordDbManager->setCurrentWordSetByIndex(0);
        }
    }

    setGameState(GameState::PLAYING);
    for (int i = 0; i < 3; i++) spawnWord();
}

void MonkeyTyperGame::endGame() {
    setGameState(GameState::GAME_OVER);
}

void MonkeyTyperGame::submitHighScore(const std::string& playerName) {
    if (newHighScoreEligibleFlag) {
        highScoreManager->addHighScore(playerName, score, scoreHistory);
        highScoreManager->saveHighScores("../util/highscores.txt");
    }
    newHighScoreEligibleFlag = false;
    highScoreNamePromptAttempted = true;
}

bool MonkeyTyperGame::isNewHighScoreEligible(int currentScoreValue) const {
    if (currentScoreValue <= 0) return false;
    const auto& currentHighScores = highScoreManager->getHighScores();
    if (currentHighScores.size() < highScoreManager->MAX_HIGH_SCORES) {
        return true;
    }
    if (currentHighScores.empty()) return true;
    return currentScoreValue > currentHighScores.back().second;
}

std::shared_ptr<sf::Font> MonkeyTyperGame::getCurrentFont() const {
    return currentGlobalFont;
}

void MonkeyTyperGame::setCurrentFont(std::shared_ptr<sf::Font> font) {
    bool fontEffectivelyChanged = false;
    if (font && font != currentGlobalFont) {
        fontEffectivelyChanged = true;
    } else if (font && !currentGlobalFont) {
        fontEffectivelyChanged = true;
    }

    if (fontEffectivelyChanged) {
        currentGlobalFont = font;
        if (settingsManager && !settingsManager->randomizeFonts()) {
            bool found = false;
            for (const auto& [name, f_ptr] : fontManager->getAvailableFonts()) {
                if (f_ptr == font) {
                    settingsManager->explicitlySelectedFontName = name;
                    found = true;
                    break;
                }
            }
            if (!found && font) {
                settingsManager->explicitlySelectedFontName = font->getInfo().family;
            }
        }
        if (uiManager) {
            uiManager->updateFonts();
        }
        updateCurrentFontDependentObjects();
    }
}

void MonkeyTyperGame::updateCurrentFontDependentObjects(){
}

SettingsManager& MonkeyTyperGame::getSettingsManager() const {
    if (!settingsManager) {
        static SettingsManager dummy;
        return dummy;
    }
    return *settingsManager;
}

bool MonkeyTyperGame::getHighScoreNamePromptAttemptedFlag() const {
    return highScoreNamePromptAttempted;
}

void MonkeyTyperGame::setHighScoreNamePromptAttemptedFlag(bool attempted) {
    highScoreNamePromptAttempted = attempted;
}


void MonkeyTyperGame::updateGame(float deltaTime) {
    gameTime += deltaTime;
    wordSpawnTimer += deltaTime;

    for (auto it = activeWords.begin(); it != activeWords.end(); ) {
        if (!(*it)) {
            it = activeWords.erase(it);
            continue;
        }
        (*it)->update(deltaTime);
        if ((*it)->getSpeed() > 0 && (*it)->isOutOfBoundsRight()) {
            endGame();
            return;
        }
        if ( (*it)->isOutOfBounds() && ( (*it)->getSpeed() < 0 || ((*it)->isComplete() && (*it)->isOutOfBounds()) ) ) {
            if ((*it)->getSpeed() < 0 && !(*it)->isComplete()) {
                score = std::max(0, score - GameConstants::MISTAKE_PENALTY);
                gameStats.mistakesMade++;
            }
            it = activeWords.erase(it);
        } else {
            ++it;
        }
    }

    int completedThisFrame = 0;
    int charsTypedThisFrame = 0;
    int mistakesOnCompletedWordsThisFrame = 0;

    activeWords.erase(std::remove_if(activeWords.begin(), activeWords.end(),
                                     [&](std::unique_ptr<Word>& word) {
                                         if (!word) return false;
                                         if (word->isComplete()) {
                                             score += word->getText().length() * (word->isLinked() ? 2 : 1);
                                             completedThisFrame++;
                                             charsTypedThisFrame += word->getText().length();
                                             mistakesOnCompletedWordsThisFrame += word->getMistakesCount();
                                             return true;
                                         }
                                         return false;
                                     }), activeWords.end());

    gameStats.wordsTyped += completedThisFrame;
    gameStats.charactersTyped += charsTypedThisFrame;
    gameStats.mistakesMade += mistakesOnCompletedWordsThisFrame;

    if (gameTime > 0.001f) {
        gameStats.wordsPerMinute = (static_cast<float>(gameStats.charactersTyped) / 5.0f) / (gameTime / 60.0f);
        if (gameStats.wordsPerMinute < 0) gameStats.wordsPerMinute = 0;

        int totalCharsForAccuracy = gameStats.charactersTyped;
        int totalMistakesForAccuracy = gameStats.mistakesMade;

        int accuracyDenominator = totalCharsForAccuracy + totalMistakesForAccuracy;
        gameStats.accuracy = accuracyDenominator > 0 ? (static_cast<float>(totalCharsForAccuracy) / accuracyDenominator) * 100.0f : 100.0f;

        if (gameStats.accuracy < 0) gameStats.accuracy = 0;
        if (gameStats.accuracy > 100) gameStats.accuracy = 100;
    } else {
        gameStats.wordsPerMinute = 0;
        gameStats.accuracy = 100.0f;
    }

    float spawnInterval = GameConstants::WORD_SPAWN_INTERVAL / settingsManager->getSpawnRateMultiplier();
    if (wordSpawnTimer >= spawnInterval) {
        wordSpawnTimer = 0;
        spawnWord();
    }

    if (gameTime - lastScoreRecordTime >= 1.0f) {
        lastScoreRecordTime = gameTime;
        scoreHistory.push_back({gameTime, score});
        if (scoreHistory.size() > GameConstants::MAX_HISTORY_POINTS) {
            scoreHistory.pop_front();
        }
    }
    if (uiManager) {
        uiManager->scoreTextDisplay.setString(fmt::format("Score: {}", score));
        uiManager->gameTimeStatsTextDisplay.setString(fmt::format("Time: {:.1f}s | WPM: {:.1f} | Acc: {:.1f}%",
                                                                  gameTime, gameStats.wordsPerMinute, gameStats.accuracy));
    }
}

void MonkeyTyperGame::spawnWord() {
    static std::mt19937 spawn_random_gen(std::random_device{}());

    std::string wordTextToSpawn;

    if (settingsManager->randomizeWordSets()) {
        const auto& randomizedSetIndices = settingsManager->getRandomizedWordSetIndices();
        if (!randomizedSetIndices.empty()) {
            auto it = randomizedSetIndices.begin();
            std::uniform_int_distribution<size_t> dist(0, randomizedSetIndices.size() - 1);
            std::advance(it, dist(spawn_random_gen));
            wordTextToSpawn = wordDbManager->getRandomWordFromSet(*it);
        } else if (!wordDbManager->getAllWordSets().empty()) {
            std::uniform_int_distribution<size_t> dist(0, wordDbManager->getAllWordSets().size() - 1);
            wordTextToSpawn = wordDbManager->getRandomWordFromSet(dist(spawn_random_gen));
        } else {
            wordTextToSpawn = wordDbManager->getRandomWord();
        }
    } else {
        wordTextToSpawn = wordDbManager->getRandomWord();
    }
    if (wordTextToSpawn.empty() || wordTextToSpawn == "error") {
        wordTextToSpawn = "fallback";
    }

    std::uniform_int_distribution<int> heightDist(100, GameConstants::WINDOW_HEIGHT - 150);
    float speed = GameConstants::BASE_SPEED * settingsManager->getWordSpeedMultiplier();
    unsigned int fontSize = settingsManager->getCurrentFontSize();
    std::shared_ptr<sf::Font> fontToUse = currentGlobalFont;

    if (settingsManager->randomizeFontsEnabled) {
        if (!settingsManager->getRandomizedFontNames().empty()) {
            auto& rFonts = settingsManager->getRandomizedFontNames();
            auto it = rFonts.begin();
            std::uniform_int_distribution<size_t> dist(0, rFonts.size() - 1);
            std::advance(it, dist(spawn_random_gen));
            fontToUse = fontManager->getFont(*it);
        } else if(!fontManager->getAvailableFonts().empty()){
            fontToUse = fontManager->getRandomFont();
        } else {
            fontToUse = fontManager->getDefaultFont();
        }
    }
    if (!fontToUse) {
        fontToUse = fontManager->getDefaultFont();
    }

    bool makeLinked = settingsManager->useLinkedWords() && (std::uniform_int_distribution<int>(0, 100)(spawn_random_gen) < 30);

    if (makeLinked) {
        int wordCount = std::uniform_int_distribution<int>(2, std::min(4, 2 + score / 30))(spawn_random_gen);
        std::vector<std::string> linkedWordTextsList;
        std::vector<float> yPositionsList;
        float wordHeightEstimate = fontToUse->getLineSpacing(fontSize);

        for (int i = 0; i < wordCount; ++i) {
            std::string partText;
            if (settingsManager->randomizeWordSets()) {
                const auto& randomizedSetIndices = settingsManager->getRandomizedWordSetIndices();
                if (!randomizedSetIndices.empty()) {
                    auto it = randomizedSetIndices.begin(); std::uniform_int_distribution<size_t> dist(0, randomizedSetIndices.size()-1); std::advance(it, dist(spawn_random_gen)); partText = wordDbManager->getRandomWordFromSet(*it);
                } else if(!wordDbManager->getAllWordSets().empty()) {
                    std::uniform_int_distribution<size_t> dist(0, wordDbManager->getAllWordSets().size()-1); partText = wordDbManager->getRandomWordFromSet(dist(spawn_random_gen));
                } else {partText = wordDbManager->getRandomWord();}
            } else {
                partText = wordDbManager->getRandomWord();
            }
            if(partText.empty() || partText == "error") partText = "lpart";
            linkedWordTextsList.push_back(partText);

            float attemptY;
            int yAttempts = 0;
            bool positionFound = false;

            while(!positionFound && yAttempts < 15) {
                attemptY = static_cast<float>(heightDist(spawn_random_gen));
                bool collision = false;
                for (const auto& existingWord : activeWords) {
                    if (existingWord) {
                        sf::FloatRect existingBounds;
                        if (existingWord->isLinked()) {
                            const auto& lw = static_cast<const LinkedWord&>(*existingWord);
                            if (!lw.getWordTexts().empty() && lw.getCurrentPart() >=0 && lw.getCurrentPart() < static_cast<int>(lw.getWordTexts().size())) {
                                existingBounds = lw.getWordTexts()[lw.getCurrentPart()].getGlobalBounds();
                            } else continue;
                        } else {
                            existingBounds = existingWord->getSfText().getGlobalBounds();
                        }
                        if (attemptY < existingBounds.top + existingBounds.height && attemptY + wordHeightEstimate > existingBounds.top) {
                            collision = true;
                            break;
                        }
                    }
                }
                for (float prevY : yPositionsList) {
                    if (std::abs(prevY - attemptY) < wordHeightEstimate * 0.9f) {
                        collision = true;
                        break;
                    }
                }
                if (!collision) positionFound = true;
                yAttempts++;
            }
            if(!positionFound) {
                attemptY = yPositionsList.empty() ? static_cast<float>(heightDist(spawn_random_gen)) : (yPositionsList.back() + wordHeightEstimate + 10.f);
                if(attemptY > GameConstants::WINDOW_HEIGHT - wordHeightEstimate - 20.f) {
                    attemptY = static_cast<float>(heightDist(spawn_random_gen));
                } else if (attemptY < 50.f) {
                    attemptY = 50.f;
                }
            }
            yPositionsList.push_back(attemptY);
        }
        if (yPositionsList.empty()) {
            yPositionsList.push_back(static_cast<float>(heightDist(spawn_random_gen)));
        }
        if (linkedWordTextsList.empty()){
            linkedWordTextsList.push_back("linkErr");
        }
        activeWords.emplace_back(std::make_unique<LinkedWord>(linkedWordTextsList, *fontToUse, speed, sf::Vector2f(-150.f, yPositionsList[0]), fontSize, yPositionsList));
    } else {
        sf::Vector2f position(-100.f, static_cast<float>(heightDist(spawn_random_gen)));
        activeWords.emplace_back(std::make_unique<Word>(wordTextToSpawn, *fontToUse, speed, position, fontSize));
    }
}

void MonkeyTyperGame::processPlayingEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        for (auto& word : activeWords) {
            if (word && word->isBeingActivelyTyped()) {
                word->resetTyping();
                break;
            }
        }
        return;
    }

    if (event.type == sf::Event::TextEntered) {
        sf::Uint32 unicode = event.text.unicode;
        if ((unicode < 32 || unicode > 126) && unicode != 8) {
            return;
        }

        Word* wordBeingTyped = nullptr;
        for (auto& word : activeWords) {
            if (word && word->isBeingActivelyTyped() && !word->isComplete()) {
                wordBeingTyped = word.get();
                break;
            }
        }

        if (wordBeingTyped) {
            wordBeingTyped->processInput(unicode, settingsManager->isHighlightTypingEnabled());
        } else if (unicode != 8 && unicode >=32 && unicode <=126) {
            std::vector<Word*> matchingWords;
            char firstCharTyped = static_cast<char>(tolower(unicode));

            for (auto& word : activeWords) {
                if (word && !word->isComplete() && !word->getText().empty() &&
                    tolower(word->getText()[0]) == firstCharTyped) {
                    matchingWords.push_back(word.get());
                }
            }

            if (!matchingWords.empty()) {
                std::sort(matchingWords.begin(), matchingWords.end(), [](const Word* a, const Word* b) {
                    if (!a) return false; if (!b) return true;
                    return a->getPosition().x > b->getPosition().x;
                });

                for(auto& w : activeWords) {
                    if(w && w.get() != matchingWords[0]) {
                        w->setAsActiveTypingTarget(false);
                    }
                }
                matchingWords[0]->setAsActiveTypingTarget(true);
                matchingWords[0]->processInput(unicode, settingsManager->isHighlightTypingEnabled());
            }
        }
    }
}

void MonkeyTyperGame::renderGame() {
    if (settingsManager->getCurrentInputStyle() == GameConstants::InputDisplayStyle::CENTERED) {
        sf::RectangleShape pathGuide(sf::Vector2f(GameConstants::WINDOW_WIDTH, 5));
        pathGuide.setFillColor(sf::Color(80, 80, 80, 150));
        pathGuide.setPosition(sf::Vector2f(0.f, GameConstants::WINDOW_HEIGHT / 2.0f));
        window.draw(pathGuide);
    }

    for (const auto& word : activeWords) {
        if (word) {
            word->draw(window);
            if (settingsManager->isHighlightTypingEnabled() && word->isBeingActivelyTyped()) {
                sf::FloatRect bounds;
                if (word->isLinked()){
                    const auto& lw = static_cast<const LinkedWord&>(*word);
                    if (lw.getCurrentPart() >= 0 && lw.getCurrentPart() < static_cast<int>(lw.getWordTexts().size())) {
                        bounds = lw.getWordTexts()[lw.getCurrentPart()].getGlobalBounds();
                    } else continue;
                } else {
                    bounds = word->getSfText().getGlobalBounds();
                }
                if (bounds.width > 0 && bounds.height > 0) {
                    sf::RectangleShape highlightRect(sf::Vector2f(bounds.width + 10, bounds.height + 6));
                    highlightRect.setFillColor(sf::Color(255, 255, 0, 50));
                    highlightRect.setPosition(sf::Vector2f(bounds.left - 5, bounds.top - 3));
                    window.draw(highlightRect);
                }
            }
        }
    }

    for (const auto& word : activeWords) {
        if (word && word->isBeingActivelyTyped() && !word->getCurrentInput().empty()) {
            sf::Text inputDisplay(word->getCurrentInput(), *currentGlobalFont, settingsManager->getCurrentFontSize());
            inputDisplay.setFillColor(sf::Color(100, 255, 100));

            sf::Vector2f textPos;
            switch (settingsManager->getCurrentInputStyle()) {
                case GameConstants::InputDisplayStyle::UNDER_CURSOR:
                    textPos = sf::Vector2f(sf::Mouse::getPosition(window).x + 7.f, sf::Mouse::getPosition(window).y + 20.f);
                    break;
                case GameConstants::InputDisplayStyle::BELOW_WORD:
                {
                    sf::FloatRect wordBounds;
                    if (word->isLinked()){
                        const auto& lw = static_cast<const LinkedWord&>(*word);
                        if (lw.getCurrentPart() >= 0 && lw.getCurrentPart() < static_cast<int>(lw.getWordTexts().size())) {
                            wordBounds = lw.getWordTexts()[lw.getCurrentPart()].getGlobalBounds();
                        } else { break; }
                    } else {
                        wordBounds = word->getSfText().getGlobalBounds();
                    }
                    textPos = sf::Vector2f(wordBounds.left, wordBounds.top + wordBounds.height + 3);
                }
                    break;
                case GameConstants::InputDisplayStyle::CENTERED:
                {
                    sf::FloatRect inputBounds = inputDisplay.getLocalBounds();
                    textPos = sf::Vector2f(GameConstants::WINDOW_WIDTH / 2.0f - inputBounds.width / 2.0f,
                                           GameConstants::WINDOW_HEIGHT / 2.0f - inputBounds.height / 2.0f + 30.0f);
                }
                    break;
            }
            inputDisplay.setPosition(textPos);
            window.draw(inputDisplay);
            break;
        }
    }
    if(uiManager) {
        window.draw(uiManager->scoreTextDisplay);
        window.draw(uiManager->gameTimeStatsTextDisplay);
    }
}

const GameConstants::GameStats& MonkeyTyperGame::getGameStats() const { return gameStats; }
float MonkeyTyperGame::getGameTime() const { return gameTime; }
int MonkeyTyperGame::getScore() const { return score; }
const std::deque<ScorePoint>& MonkeyTyperGame::getScoreHistory() const { return scoreHistory; }