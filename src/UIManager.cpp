#include "../include/UIManager.h"
#include "../include/MonkeyTyperGame.h"
#include "../include/FontManager.h"
#include "../include/SettingsManager.h"
#include "../include/HighScoreManager.h"
#include "../include/WordDatabaseManager.h"
#include <fmt/core.h>
#include <random>
#include <algorithm>

UIManager::UIManager(sf::RenderWindow& window, MonkeyTyperGame& game, FontManager& fontManager,
                     SettingsManager& settingsManager, HighScoreManager& highScoreManager, WordDatabaseManager& wordDBManager)
        : window(window), game(game), fontManager(fontManager), settingsManager(settingsManager),
          highScoreManager(highScoreManager), wordDBManager(wordDBManager),
          enteringHighScoreName(false), selectedScoreIndex(-1) {}

void UIManager::initUI() {
    auto currentFont = game.getCurrentFont();
    if (!currentFont) {
        currentFont = fontManager.getDefaultFont();
    }

    scoreTextDisplay.setFont(*currentFont);
    scoreTextDisplay.setCharacterSize(30);
    scoreTextDisplay.setFillColor(sf::Color::White);
    scoreTextDisplay.setPosition(sf::Vector2f(10.f, 10.f));

    gameTimeStatsTextDisplay.setFont(*currentFont);
    gameTimeStatsTextDisplay.setCharacterSize(20);
    gameTimeStatsTextDisplay.setFillColor(sf::Color(180, 180, 180));
    gameTimeStatsTextDisplay.setPosition(sf::Vector2f(10.f, 50.f));

    gameOverHeaderText.setFont(*currentFont);
    gameOverHeaderText.setCharacterSize(50);
    gameOverHeaderText.setFillColor(sf::Color::Red);
    gameOverHeaderText.setString("GAME OVER");

    finalScoreText.setFont(*currentFont);
    finalScoreText.setCharacterSize(40);
    finalScoreText.setFillColor(sf::Color::White);

    createMenuButtons();
    createGameOverButtons();
    createSettingsButtons();
    createHighScoresButtons();
    createFontSelectionRelatedButtons();
    initSettingsAnimations();
}

void UIManager::initSettingsAnimations() {
    settingsAnimations.push_back(TextAnimation{"The quick brown fox"});
    settingsAnimations.push_back(TextAnimation{"Typing example..."});
    settingsAnimations.push_back(TextAnimation{"Different font size"});
    settingsAnimations.push_back(TextAnimation{"Highlighted typing"});
}

void UIManager::updateFonts() {
    auto currentFont = game.getCurrentFont();
    if (!currentFont) {
        currentFont = fontManager.getDefaultFont();
    }

    scoreTextDisplay.setFont(*currentFont);
    gameTimeStatsTextDisplay.setFont(*currentFont);
    gameOverHeaderText.setFont(*currentFont);
    finalScoreText.setFont(*currentFont);

    createMenuButtons();
    createGameOverButtons();
    createSettingsButtons();
    createHighScoresButtons();
    createFontSelectionRelatedButtons();
}

void UIManager::createMenuButtons() {
    menuButtons.clear();
    auto font = game.getCurrentFont();
    if (!font) font = fontManager.getDefaultFont();
    const std::vector<std::string> labels = {"Start Game", "High Scores", "Settings", "Exit"};
    for (size_t i = 0; i < labels.size(); ++i) {
        sf::Vector2f textDesiredPos(GameConstants::WINDOW_WIDTH / 2.0f, 250.0f + i * 80.0f);
        Button button(labels[i], *font, 40, sf::Vector2f(0,0) );
        sf::FloatRect currentTextLocalBounds = button.text.getLocalBounds();
        button.setPosition(sf::Vector2f(textDesiredPos.x - currentTextLocalBounds.width / 2.0f, textDesiredPos.y));
        menuButtons.push_back(button);
    }
}

void UIManager::createGameOverButtons() {
    gameOverButtons.clear();
    auto font = game.getCurrentFont();
    if (!font) font = fontManager.getDefaultFont();
    const std::vector<std::string> labels = {"Play Again", "Main Menu", "Exit"};

    std::vector<Button> tempButtonsStorage;
    float totalButtonsLayoutWidth = 0;
    float buttonLayoutSpacing = 60.0f;

    for(const auto& label : labels) {
        tempButtonsStorage.emplace_back(label, *font, 40, sf::Vector2f(0,0));
        totalButtonsLayoutWidth += tempButtonsStorage.back().getGlobalBounds().width;
    }
    if (labels.size() > 1) {
        totalButtonsLayoutWidth += (labels.size() - 1) * buttonLayoutSpacing;
    }

    float currentButtonLayoutX = (GameConstants::WINDOW_WIDTH - totalButtonsLayoutWidth) / 2.0f;
    float buttonLayoutY = 580.0f;

    for (const auto& tempBtnRef : tempButtonsStorage) {
        Button finalButton(tempBtnRef.text.getString(), *font, 40, sf::Vector2f(0,0));
        sf::FloatRect currentTextLocalBounds = finalButton.text.getLocalBounds();
        float buttonActualRenderWidth = finalButton.getGlobalBounds().width;

        float textTargetXForButton = currentButtonLayoutX + buttonActualRenderWidth / 2.f - currentTextLocalBounds.width / 2.f;
        finalButton.setPosition(sf::Vector2f(textTargetXForButton, buttonLayoutY));
        gameOverButtons.push_back(finalButton);
        currentButtonLayoutX += buttonActualRenderWidth + buttonLayoutSpacing;
    }
}


void UIManager::createSettingsButtons() {
    settingsButtons.clear();
    auto font = game.getCurrentFont();
    if (!font) font = fontManager.getDefaultFont();

    const std::vector<std::string> settingLabels = {
            fmt::format("Word Speed: {:.0f}%", settingsManager.wordSpeedMultiplier * 100),
            fmt::format("Spawn Rate: {:.0f}%", settingsManager.spawnRateMultiplier * 100),
            fmt::format("Font Size: {}", settingsManager.currentFontSize),
            fmt::format("Highlight Typing: {}", settingsManager.highlightTyping ? "ON" : "OFF"),
            fmt::format("Input Display: {}", GameConstants::INPUT_DISPLAY_NAMES[static_cast<int>(settingsManager.currentInputStyle)]),
            fmt::format("Linked Words: {}", settingsManager.useLinkedWordsEnabled ? "ON" : "OFF"),
            fmt::format("Randomize Fonts: {}", settingsManager.randomizeFontsEnabled ? "ON" : "OFF"),
            settingsManager.randomizeFontsEnabled ? "Edit Randomized Fonts" : "",
            fmt::format("Randomize Word Sets: {}", settingsManager.randomizeWordSets() ? "ON" : "OFF"),
            settingsManager.randomizeWordSets() ? "Edit Randomized Word Sets" : "",
            "Select Font",
            "Select Word Sets",
            "Back"
    };

    const float buttonLayoutSpacing = 60.0f;
    const float startButtonY = 120.0f;
    const unsigned int buttonFontSize = 32;
    float currentButtonY = startButtonY;

    for (const auto& label : settingLabels) {
        if (label.empty()) continue;
        sf::Vector2f textDesiredPos(GameConstants::WINDOW_WIDTH / 2.0f, currentButtonY);
        Button button(label, *font, buttonFontSize, sf::Vector2f(0,0));
        sf::FloatRect currentTextLocalBounds = button.text.getLocalBounds();
        button.setPosition(sf::Vector2f(textDesiredPos.x - currentTextLocalBounds.width / 2.0f, textDesiredPos.y));
        settingsButtons.push_back(button);
        currentButtonY += buttonLayoutSpacing;
    }
}

void UIManager::refreshSettingsButtons() {
    createSettingsButtons();
}

void UIManager::createHighScoresButtons() {
    highScoresNavigationButtons.clear();
    auto font = game.getCurrentFont();
    if (!font) font = fontManager.getDefaultFont();

    Button backButtonPrototype("Back", *font, 30, sf::Vector2f(0,0));
    Button graphButtonPrototype("View Graph", *font, 30, sf::Vector2f(0,0));

    float backButtonWidth = backButtonPrototype.getGlobalBounds().width;
    float graphButtonWidth = graphButtonPrototype.getGlobalBounds().width;
    float hsNavButtonSpacing = 40.f;
    float totalHsNavWidth = backButtonWidth + graphButtonWidth + hsNavButtonSpacing;
    float hsNavStartX = (GameConstants::WINDOW_WIDTH - totalHsNavWidth) / 2.f;
    float hsNavY = GameConstants::WINDOW_HEIGHT - 80.0f;

    Button actualBackButton("Back", *font, 30, sf::Vector2f(0,0));
    sf::FloatRect backTextLocalBounds = actualBackButton.text.getLocalBounds();
    actualBackButton.setPosition(sf::Vector2f(hsNavStartX + backButtonWidth / 2.f - backTextLocalBounds.width / 2.f, hsNavY));
    highScoresNavigationButtons.push_back(actualBackButton);

    Button actualGraphButton("View Graph", *font, 30, sf::Vector2f(0,0));
    sf::FloatRect graphTextLocalBounds = actualGraphButton.text.getLocalBounds();
    actualGraphButton.setPosition(sf::Vector2f(hsNavStartX + backButtonWidth + hsNavButtonSpacing + graphButtonWidth / 2.f - graphTextLocalBounds.width / 2.f, hsNavY));
    highScoresNavigationButtons.push_back(actualGraphButton);
}


