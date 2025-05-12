#pragma once
#include "Word.h"
#include <vector>

class LinkedWord : public Word {
public:
    LinkedWord(const std::vector<std::string>& words, const sf::Font& font, float speed,
               const sf::Vector2f& position, unsigned int fontSize = GameConstants::DEFAULT_FONT_SIZE,
               const std::vector<float>& yPositions = {});

    bool isLinked() const override;
    const std::vector<sf::Vector2f>& getLinkPoints() const override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    void processInput(sf::Uint32 unicode, bool highlightTyping) override;
    bool isOutOfBoundsRight() const override;
    bool isOutOfBounds() const override;
    void updateTextColor(bool highlightTyping) override;
    const std::string& getText() const override;

    const std::vector<sf::Text>& getWordTexts() const { return wordTexts; }
    void setCurrentPart(int index);
    int getCurrentPart() const { return currentPartIndex; }

private:
    void updatePositions();
    int currentPartIndex = 0;

    std::vector<std::string> words;
    std::vector<sf::Text> wordTexts;
    std::vector<sf::Vector2f> linkPoints;
    std::vector<float> yPositions;

    void updateColorBasedOnPosition();
};