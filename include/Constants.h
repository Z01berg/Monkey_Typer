#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <string>

namespace GameConstants {
    // Window settings
    constexpr unsigned int WINDOW_WIDTH = 1700;
    constexpr unsigned int WINDOW_HEIGHT = 900;

    // Game mechanics
    constexpr float BASE_SPEED = 100.0f;
    constexpr float WORD_SPAWN_INTERVAL = 2.0f;
    constexpr int MISTAKE_PENALTY = 5;
    constexpr int MAX_HISTORY_POINTS = 1000;

    // Animation timings
    constexpr float ANIMATION_DURATION = 2.0f;
    constexpr float TYPE_ANIMATION_SPEED = 0.1f;

    // Text settings
    constexpr unsigned int DEFAULT_FONT_SIZE = 24;

    // Colors
    const sf::Color DEFAULT_TEXT_COLOR = sf::Color::White;
    const sf::Color HIGHLIGHT_COLOR = sf::Color::Yellow;
    const sf::Color COMPLETED_COLOR = sf::Color::Green;

    // Input display styles
    enum class InputDisplayStyle {
        UNDER_CURSOR,
        BELOW_WORD,
        CENTERED
    };

    const std::array<std::string, 3> INPUT_DISPLAY_NAMES = {
            "Under Cursor",
            "Below Word",
            "Centered"
    };

    // Game statistics
    struct GameStats {
        float wordsPerMinute = 0.0f;
        float accuracy = 100.0f;
        int wordsTyped = 0;
        int charactersTyped = 0;
        int mistakesMade = 0;
    };
}