void UIManager::createFontSelectionRelatedButtons() {
    auto font = game.getCurrentFont();
    if (!font) font = fontManager.getDefaultFont();
    sf::Vector2f backButtonTextDesiredPos(
            GameConstants::WINDOW_WIDTH / 2.0f,
            GameConstants::WINDOW_HEIGHT - 100.0f
    );

    fontScreenBackButton = Button("Back", *font, 40, sf::Vector2f(0,0));
    sf::FloatRect currentTextLocalBoundsFont = fontScreenBackButton.text.getLocalBounds();
    fontScreenBackButton.setPosition(sf::Vector2f(
            backButtonTextDesiredPos.x - currentTextLocalBoundsFont.width / 2.0f,
            backButtonTextDesiredPos.y
    ));

    wordSetScreenBackButton = Button("Back", *font, 40, sf::Vector2f(0,0));
    sf::FloatRect currentTextLocalBoundsWordSet = wordSetScreenBackButton.text.getLocalBounds();
    wordSetScreenBackButton.setPosition(sf::Vector2f(
            backButtonTextDesiredPos.x - currentTextLocalBoundsWordSet.width / 2.0f,
            backButtonTextDesiredPos.y
    ));

    sf::Vector2f graphBackButtonTextDesiredPos(
            GameConstants::WINDOW_WIDTH / 2.0f,
            GameConstants::WINDOW_HEIGHT - 60.0f
    );
    graphViewBackButton = Button("Back", *font, 30, sf::Vector2f(0,0));
    sf::FloatRect currentTextLocalBoundsGraph = graphViewBackButton.text.getLocalBounds();
    graphViewBackButton.setPosition(sf::Vector2f(
            graphBackButtonTextDesiredPos.x - currentTextLocalBoundsGraph.width / 2.0f,
            graphBackButtonTextDesiredPos.y
    ));
}

void UIManager::update(const sf::Vector2f& mousePos, float deltaTime) {
    switch (game.getGameState()) {
        case MonkeyTyperGame::GameState::MENU:
            for (auto& button : menuButtons) button.update(mousePos);
            break;
        case MonkeyTyperGame::GameState::GAME_OVER:
            if (!enteringHighScoreName) {
                for (auto& button : gameOverButtons) button.update(mousePos);
            }
            break;
        case MonkeyTyperGame::GameState::HIGH_SCORES:
            for (auto& button : highScoresNavigationButtons) button.update(mousePos);
            break;
        case MonkeyTyperGame::GameState::SETTINGS:
            for (auto& button : settingsButtons) button.update(mousePos);
            for (auto& anim : settingsAnimations) {
                anim.update(deltaTime);
                if (anim.complete) anim.reset();
            }
            break;
        case MonkeyTyperGame::GameState::FONT_SELECTION:
        case MonkeyTyperGame::GameState::FONT_RANDOMIZATION:
            fontScreenBackButton.update(mousePos);
            break;
        case MonkeyTyperGame::GameState::WORD_SET_SELECTION:
            wordSetScreenBackButton.update(mousePos);
            break;
        case MonkeyTyperGame::GameState::GRAPH_VIEW:
            graphViewBackButton.update(mousePos);
            break;
        default:
            break;
    }
}

void UIManager::processMenuEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        for (size_t i = 0; i < menuButtons.size(); ++i) {
            if (menuButtons[i].contains(mousePos)) {
                switch (i) {
                    case 0: game.startGame(); break;
                    case 1: game.setGameState(MonkeyTyperGame::GameState::HIGH_SCORES); selectedScoreIndex = -1; break;
                    case 2:
                        game.setGameState(MonkeyTyperGame::GameState::SETTINGS);
                        for (auto& anim : settingsAnimations) anim.reset();
                        refreshSettingsButtons();
                        break;
                    case 3: window.close(); break;
                }
                return;
            }
        }
    }
}

void UIManager::processGameOverEvent(sf::Event& event) {
    if (enteringHighScoreName) {
        bool inputModeEventHandled = false;
        if (event.type == sf::Event::TextEntered) {
            inputModeEventHandled = true;
            if (event.text.unicode == 13) { // Enter
                if (!playerNameInput.empty()) {
                    game.submitHighScore(playerNameInput);
                }
                enteringHighScoreName = false;
                playerNameInput.clear();
            } else if (event.text.unicode == 8) { // Backspace
                if (!playerNameInput.empty()) playerNameInput.pop_back();
            }
            else if (event.text.unicode >= 32 && event.text.unicode <= 126 && playerNameInput.length() < 20) {
                playerNameInput += static_cast<char>(event.text.unicode);
            } else {
                inputModeEventHandled = false;
            }
        } else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                inputModeEventHandled = true;
                enteringHighScoreName = false;
                playerNameInput.clear();
                game.setHighScoreNamePromptAttemptedFlag(true); // Mark ESC as an attempt to deal with prompt
            }
        }

        if (inputModeEventHandled) {
            return;
        }
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (enteringHighScoreName) {
            sf::Vector2f boxSize(400, 120);
            sf::Vector2f boxPos( GameConstants::WINDOW_WIDTH / 2.0f - boxSize.x / 2.0f,
                                 GameConstants::WINDOW_HEIGHT / 2.0f - boxSize.y / 2.0f );
            sf::FloatRect inputBoxArea(boxPos, boxSize);
            if (!inputBoxArea.contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                enteringHighScoreName = false;
                playerNameInput.clear();
                game.setHighScoreNamePromptAttemptedFlag(true);
            } else {
                return;
            }
        }

        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        for (size_t i = 0; i < gameOverButtons.size(); ++i) {
            if (gameOverButtons[i].contains(mousePos)) {
                switch (i) {
                    case 0: game.startGame(); break;
                    case 1: game.setGameState(MonkeyTyperGame::GameState::MENU); break;
                    case 2: window.close(); break;
                }
                return;
            }
        }
    }
}


void UIManager::processHighScoresEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

        sf::RectangleShape localHsPanelDefinition;
        localHsPanelDefinition.setSize(sf::Vector2f(800, 500));
        localHsPanelDefinition.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH / 2.0f - localHsPanelDefinition.getSize().x / 2.0f, 150.0f));
        float headerRegionHeight = 20.0f + 40.0f;
        float listContentStartYPos = localHsPanelDefinition.getPosition().y + headerRegionHeight + 10.f;

        const auto& scores = highScoreManager.getHighScores();
        for (size_t i = 0; i < scores.size(); ++i) {
            float yPos = listContentStartYPos + i * 40.0f;
            sf::FloatRect scoreItemRect(
                    localHsPanelDefinition.getPosition().x + 20.f,
                    yPos - 5.0f,
                    localHsPanelDefinition.getSize().x - 40.f,
                    35.0f
            );
            if (scoreItemRect.contains(mousePos)) {
                selectedScoreIndex = static_cast<int>(i);
                return;
            }
        }

        if (!highScoresNavigationButtons.empty() && highScoresNavigationButtons[0].contains(mousePos)) {
            game.setGameState(MonkeyTyperGame::GameState::MENU);
            selectedScoreIndex = -1;
            return;
        }
        if (highScoresNavigationButtons.size() > 1 && selectedScoreIndex >= 0 &&
            static_cast<size_t>(selectedScoreIndex) < scores.size() &&
            highScoresNavigationButtons[1].contains(mousePos)) {
            if (!highScoreManager.getScoreHistory(selectedScoreIndex).empty()) {
                game.setGameState(MonkeyTyperGame::GameState::GRAPH_VIEW);
            }
            return;
        }
    }
    if (event.type == sf::Event::KeyPressed) {
        game.setGameState(MonkeyTyperGame::GameState::MENU);
        selectedScoreIndex = -1;
    }
}

void UIManager::processSettingsEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        bool settingChangedProcessed = false;

        for (size_t i = 0; i < settingsButtons.size(); ++i) {
            if (settingsButtons[i].contains(mousePos)) {
                settingChangedProcessed = true;
                std::string buttonText = settingsButtons[i].text.getString().toAnsiString();

                if (buttonText == "Back") {
                    game.setGameState(MonkeyTyperGame::GameState::MENU);
                    settingsManager.saveSettings("../util/settings.cfg", game.getCurrentFont(), fontManager);
                } else if (buttonText == "Select Word Sets") {
                    game.setGameState(MonkeyTyperGame::GameState::WORD_SET_SELECTION);
                } else if (buttonText == "Select Font") {
                    game.setGameState(MonkeyTyperGame::GameState::FONT_SELECTION);
                } else if (buttonText == "Edit Randomized Fonts" && settingsManager.randomizeFontsEnabled) {
                    game.setGameState(MonkeyTyperGame::GameState::FONT_RANDOMIZATION);
                } else if (buttonText.find("Word Speed:") == 0) {
                    float cs = settingsManager.getWordSpeedMultiplier() + 0.25f;
                    settingsManager.setWordSpeedMultiplier(cs > 2.01f ? 0.25f : cs);
                } else if (buttonText.find("Spawn Rate:") == 0) {
                    float cr = settingsManager.getSpawnRateMultiplier() + 0.25f;
                    settingsManager.setSpawnRateMultiplier(cr > 2.01f ? 0.25f : cr);
                } else if (buttonText.find("Font Size:") == 0) {
                    unsigned int cfs = settingsManager.getCurrentFontSize() + 2;
                    settingsManager.setCurrentFontSize(cfs > 40 ? 16 : cfs);
                    game.updateCurrentFontDependentObjects();
                    updateFonts();
                } else if (buttonText.find("Highlight Typing:") == 0) {
                    settingsManager.setHighlightTypingEnabled(!settingsManager.isHighlightTypingEnabled());
                } else if (buttonText.find("Input Display:") == 0) {
                    int style = (static_cast<int>(settingsManager.getCurrentInputStyle()) + 1) % GameConstants::INPUT_DISPLAY_NAMES.size();
                    settingsManager.setCurrentInputStyle(static_cast<GameConstants::InputDisplayStyle>(style));
                } else if (buttonText.find("Linked Words:") == 0) {
                    settingsManager.setUseLinkedWords(!settingsManager.useLinkedWords());
                } else if (buttonText.find("Randomize Fonts:") == 0) {
                    bool wasRandomizing = settingsManager.randomizeFonts();
                    settingsManager.setRandomizeFonts(!wasRandomizing);
                    if (!settingsManager.randomizeFonts()) {
                        game.setCurrentFont(fontManager.getFont(settingsManager.explicitlySelectedFontName));
                    } else {
                        if (!settingsManager.getRandomizedFontNames().empty()) {
                            auto& rFonts = settingsManager.getRandomizedFontNames();
                            auto it = rFonts.begin();
                            static std::mt19937 ui_set_font_gen_toggle(std::random_device{}());
                            std::uniform_int_distribution<size_t> dist(0, rFonts.size() - 1);
                            std::advance(it, dist(ui_set_font_gen_toggle));
                            game.setCurrentFont(fontManager.getFont(*it));
                        } else if (!fontManager.getAvailableFonts().empty()) {
                            game.setCurrentFont(fontManager.getRandomFont());
                        } else {
                            game.setCurrentFont(fontManager.getDefaultFont());
                        }
                    }
                } else if (buttonText.find("Randomize Word Sets:") == 0) {
                    settingsManager.setRandomizeWordSets(!settingsManager.randomizeWordSets());
                    if (!settingsManager.randomizeWordSets()) {
                        if (!settingsManager.getRandomizedWordSetIndices().empty()) {
                            wordDBManager.setCurrentWordSetByIndex(*settingsManager.getRandomizedWordSetIndices().begin());
                        } else if (!wordDBManager.getAllWordSets().empty()) {
                            wordDBManager.setCurrentWordSetByIndex(0);
                        }
                    }
                } else if (buttonText == "Edit Randomized Word Sets" && settingsManager.randomizeWordSets()) {
                    game.setGameState(MonkeyTyperGame::GameState::WORD_SET_SELECTION);
                }else {
                    settingChangedProcessed = false;
                }

                if (settingChangedProcessed) {
                    refreshSettingsButtons();
                }
                return;
            }
        }
    }
}


void UIManager::processFontSelectionEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

        if (fontScreenBackButton.contains(mousePos)) {
            game.setGameState(MonkeyTyperGame::GameState::SETTINGS);
            refreshSettingsButtons();
            return;
        }

        sf::RectangleShape localFsPanelDef;
        localFsPanelDef.setPosition(sf::Vector2f(100.0f, 130.0f));
        localFsPanelDef.setSize(sf::Vector2f(GameConstants::WINDOW_WIDTH - 200.0f, GameConstants::WINDOW_HEIGHT - 250.0f));

        float headerH = 20.f;
        float lineH = 30.f;
        float listStartOffsetY = headerH + lineH + 30.f;
        float listContentStartYPos = localFsPanelDef.getPosition().y + listStartOffsetY;

        int i = 0;
        for (const auto& [name, fontPtr] : fontManager.getAvailableFonts()) {
            float yPos = listContentStartYPos + i * 50.0f;
            sf::FloatRect itemRect(
                    localFsPanelDef.getPosition().x + 20.f,
                    yPos -5.0f,
                    localFsPanelDef.getSize().x - 40.f,
                    40.0f
            );
            if (itemRect.contains(mousePos)) {
                game.setCurrentFont(fontPtr);
                break;
            }
            i++;
        }
    }
}

void UIManager::processFontRandomizationEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

        if (fontScreenBackButton.contains(mousePos)) {
            game.setGameState(MonkeyTyperGame::GameState::SETTINGS);
            refreshSettingsButtons();
            return;
        }

        sf::RectangleShape localFrPanelDef;
        localFrPanelDef.setPosition(sf::Vector2f(100.0f, 130.0f));
        localFrPanelDef.setSize(sf::Vector2f(GameConstants::WINDOW_WIDTH - 200.0f, GameConstants::WINDOW_HEIGHT - 250.0f));
        float headerH = 20.f;
        float lineH = 30.f;
        float listStartOffsetY = headerH + lineH + 30.f;
        float listContentStartYPos = localFrPanelDef.getPosition().y + listStartOffsetY;
        int i = 0;
        for (const auto& [name, fontPtr] : fontManager.getAvailableFonts()) {
            float yPos = listContentStartYPos + i * 50.0f;
            sf::FloatRect itemRect(localFrPanelDef.getPosition().x + 20.f, yPos - 5.0f, localFrPanelDef.getSize().x - 40.f, 40.0f);
            if (itemRect.contains(mousePos)) {
                settingsManager.toggleRandomizedFont(name);
                break;
            }
            i++;
        }
    }
}

void UIManager::processWordSetSelectionEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

        if (wordSetScreenBackButton.contains(mousePos)) {
            game.setGameState(MonkeyTyperGame::GameState::SETTINGS);
            refreshSettingsButtons();
            return;
        }

        sf::RectangleShape localWssPanelDef;
        localWssPanelDef.setPosition(sf::Vector2f(100.0f, 130.0f));
        localWssPanelDef.setSize(sf::Vector2f(GameConstants::WINDOW_WIDTH - 200.0f, GameConstants::WINDOW_HEIGHT - 250.0f));
        float headerH = 20.f;
        float lineH = 30.f;
        float listStartOffsetY = headerH + lineH + 30.f;
        float listContentStartYPos = localWssPanelDef.getPosition().y + listStartOffsetY;

        for (size_t i = 0; i < wordDBManager.getAllWordSets().size(); ++i) {
            float yPos = listContentStartYPos + i * 50.0f;
            sf::FloatRect itemRect(localWssPanelDef.getPosition().x + 20.f, yPos - 5.0f, localWssPanelDef.getSize().x - 40.f, 40.0f);
            if (itemRect.contains(mousePos)) {
                if (settingsManager.randomizeWordSets()) {
                    settingsManager.toggleRandomizedWordSet(i);
                } else {
                    wordDBManager.setCurrentWordSetByIndex(i);
                    settingsManager.clearRandomizedWordSets();
                    settingsManager.toggleRandomizedWordSet(i);
                }
                break;
            }
        }
    }
}

void UIManager::processGraphViewEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        if (graphViewBackButton.contains(mousePos)) {
            game.setGameState(MonkeyTyperGame::GameState::HIGH_SCORES);
        }
    }
    if (event.type == sf::Event::KeyPressed) {
        game.setGameState(MonkeyTyperGame::GameState::HIGH_SCORES);
    }
}

void UIManager::renderMenu() {
    sf::RectangleShape bg(sf::Vector2f(GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT));
    bg.setFillColor(sf::Color(30, 30, 50));
    window.draw(bg);

    auto currentFont = game.getCurrentFont();
    if (!currentFont) currentFont = fontManager.getDefaultFont();
    sf::Text menuTitle("Monkey Typer", *currentFont, 60);
    menuTitle.setFillColor(sf::Color::Yellow);
    sf::FloatRect currentMenuTitleLocalBounds = menuTitle.getLocalBounds();
    menuTitle.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH / 2.0f - currentMenuTitleLocalBounds.width / 2.0f, 100.0f));
    window.draw(menuTitle);

    for (const auto& button : menuButtons) {
        button.draw(window);
    }
}

void UIManager::renderGameOver(const GameConstants::GameStats& gameStats, int finalScoreVal, const std::deque<ScorePoint>& scoreHistory, const std::shared_ptr<sf::Font>& currentFontRef) {
    auto currentFont = currentFontRef;
    if (!currentFont) currentFont = fontManager.getDefaultFont();

    sf::RectangleShape bg(sf::Vector2f(GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT));
    bg.setFillColor(sf::Color(40, 30, 30));
    window.draw(bg);

    sf::FloatRect currentGameOverHeaderTextLocalBounds = gameOverHeaderText.getLocalBounds();
    gameOverHeaderText.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH / 2.0f - currentGameOverHeaderTextLocalBounds.width / 2.0f, 50.0f));
    window.draw(gameOverHeaderText);

    sf::RectangleShape statsDisplayPanel;
    statsDisplayPanel.setSize(sf::Vector2f(800, 450));
    statsDisplayPanel.setFillColor(sf::Color(50, 50, 60, 220));
    statsDisplayPanel.setOutlineColor(sf::Color(100, 100, 120));
    statsDisplayPanel.setOutlineThickness(2);
    statsDisplayPanel.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH / 2.0f - statsDisplayPanel.getSize().x / 2.0f,
                                               GameConstants::WINDOW_HEIGHT / 2.0f - statsDisplayPanel.getSize().y / 2.0f - 20.f));
    window.draw(statsDisplayPanel);

    finalScoreText.setString(fmt::format("Final Score: {}", finalScoreVal));
    finalScoreText.setPosition(sf::Vector2f(statsDisplayPanel.getPosition().x + 20.0f, statsDisplayPanel.getPosition().y + 20.0f));
    window.draw(finalScoreText);

    if (game.isNewHighScoreEligible(finalScoreVal) && !game.getHighScoreNamePromptAttemptedFlag() && !enteringHighScoreName) {
        enteringHighScoreName = true;
        playerNameInput.clear();
        game.setHighScoreNamePromptAttemptedFlag(true);
    }


    sf::Text highScoreDisplayText;
    highScoreDisplayText.setFont(*currentFont);
    highScoreDisplayText.setCharacterSize(28);
    if (game.isNewHighScoreEligible(finalScoreVal)) {
        highScoreDisplayText.setString("NEW HIGH SCORE!");
        highScoreDisplayText.setFillColor(sf::Color(255, 215, 0));
    } else {
        int topOverallScore = highScoreManager.getHighScores().empty() ? 0 : highScoreManager.getHighScores()[0].second;
        highScoreDisplayText.setString(fmt::format("Overall High: {}", topOverallScore));
        highScoreDisplayText.setFillColor(sf::Color::White);
    }
    sf::FloatRect finalScoreTextGlobalBounds = finalScoreText.getGlobalBounds();
    highScoreDisplayText.setPosition(sf::Vector2f(finalScoreText.getPosition().x + finalScoreTextGlobalBounds.width + 50.f, finalScoreText.getPosition().y));
    window.draw(highScoreDisplayText);

    sf::FloatRect graphRenderArea(
            statsDisplayPanel.getPosition().x + 20.0f,
            statsDisplayPanel.getPosition().y + 80.0f,
            statsDisplayPanel.getSize().x / 2.0f - 30.0f,
            200.0f
    );
    renderScoreGraph(scoreHistory, graphRenderArea, currentFont, "This Game's Progress");

    const float statsTextStartX = statsDisplayPanel.getPosition().x + statsDisplayPanel.getSize().x / 2.0f + 10.0f;
    const float statsTextStartY = graphRenderArea.top;
    const float statRowHeight = 25.0f;
    const unsigned int statFontSize = 18;

    std::vector<std::pair<std::string, std::string>> statsToDisplayList = {
            {"Words/min", fmt::format("{:.1f}", gameStats.wordsPerMinute)},
            {"Accuracy", fmt::format("{:.1f}%", gameStats.accuracy)},
            {"Completed", fmt::format("{} words", gameStats.wordsTyped)},
            {"Typed", fmt::format("{} chars", gameStats.charactersTyped)},
            {"Mistakes", fmt::format("{}", gameStats.mistakesMade)},
            {"Time", fmt::format("{:.1f}s", game.getGameTime())},
    };
    if (gameStats.wordsTyped > 0) {
        statsToDisplayList.push_back({"Avg Word Len", fmt::format("{:.1f}", static_cast<float>(gameStats.charactersTyped) / gameStats.wordsTyped)});
    } else {
        statsToDisplayList.push_back({"Avg Word Len", "N/A"});
    }


    for (size_t i = 0; i < statsToDisplayList.size(); ++i) {
        float y = statsTextStartY + i * statRowHeight;
        if (y + statRowHeight > statsDisplayPanel.getPosition().y + statsDisplayPanel.getSize().y - 80.f) break;

        sf::Text label(statsToDisplayList[i].first + ":", *currentFont, statFontSize);
        label.setFillColor(sf::Color(180, 180, 180));
        label.setPosition(sf::Vector2f(statsTextStartX, y));
        window.draw(label);

        sf::Text value(statsToDisplayList[i].second, *currentFont, statFontSize);
        value.setFillColor(sf::Color::White);
        value.setPosition(sf::Vector2f(statsTextStartX + 150, y));
        window.draw(value);
    }

    float buttonYPos = statsDisplayPanel.getPosition().y + statsDisplayPanel.getSize().y - 60.f;
    float totalButtonsAreaWidth = 0;
    std::vector<float> gameOverButtonWidths;
    for(const auto& btn : gameOverButtons) gameOverButtonWidths.push_back(btn.getGlobalBounds().width);
    for(float w : gameOverButtonWidths) totalButtonsAreaWidth += w;
    float buttonLayoutSpacingGameOver = 30.f;
    if (gameOverButtons.size() > 1) totalButtonsAreaWidth += (gameOverButtons.size() -1) * buttonLayoutSpacingGameOver;

    float currentBtnLayoutX = statsDisplayPanel.getPosition().x + (statsDisplayPanel.getSize().x - totalButtonsAreaWidth) / 2.f;

    for (size_t i = 0; i < gameOverButtons.size(); ++i) {
        Button btnToDraw = gameOverButtons[i];
        sf::FloatRect btnTextActualLocalBounds = btnToDraw.text.getLocalBounds();
        float textTargetXBtn = currentBtnLayoutX + (gameOverButtonWidths[i] / 2.f) - (btnTextActualLocalBounds.width / 2.f);

        btnToDraw.setPosition(sf::Vector2f(textTargetXBtn, buttonYPos));
        btnToDraw.draw(window);
        currentBtnLayoutX += gameOverButtonWidths[i] + buttonLayoutSpacingGameOver;
    }

    if (enteringHighScoreName) {
        sf::RectangleShape overlay(sf::Vector2f(GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(overlay);
        sf::Vector2f boxSize(400, 120);
        sf::Vector2f boxPos(
                GameConstants::WINDOW_WIDTH / 2.0f - boxSize.x / 2.0f,
                GameConstants::WINDOW_HEIGHT / 2.0f - boxSize.y / 2.0f
        );
        sf::RectangleShape nameInputBox(boxSize);
        nameInputBox.setFillColor(sf::Color(60, 60, 70, 220));
        nameInputBox.setOutlineColor(sf::Color::White);
        nameInputBox.setOutlineThickness(2);
        nameInputBox.setPosition(boxPos);
        window.draw(nameInputBox);
        sf::Text prompt("Enter your name (ESC to skip):", *currentFont, 28);
        sf::FloatRect promptCurrentLocalBounds = prompt.getLocalBounds();
        prompt.setPosition(sf::Vector2f(boxPos.x + boxSize.x / 2.0f - promptCurrentLocalBounds.width / 2.0f, boxPos.y + 15));
        window.draw(prompt);
        sf::Text nameDisplayText(playerNameInput + "_", *currentFont, 30);
        sf::FloatRect nameDisplayCurrentLocalBounds = nameDisplayText.getLocalBounds();
        nameDisplayText.setPosition(sf::Vector2f(boxPos.x + boxSize.x / 2.0f - nameDisplayCurrentLocalBounds.width / 2.0f, boxPos.y + 60));
        window.draw(nameDisplayText);
    }
}

void UIManager::renderScoreGraph(const std::deque<ScorePoint>& scoreHistory, const sf::FloatRect& graphBounds, const std::shared_ptr<sf::Font>& currentFontRef, const std::string& titleStr) {
    auto currentFont = currentFontRef;
    if (!currentFont) currentFont = fontManager.getDefaultFont();
    if (scoreHistory.empty()) return;

    sf::RectangleShape graphBg(sf::Vector2f(graphBounds.width, graphBounds.height));
    graphBg.setPosition(graphBounds.left, graphBounds.top);
    graphBg.setFillColor(sf::Color(40, 40, 50));
    graphBg.setOutlineColor(sf::Color(100, 100, 120));
    graphBg.setOutlineThickness(1);
    window.draw(graphBg);

    sf::VertexArray yAxis(sf::Lines, 2);
    yAxis[0].position = sf::Vector2f(graphBounds.left, graphBounds.top);
    yAxis[0].color = sf::Color(100,100,120);
    yAxis[1].position = sf::Vector2f(graphBounds.left, graphBounds.top + graphBounds.height);
    yAxis[1].color = sf::Color(100,100,120);
    window.draw(yAxis);

    sf::VertexArray xAxis(sf::Lines, 2);
    xAxis[0].position = sf::Vector2f(graphBounds.left, graphBounds.top + graphBounds.height);
    xAxis[0].color = sf::Color(100,100,120);
    xAxis[1].position = sf::Vector2f(graphBounds.left + graphBounds.width, graphBounds.top + graphBounds.height);
    xAxis[1].color = sf::Color(100,100,120);
    window.draw(xAxis);

    float maxScoreVal = 0;
    for (const auto& point : scoreHistory) maxScoreVal = std::max(maxScoreVal, static_cast<float>(point.score));
    if (maxScoreVal < 1.f) maxScoreVal = 1.f;

    int numGridLinesY = 5;
    for (int i = 0; i <= numGridLinesY; ++i) {
        float ratio = static_cast<float>(i) / numGridLinesY;
        float yLinePos = graphBounds.top + graphBounds.height * (1.0f - ratio) ;

        if (i > 0 && i < numGridLinesY) {
            sf::VertexArray gridLine(sf::Lines, 2);
            gridLine[0].position = sf::Vector2f(graphBounds.left, yLinePos);
            gridLine[0].color = sf::Color(60,60,70, 150);
            gridLine[1].position = sf::Vector2f(graphBounds.left + graphBounds.width, yLinePos);
            gridLine[1].color = sf::Color(60,60,70, 150);
            window.draw(gridLine);
        }

        sf::Text yLabel(fmt::format("{}", static_cast<int>(maxScoreVal * ratio)), *currentFont, 10);
        yLabel.setFillColor(sf::Color(180,180,180));
        sf::FloatRect yLabelLocalBounds = yLabel.getLocalBounds();
        yLabel.setPosition(sf::Vector2f(graphBounds.left - yLabelLocalBounds.width - 8, yLinePos - yLabelLocalBounds.height / 1.5f -2));
        window.draw(yLabel);
    }
    sf::Text yAxisTitleText("Score", *currentFont, 12);
    yAxisTitleText.setFillColor(sf::Color(200,200,200));
    yAxisTitleText.setRotation(-90);
    sf::FloatRect yAxisTitleTextLocalBounds = yAxisTitleText.getLocalBounds();
    yAxisTitleText.setPosition(sf::Vector2f(graphBounds.left - 30 - yAxisTitleTextLocalBounds.height/2.f,
                                            graphBounds.top + graphBounds.height / 2 + yAxisTitleTextLocalBounds.width / 2));
    window.draw(yAxisTitleText);

    float firstTimestamp = scoreHistory.front().timeStamp;
    float lastTimestamp = scoreHistory.back().timeStamp;
    float timeRange = lastTimestamp - firstTimestamp;
    if (timeRange < 0.1f) timeRange = 0.1f;

    int numGridLinesX = std::min(5, static_cast<int>(timeRange / 10.0f) + 1);
    if (numGridLinesX < 2 && timeRange > 1.0f) numGridLinesX = 2;
    if (timeRange <= 1.0f && numGridLinesX < 1) numGridLinesX = 1;


    for (int i = 0; i <= numGridLinesX; ++i) {
        float ratio = (numGridLinesX > 0) ? (static_cast<float>(i) / numGridLinesX) : 0.f;
        float xLinePos = graphBounds.left + ratio * graphBounds.width;

        if (i > 0 && i < numGridLinesX) {
            sf::VertexArray gridLine(sf::Lines, 2);
            gridLine[0].position = sf::Vector2f(xLinePos, graphBounds.top);
            gridLine[0].color = sf::Color(60,60,70,150);
            gridLine[1].position = sf::Vector2f(xLinePos, graphBounds.top + graphBounds.height);
            gridLine[1].color = sf::Color(60,60,70,150);
            window.draw(gridLine);
        }
        float timeLabelVal = firstTimestamp + ratio * timeRange;
        sf::Text xLabel(fmt::format("{:.0f}s", timeLabelVal), *currentFont, 10);
        xLabel.setFillColor(sf::Color(150,150,150));
        sf::FloatRect xLabelLocalBounds = xLabel.getLocalBounds();
        xLabel.setPosition(sf::Vector2f(xLinePos - xLabelLocalBounds.width/2.f, graphBounds.top + graphBounds.height + 5));
        window.draw(xLabel);
    }
    sf::Text xAxisTitleText("Time (s)", *currentFont, 12);
    xAxisTitleText.setFillColor(sf::Color(200,200,200));
    sf::FloatRect xAxisTitleTextLocalBounds = xAxisTitleText.getLocalBounds();
    xAxisTitleText.setPosition(sf::Vector2f(graphBounds.left + graphBounds.width/2.f - xAxisTitleTextLocalBounds.width/2.f,
                                            graphBounds.top + graphBounds.height + 20.f + ( (numGridLinesX>0) ? 12.f:0.f) ));
    window.draw(xAxisTitleText);


    if (scoreHistory.size() >= 2) {
        sf::VertexArray lines(sf::LineStrip, scoreHistory.size());
        for (size_t i = 0; i < scoreHistory.size(); ++i) {
            const auto& point = scoreHistory[i];
            float xRatio = (timeRange > 0.001f) ? ((point.timeStamp - firstTimestamp) / timeRange) : (scoreHistory.size() > 1 ? static_cast<float>(i) / (scoreHistory.size()-1) : 0.5f);
            float x = graphBounds.left + xRatio * graphBounds.width;
            float yPlot = graphBounds.top + graphBounds.height - (static_cast<float>(point.score) / maxScoreVal) * graphBounds.height;
            yPlot = std::max(graphBounds.top, std::min(yPlot, graphBounds.top + graphBounds.height));
            lines[i].position = sf::Vector2f(x, yPlot);
            lines[i].color = sf::Color(100, 255, 100, 220);
        }
        window.draw(lines);

        for(size_t i=0; i<scoreHistory.size(); ++i) {
            if (scoreHistory.size() <= 20 || i % std::max(1, static_cast<int>(scoreHistory.size() / 15)) == 0 || i == scoreHistory.size() -1) {
                sf::CircleShape pointCircle(2.5f);
                pointCircle.setFillColor(sf::Color::Cyan);
                pointCircle.setOutlineColor(sf::Color::Black);
                pointCircle.setOutlineThickness(0.5f);
                pointCircle.setOrigin(sf::Vector2f(2.5f, 2.5f));
                pointCircle.setPosition(lines[i].position);
                window.draw(pointCircle);
            }
        }
    }

    sf::Text graphTitleTextDisplay(titleStr, *currentFont, 16);
    graphTitleTextDisplay.setFillColor(sf::Color(200, 200, 200));
    graphTitleTextDisplay.setPosition(sf::Vector2f(graphBounds.left + 5, graphBounds.top - 20));
    window.draw(graphTitleTextDisplay);
}

void UIManager::renderHighScores() {
    auto currentFont = game.getCurrentFont();
    if (!currentFont) currentFont = fontManager.getDefaultFont();

    sf::Text hsScreenTitle("High Scores", *currentFont, 60);
    hsScreenTitle.setFillColor(sf::Color::Yellow);
    sf::FloatRect hsScreenTitleLocalBounds = hsScreenTitle.getLocalBounds();
    hsScreenTitle.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH / 2.0f - hsScreenTitleLocalBounds.width / 2.0f, 50.0f));
    window.draw(hsScreenTitle);

    sf::RectangleShape hsContentPanel;
    hsContentPanel.setSize(sf::Vector2f(800, 500));
    hsContentPanel.setFillColor(sf::Color(50, 50, 60, 220));
    hsContentPanel.setOutlineColor(sf::Color(100, 100, 120));
    hsContentPanel.setOutlineThickness(2);
    hsContentPanel.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH / 2.0f - hsContentPanel.getSize().x / 2.0f, 150.0f));
    window.draw(hsContentPanel);

    const auto& scores = highScoreManager.getHighScores();
    float headerY = hsContentPanel.getPosition().y + 20.0f;
    float listStartY = headerY + 70.0f;

    if (scores.empty()) {
        sf::Text noScoresText("No high scores yet!", *currentFont, 40);
        sf::FloatRect noScoresTextLocalBounds = noScoresText.getLocalBounds();
        noScoresText.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH / 2.0f - noScoresTextLocalBounds.width / 2.0f,
                                              hsContentPanel.getPosition().y + hsContentPanel.getSize().y / 2.f - noScoresTextLocalBounds.height / 2.f));
        window.draw(noScoresText);
    } else {
        std::vector<std::pair<std::string, float>> headers = {
                {"RANK", hsContentPanel.getPosition().x + 50.0f},
                {"NAME", hsContentPanel.getPosition().x + 150.0f},
                {"SCORE", hsContentPanel.getPosition().x + hsContentPanel.getSize().x - 150.0f - 50.f}
        };
        for(const auto& headerItem : headers) {
            sf::Text headerItemText(headerItem.first, *currentFont, 28);
            headerItemText.setFillColor(sf::Color::Yellow);
            if (headerItem.first == "SCORE") {
                sf::FloatRect headerItemTextLocalBounds = headerItemText.getLocalBounds();
                headerItemText.setPosition(sf::Vector2f(headerItem.second - headerItemTextLocalBounds.width/2.f + 50.f, headerY));
            } else {
                headerItemText.setPosition(sf::Vector2f(headerItem.second, headerY));
            }
            window.draw(headerItemText);
        }
        sf::RectangleShape headerContentLine(sf::Vector2f(hsContentPanel.getSize().x - 40.f, 2));
        headerContentLine.setFillColor(sf::Color(100, 100, 120));
        headerContentLine.setPosition(sf::Vector2f(hsContentPanel.getPosition().x + 20.f, headerY + 40.0f));
        window.draw(headerContentLine);

        for (size_t i = 0; i < scores.size(); ++i) {
            float yPos = listStartY + i * 40.0f;
            float navButtonHeight = highScoresNavigationButtons.empty() ? 0.f : highScoresNavigationButtons[0].getGlobalBounds().height;
            if (yPos + 20.f > hsContentPanel.getPosition().y + hsContentPanel.getSize().y - (navButtonHeight + 20.f) ) break;

            if (static_cast<int>(i) == selectedScoreIndex) {
                sf::RectangleShape selectedHighlightRect(sf::Vector2f(hsContentPanel.getSize().x - 40.f, 35));
                selectedHighlightRect.setFillColor(sf::Color(100, 100, 120, 100));
                selectedHighlightRect.setPosition(sf::Vector2f(hsContentPanel.getPosition().x + 20.f, yPos - 5.0f));
                window.draw(selectedHighlightRect);
            }

            sf::Color itemTextColor = sf::Color::White;
            if (i < 3) {
                const sf::Color medalColors[] = {sf::Color(255, 215, 0), sf::Color(192, 192, 192), sf::Color(205, 127, 50)};
                itemTextColor = medalColors[i];
            }

            sf::Text rankDisplayText(fmt::format("{}.", i + 1), *currentFont, 24);
            rankDisplayText.setFillColor(itemTextColor);
            rankDisplayText.setPosition(sf::Vector2f(headers[0].second, yPos));
            window.draw(rankDisplayText);

            sf::Text nameDisplayTextItem(scores[i].first, *currentFont, 24);
            nameDisplayTextItem.setFillColor(itemTextColor);
            std::string currentDisplayNameStr = scores[i].first;
            float nameColumnActualWidth = headers[2].second - headers[1].second - 50.f;
            nameDisplayTextItem.setString(currentDisplayNameStr);
            while(nameDisplayTextItem.getLocalBounds().width > nameColumnActualWidth && currentDisplayNameStr.length() > 5){
                currentDisplayNameStr.pop_back();
                nameDisplayTextItem.setString(currentDisplayNameStr + "..");
            }
            if(currentDisplayNameStr.empty() && !scores[i].first.empty()){
                nameDisplayTextItem.setString(scores[i].first.substr(0, std::max(0, (int)nameColumnActualWidth / 12)) + "..");
            }
            nameDisplayTextItem.setPosition(sf::Vector2f(headers[1].second, yPos));
            window.draw(nameDisplayTextItem);

            sf::Text scoreValDisplayText(fmt::format("{}", scores[i].second), *currentFont, 24);
            scoreValDisplayText.setFillColor(itemTextColor);
            sf::FloatRect currentScoreTextLocalBounds = scoreValDisplayText.getLocalBounds();
            scoreValDisplayText.setPosition(sf::Vector2f(headers[2].second + 100.f - currentScoreTextLocalBounds.width, yPos));
            window.draw(scoreValDisplayText);
        }
    }

    if (!highScoresNavigationButtons.empty()) {
        Button& backBtnRef = highScoresNavigationButtons[0];
        float commonNavY = hsContentPanel.getPosition().y + hsContentPanel.getSize().y + 30.f;
        float navButtonHeight = backBtnRef.getGlobalBounds().height;
        if (commonNavY + navButtonHeight > GameConstants::WINDOW_HEIGHT - 10.f) {
            commonNavY = GameConstants::WINDOW_HEIGHT - navButtonHeight - 10.f;
        }

        if (highScoresNavigationButtons.size() > 1) {
            Button& graphBtnRef = highScoresNavigationButtons[1];
            float backBtnNavWidth = backBtnRef.getGlobalBounds().width;
            float graphBtnNavWidth = graphBtnRef.getGlobalBounds().width;
            float navSpacing = 20.f;
            float totalNavLayoutWidth = backBtnNavWidth + graphBtnNavWidth + navSpacing;
            float navLayoutStartX = (GameConstants::WINDOW_WIDTH - totalNavLayoutWidth) / 2.f;

            sf::FloatRect backBtnTextLocalBounds = backBtnRef.text.getLocalBounds();
            backBtnRef.setPosition(sf::Vector2f(navLayoutStartX + backBtnNavWidth/2.f - backBtnTextLocalBounds.width/2.f, commonNavY));

            sf::FloatRect graphBtnTextLocalBounds = graphBtnRef.text.getLocalBounds();
            graphBtnRef.setPosition(sf::Vector2f(navLayoutStartX + backBtnNavWidth + navSpacing + graphBtnNavWidth/2.f - graphBtnTextLocalBounds.width/2.f, commonNavY));
        } else {
            sf::FloatRect backBtnTextLocalBounds = backBtnRef.text.getLocalBounds();
            backBtnRef.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH/2.f - backBtnTextLocalBounds.width/2.f, commonNavY));
        }
    }
    for(const auto& button : highScoresNavigationButtons) button.draw(window);


    sf::Text hsScreenPrompt("Click score for details. Press any key to return.", *currentFont, 20);
    sf::FloatRect hsScreenPromptLocalBounds = hsScreenPrompt.getLocalBounds();
    hsScreenPrompt.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH / 2.0f - hsScreenPromptLocalBounds.width / 2.0f,
                                            GameConstants::WINDOW_HEIGHT - hsScreenPrompt.getCharacterSize() - 15.f));
    window.draw(hsScreenPrompt);
}

void UIManager::renderSettings() {
    auto currentFont = game.getCurrentFont();
    if (!currentFont) currentFont = fontManager.getDefaultFont();
    sf::Text settingsScreenTitle("Settings", *currentFont, 60);
    settingsScreenTitle.setFillColor(sf::Color::Yellow);
    sf::FloatRect settingsTitleLocalBounds = settingsScreenTitle.getLocalBounds();
    settingsScreenTitle.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH / 2.0f - settingsTitleLocalBounds.width / 2.0f, 50.0f));
    window.draw(settingsScreenTitle);

    for (const auto& button : settingsButtons) {
        button.draw(window);
    }

    const float animPanelAreaX = GameConstants::WINDOW_WIDTH - 470.0f;
    const float animPanelW = 420.0f;
    const float animPanelH = 100.0f;
    const float animPanelMargin = 20.0f;

    for (size_t i = 0; i < settingsAnimations.size(); ++i) {
        float currentAnimPanelY = 120.0f + i * (animPanelH + animPanelMargin);
        if (currentAnimPanelY + animPanelH > GameConstants::WINDOW_HEIGHT - 20.f) break;

        sf::RectangleShape animPanelShape(sf::Vector2f(animPanelW, animPanelH));
        animPanelShape.setFillColor(sf::Color(50, 50, 60, 220));
        animPanelShape.setOutlineColor(sf::Color(100, 100, 120));
        animPanelShape.setOutlineThickness(1);
        animPanelShape.setPosition(sf::Vector2f(animPanelAreaX, currentAnimPanelY));
        window.draw(animPanelShape);

        sf::Text exampleTitleTextObj;
        exampleTitleTextObj.setFont(*currentFont);
        exampleTitleTextObj.setCharacterSize(16);
        exampleTitleTextObj.setFillColor(sf::Color(180, 180, 180));

        std::string currentExampleTitleString;
        switch (i) {
            case 0: currentExampleTitleString = "Speed Example (Visual Only)"; break;
            case 1: currentExampleTitleString = "Spawn Rate Example (Visual Only)"; break;
            case 2: currentExampleTitleString = "Font Size Example"; break;
            case 3: currentExampleTitleString = "Highlight Example"; break;
            default: currentExampleTitleString = "Example"; break;
        }
        exampleTitleTextObj.setString(currentExampleTitleString);
        exampleTitleTextObj.setPosition(sf::Vector2f(animPanelAreaX + 10, currentAnimPanelY + 10));
        window.draw(exampleTitleTextObj);

        const TextAnimation& anim = settingsAnimations[i];
        std::string displayText = anim.getCurrentText();
        sf::Text exampleAnimTextObj(displayText, *currentFont, 24);
        exampleAnimTextObj.setFillColor(sf::Color::White);

        if (i == 2) {
            exampleAnimTextObj.setCharacterSize(settingsManager.currentFontSize);
        } else if (i == 3 && settingsManager.highlightTyping && anim.isTypingPhase()) {
            std::string typedPart = displayText.substr(0, displayText.find('_'));
            if (typedPart.length() > anim.text.length()) typedPart = anim.text.substr(0, std::min(typedPart.length(), anim.text.length()));

            sf::Text fullWordDisplay(anim.text, *currentFont, 24);
            fullWordDisplay.setFillColor(sf::Color::White);
            fullWordDisplay.setPosition(sf::Vector2f(animPanelAreaX + 20, currentAnimPanelY + 50));
            window.draw(fullWordDisplay);

            sf::Text typedHighlightDisplay(typedPart, *currentFont, 24);
            typedHighlightDisplay.setFillColor(sf::Color::Yellow);
            typedHighlightDisplay.setPosition(sf::Vector2f(animPanelAreaX + 20, currentAnimPanelY + 50));
            window.draw(typedHighlightDisplay);
            continue;
        }

        exampleAnimTextObj.setPosition(sf::Vector2f(animPanelAreaX + 20, currentAnimPanelY + 50));
        window.draw(exampleAnimTextObj);
    }
}

void UIManager::renderFontSelection() {
    auto uiFont = game.getCurrentFont();
    if (!uiFont) uiFont = fontManager.getDefaultFont();

    sf::RectangleShape bg(sf::Vector2f(GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT));
    bg.setFillColor(sf::Color(40, 30, 30)); window.draw(bg);

    sf::Text screenTitle("Select Font", *uiFont, 60);
    screenTitle.setFillColor(sf::Color::Yellow);
    sf::FloatRect screenTitleLocalBounds = screenTitle.getLocalBounds();
    screenTitle.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH / 2.0f - screenTitleLocalBounds.width / 2.0f, 50.0f));
    window.draw(screenTitle);

    sf::RectangleShape localScreenPanel;
    localScreenPanel.setSize(sf::Vector2f(GameConstants::WINDOW_WIDTH - 200.0f, GameConstants::WINDOW_HEIGHT - 250.0f));
    localScreenPanel.setFillColor(sf::Color(50, 50, 60, 220));
    localScreenPanel.setOutlineColor(sf::Color(100, 100, 120));
    localScreenPanel.setOutlineThickness(2);
    localScreenPanel.setPosition(sf::Vector2f(100.f,130.f));
    window.draw(localScreenPanel);

    float headerContentY = localScreenPanel.getPosition().y + 20.f;
    sf::Text nameColHeader("FONT NAME", *uiFont, 24);
    nameColHeader.setFillColor(sf::Color::Yellow);
    nameColHeader.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + 50.f, headerContentY));
    window.draw(nameColHeader);
    sf::Text sampleColHeader("SAMPLE TEXT", *uiFont, 24);
    sampleColHeader.setFillColor(sf::Color::Yellow);
    sampleColHeader.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + localScreenPanel.getSize().x / 2.f, headerContentY));
    window.draw(sampleColHeader);
    sf::RectangleShape headerLine(sf::Vector2f(localScreenPanel.getSize().x - 40.f, 2));
    headerLine.setFillColor(sf::Color(100, 100, 120));
    headerLine.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + 20.f, headerContentY + 30.0f));
    window.draw(headerLine);

    int itemIndex = 0;
    const auto& gameActiveFont = game.getCurrentFont();
    float listContentStartY = headerContentY + 60.f;

    for (const auto& [fontEntryName, fontEntryPtr] : fontManager.getAvailableFonts()) {
        float yPos = listContentStartY + itemIndex * 50.0f;
        float backButtonHeight = fontScreenBackButton.getGlobalBounds().height;
        if (yPos + 40.f > localScreenPanel.getPosition().y + localScreenPanel.getSize().y - backButtonHeight - 10.f) break;

        bool isActiveGameFontCurrently = (fontEntryPtr == gameActiveFont);

        if (isActiveGameFontCurrently && !settingsManager.randomizeFonts()) {
            sf::RectangleShape selectedHighlight(sf::Vector2f(localScreenPanel.getSize().x - 40.f, 40.0f));
            selectedHighlight.setFillColor(sf::Color(70, 100, 70, 100));
            selectedHighlight.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + 20.f, yPos -5.0f));
            window.draw(selectedHighlight);
        }

        sf::Text fontItemNameText(fontEntryName, *uiFont, 24);
        fontItemNameText.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + 50.f, yPos));
        sf::Text fontSampleDisplayText("The quick brown fox", *fontEntryPtr, 24);
        fontSampleDisplayText.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + localScreenPanel.getSize().x / 2.f, yPos));

        sf::Color currentItemTextColorToDisplay = (isActiveGameFontCurrently && !settingsManager.randomizeFonts()) ? sf::Color::Green : sf::Color::White;
        fontItemNameText.setFillColor(currentItemTextColorToDisplay);
        fontSampleDisplayText.setFillColor(currentItemTextColorToDisplay);
        window.draw(fontItemNameText);
        window.draw(fontSampleDisplayText);
        itemIndex++;
    }
    fontScreenBackButton.draw(window);

    sf::Text screenHint("Drop .ttf/.otf in 'fonts' folder and restart.", *uiFont, 20);
    screenHint.setFillColor(sf::Color(150,150,150));
    sf::FloatRect screenHintLocalBounds = screenHint.getLocalBounds();
    screenHint.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH / 2.0f - screenHintLocalBounds.width / 2.0f, GameConstants::WINDOW_HEIGHT - 50.0f));
    window.draw(screenHint);
}

void UIManager::renderFontRandomization() {
    auto uiFont = game.getCurrentFont();
    if (!uiFont) uiFont = fontManager.getDefaultFont();
    sf::RectangleShape bg(sf::Vector2f(GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT));
    bg.setFillColor(sf::Color(40, 30, 30)); window.draw(bg);
    sf::Text screenTitle("Select Fonts for Randomization", *uiFont, 50);
    screenTitle.setFillColor(sf::Color::Yellow);
    sf::FloatRect screenTitleLocalBounds = screenTitle.getLocalBounds();
    screenTitle.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH / 2.0f - screenTitleLocalBounds.width / 2.0f, 50.0f));
    window.draw(screenTitle);
    sf::RectangleShape localScreenPanel;
    localScreenPanel.setSize(sf::Vector2f(GameConstants::WINDOW_WIDTH - 200.0f, GameConstants::WINDOW_HEIGHT - 250.0f));
    localScreenPanel.setFillColor(sf::Color(50, 50, 60, 220));
    localScreenPanel.setOutlineColor(sf::Color(100,100,120));
    localScreenPanel.setOutlineThickness(2);
    localScreenPanel.setPosition(sf::Vector2f(100.f, 130.f)); window.draw(localScreenPanel);

    float headerContentY = localScreenPanel.getPosition().y + 20.f;
    sf::Text nameColHeader("FONT NAME", *uiFont, 24);
    nameColHeader.setFillColor(sf::Color::Yellow);
    nameColHeader.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + 50.f, headerContentY));
    window.draw(nameColHeader);
    sf::Text sampleColHeader("SAMPLE TEXT", *uiFont, 24);
    sampleColHeader.setFillColor(sf::Color::Yellow);
    sampleColHeader.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + localScreenPanel.getSize().x / 2.f, headerContentY));
    window.draw(sampleColHeader);
    sf::RectangleShape headerLine(sf::Vector2f(localScreenPanel.getSize().x - 40.f, 2));
    headerLine.setFillColor(sf::Color(100, 100, 120));
    headerLine.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + 20.f, headerContentY + 30.0f));
    window.draw(headerLine);

    int itemIndex = 0;
    const auto& randomizedFontNames = settingsManager.getRandomizedFontNames();
    float listContentStartY = headerContentY + 60.f;
    for (const auto& [fontEntryName, fontEntryPtr] : fontManager.getAvailableFonts()) {
        float yPos = listContentStartY + itemIndex * 50.0f;
        float backButtonHeight = fontScreenBackButton.getGlobalBounds().height;
        if (yPos + 40.f > localScreenPanel.getPosition().y + localScreenPanel.getSize().y - backButtonHeight - 10.f) break;

        bool isSelectedForRandomPool = randomizedFontNames.count(fontEntryName);
        if (isSelectedForRandomPool) {
            sf::RectangleShape selectedHighlight(sf::Vector2f(localScreenPanel.getSize().x - 40.f, 40.0f));
            selectedHighlight.setFillColor(sf::Color(70, 100, 70, 100));
            selectedHighlight.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + 20.f, yPos - 5.0f));
            window.draw(selectedHighlight);
        }
        sf::Text fontItemNameText(fontEntryName, *uiFont, 24);
        fontItemNameText.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + 50.f, yPos));
        sf::Text fontSampleDisplayText("The quick brown fox", *fontEntryPtr, 24);
        fontSampleDisplayText.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + localScreenPanel.getSize().x / 2.f, yPos));
        sf::Color currentItemTextColor = isSelectedForRandomPool ? sf::Color::Green : sf::Color::White;
        fontItemNameText.setFillColor(currentItemTextColor);
        fontSampleDisplayText.setFillColor(currentItemTextColor);
        window.draw(fontItemNameText);
        window.draw(fontSampleDisplayText);
        itemIndex++;
    }
    fontScreenBackButton.draw(window);
}

void UIManager::renderWordSetSelection() {
    auto uiFont = game.getCurrentFont();
    if (!uiFont) uiFont = fontManager.getDefaultFont();
    sf::RectangleShape bg(sf::Vector2f(GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT));
    bg.setFillColor(sf::Color(40, 30, 30)); window.draw(bg);
    sf::Text screenTitle("Select Word Sets", *uiFont, 46);
    screenTitle.setFillColor(sf::Color::Yellow);
    sf::FloatRect screenTitleLocalBounds = screenTitle.getLocalBounds();
    screenTitle.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH / 2.0f - screenTitleLocalBounds.width / 2.0f, 50.0f));
    window.draw(screenTitle);
    sf::RectangleShape localScreenPanel;
    localScreenPanel.setSize(sf::Vector2f(GameConstants::WINDOW_WIDTH - 200.0f, GameConstants::WINDOW_HEIGHT - 250.0f));
    localScreenPanel.setFillColor(sf::Color(50, 50, 60, 220));
    localScreenPanel.setOutlineColor(sf::Color(100,100,120));
    localScreenPanel.setOutlineThickness(2);
    localScreenPanel.setPosition(sf::Vector2f(100.f, 130.f)); window.draw(localScreenPanel);

    float headerContentY = localScreenPanel.getPosition().y + 20.f;
    sf::Text nameColHeader("WORD SET", *uiFont, 24);
    nameColHeader.setFillColor(sf::Color::Yellow);
    nameColHeader.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + 50.f, headerContentY));
    window.draw(nameColHeader);
    sf::Text sampleColHeader("SAMPLE WORDS", *uiFont, 24);
    sampleColHeader.setFillColor(sf::Color::Yellow);
    sampleColHeader.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + localScreenPanel.getSize().x / 2.f + 20.f, headerContentY)); // Adjusted sample column X
    window.draw(sampleColHeader);
    sf::RectangleShape headerLine(sf::Vector2f(localScreenPanel.getSize().x - 40.f, 2));
    headerLine.setFillColor(sf::Color(100, 100, 120));
    headerLine.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + 20.f, headerContentY + 30.0f));
    window.draw(headerLine);

    const auto& allWordSets = wordDBManager.getAllWordSets();
    const auto& wordSetFilenames = wordDBManager.getWordSetNames();
    const auto& randomizedSetIndices = settingsManager.getRandomizedWordSetIndices();
    float listContentStartY = headerContentY + 60.f;

    for (size_t setIdx = 0; setIdx < allWordSets.size(); ++setIdx) {
        float yPos = listContentStartY + setIdx * 50.0f;
        float backButtonHeight = wordSetScreenBackButton.getGlobalBounds().height;
        if (yPos + 40.f > localScreenPanel.getPosition().y + localScreenPanel.getSize().y - backButtonHeight - 10.f) break;

        const auto& wordSet = allWordSets[setIdx];
        std::string setNameDisplay = (setIdx < wordSetFilenames.size()) ? wordSetFilenames[setIdx] : "Set " + std::to_string(setIdx + 1);

        bool isSelectedCurrently;
        if (settingsManager.randomizeWordSets()) {
            isSelectedCurrently = randomizedSetIndices.count(setIdx);
        } else {
            isSelectedCurrently = (!randomizedSetIndices.empty() && *randomizedSetIndices.begin() == setIdx);
        }

        if (isSelectedCurrently) {
            sf::RectangleShape selectedHighlight(sf::Vector2f(localScreenPanel.getSize().x - 40.f, 40.0f));
            selectedHighlight.setFillColor(sf::Color(70, 100, 70, 100));
            selectedHighlight.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + 20.f, yPos - 5.0f));
            window.draw(selectedHighlight);
        }
        sf::Text setItemNameText(setNameDisplay, *uiFont, 24);
        setItemNameText.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + 50.f, yPos));

        std::string sampleWordsString;
        for(size_t k=0; k < std::min((size_t)3, wordSet.size()); ++k) {
            if (k > 0) sampleWordsString += ", ";
            sampleWordsString += wordSet[k];
            if (sampleWordsString.length() > 35) { sampleWordsString += "..."; break;}
        }
        if (wordSet.empty()) sampleWordsString = "[Empty Set]";

        sf::Text setSampleDisplayText(sampleWordsString, *uiFont, 20);
        setSampleDisplayText.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + localScreenPanel.getSize().x / 2.f + 20.f, yPos));

        sf::Color currentItemTextColorToDisplay = isSelectedCurrently ? sf::Color::Green : sf::Color::White;
        setItemNameText.setFillColor(currentItemTextColorToDisplay);
        setSampleDisplayText.setFillColor(currentItemTextColorToDisplay);
        window.draw(setItemNameText);
        window.draw(setSampleDisplayText);
    }
    wordSetScreenBackButton.draw(window);
}

void UIManager::renderGraphView() {
    auto uiFont = game.getCurrentFont();
    if(!uiFont) uiFont = fontManager.getDefaultFont();
    sf::RectangleShape bg(sf::Vector2f(GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT));
    bg.setFillColor(sf::Color(40, 30, 30)); window.draw(bg);
    sf::RectangleShape topBorderShape(sf::Vector2f(GameConstants::WINDOW_WIDTH, 5));
    topBorderShape.setFillColor(sf::Color(80,80,100)); topBorderShape.setPosition(sf::Vector2f(0.f,0.f)); window.draw(topBorderShape);
    sf::RectangleShape bottomBorderShape(sf::Vector2f(GameConstants::WINDOW_WIDTH, 5));
    bottomBorderShape.setFillColor(sf::Color(80,80,100)); bottomBorderShape.setPosition(sf::Vector2f(0.f, GameConstants::WINDOW_HEIGHT - 5.f)); window.draw(bottomBorderShape);

    sf::Text screenTitle("Performance Analysis", *uiFont, 56);
    screenTitle.setFillColor(sf::Color::Yellow);
    sf::FloatRect screenTitleLocalBounds = screenTitle.getLocalBounds();
    screenTitle.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH / 2.0f - screenTitleLocalBounds.width / 2.0f, 30.0f));
    window.draw(screenTitle);

    sf::RectangleShape localScreenPanel;
    localScreenPanel.setSize(sf::Vector2f(GameConstants::WINDOW_WIDTH * 0.9f, GameConstants::WINDOW_HEIGHT * 0.75f));
    localScreenPanel.setFillColor(sf::Color(25, 25, 35, 230));
    localScreenPanel.setOutlineColor(sf::Color(100, 100, 120, 150));
    localScreenPanel.setOutlineThickness(3);
    localScreenPanel.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH * (1.f - 0.9f) / 2.f, 100.0f));

    if (selectedScoreIndex >= 0 && static_cast<size_t>(selectedScoreIndex) < highScoreManager.getHighScores().size()) {
        window.draw(localScreenPanel);
        const auto& scoreEntry = highScoreManager.getHighScores()[selectedScoreIndex];
        const auto& history = highScoreManager.getScoreHistory(selectedScoreIndex);

        sf::Text playerInfoText(fmt::format("Player: {}  |  Final Score: {}", scoreEntry.first, scoreEntry.second), *uiFont, 28);
        playerInfoText.setFillColor(sf::Color(200,200,255));
        sf::FloatRect playerInfoLocalBounds = playerInfoText.getLocalBounds();
        playerInfoText.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + localScreenPanel.getSize().x / 2.f - playerInfoLocalBounds.width / 2.f,
                                                localScreenPanel.getPosition().y + 20.f));
        window.draw(playerInfoText);

        if (!history.empty()) {
            sf::FloatRect graphDisplayArea(
                    localScreenPanel.getPosition().x + 60.0f,
                    localScreenPanel.getPosition().y + 80.0f,
                    localScreenPanel.getSize().x - 120.0f,
                    localScreenPanel.getSize().y * 0.5f - 80.0f
            );
            renderScoreGraph(std::deque<ScorePoint>(history.begin(), history.end()), graphDisplayArea, uiFont, "Score Over Time");

            sf::Text keyMetricsTitle("Key Metrics:", *uiFont, 24);
            keyMetricsTitle.setFillColor(sf::Color::Cyan);
            keyMetricsTitle.setPosition(sf::Vector2f(graphDisplayArea.left, graphDisplayArea.top + graphDisplayArea.height + 25.f));
            window.draw(keyMetricsTitle);

            float totalTime = 0.f;
            if (!history.empty()) {
                totalTime = history.back().timeStamp - history.front().timeStamp;
            }
            if (totalTime < 0.1f) totalTime = 0.1f;

            int peakScoreVal = 0;
            for(const auto& p : history) peakScoreVal = std::max(peakScoreVal, p.score);

            float scoreChangeVal = 0.f;
            if(!history.empty()){
                scoreChangeVal = static_cast<float>(history.back().score) - history.front().score;
            }
            float scorePerSecondVal = (totalTime > 0.001f) ? scoreChangeVal / totalTime : 0;

            std::vector<std::pair<std::string, std::string>> detailedStatsList = {
                    {"Duration of Run:", fmt::format("{:.1f}s", totalTime)},
                    {"Peak Score Reached:", fmt::format("{}", peakScoreVal)},
                    {"Net Score Change:", fmt::format("{}{}", scoreChangeVal >=0 ? "+" : "", static_cast<int>(scoreChangeVal) )},
                    {"Avg. Score/Sec:", fmt::format("{:.1f}", scorePerSecondVal)},
                    {"Data Points:", fmt::format("{}", history.size())}
            };

            float statDisplayY = keyMetricsTitle.getPosition().y + 40.f;
            float statDisplayXCol1 = graphDisplayArea.left + 20.f;
            float statDisplayXCol2 = graphDisplayArea.left + graphDisplayArea.width / 2.f + 30.f;
            float statLabelWidth = 180.f;

            for(size_t k=0; k < detailedStatsList.size(); ++k) {
                sf::Text statLabelText(detailedStatsList[k].first, *uiFont, 20);
                statLabelText.setFillColor(sf::Color(180,180,180));
                sf::Text statValueText(detailedStatsList[k].second, *uiFont, 20);
                statValueText.setFillColor(sf::Color::White);

                if (k < (detailedStatsList.size() + 1) / 2) {
                    statLabelText.setPosition(sf::Vector2f(statDisplayXCol1, statDisplayY + (k * 30.f)));
                    statValueText.setPosition(sf::Vector2f(statDisplayXCol1 + statLabelWidth, statDisplayY + (k * 30.f)));
                } else {
                    statLabelText.setPosition(sf::Vector2f(statDisplayXCol2, statDisplayY + ((k - (detailedStatsList.size()+1)/2) * 30.f)));
                    statValueText.setPosition(sf::Vector2f(statDisplayXCol2 + statLabelWidth, statDisplayY + ((k - (detailedStatsList.size()+1)/2) * 30.f)));
                }
                window.draw(statLabelText);
                window.draw(statValueText);
            }

        } else {
            sf::Text noHistoryData("No graph data available for this score.", *uiFont, 24);
            sf::FloatRect noHistoryDataLocalBounds = noHistoryData.getLocalBounds();
            noHistoryData.setPosition(sf::Vector2f(localScreenPanel.getPosition().x + localScreenPanel.getSize().x/2.f - noHistoryDataLocalBounds.width/2.f,
                                                   localScreenPanel.getPosition().y + localScreenPanel.getSize().y / 2.0f));
            window.draw(noHistoryData);
        }
        sf::FloatRect gvBackButtonGlobalBounds = graphViewBackButton.getGlobalBounds();
        graphViewBackButton.setPosition(sf::Vector2f(
                localScreenPanel.getPosition().x + localScreenPanel.getSize().x / 2.f - gvBackButtonGlobalBounds.width / 2.f,
                localScreenPanel.getPosition().y + localScreenPanel.getSize().y - gvBackButtonGlobalBounds.height - 15.f
        ));

    } else {
        sf::Text noScoreSelection("No score selected or data unavailable.", *uiFont, 24);
        sf::FloatRect noScoreSelectionLocalBounds = noScoreSelection.getLocalBounds();
        noScoreSelection.setPosition(sf::Vector2f(GameConstants::WINDOW_WIDTH / 2.f - noScoreSelectionLocalBounds.width/2.f,
                                                  GameConstants::WINDOW_HEIGHT / 2.f));
        window.draw(noScoreSelection);
        sf::FloatRect gvBackButtonGlobalBounds = graphViewBackButton.getGlobalBounds();
        graphViewBackButton.setPosition(sf::Vector2f(
                GameConstants::WINDOW_WIDTH / 2.f - gvBackButtonGlobalBounds.width / 2.f,
                GameConstants::WINDOW_HEIGHT - 80.f
        ));
    }
    graphViewBackButton.draw(window);
}