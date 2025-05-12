#pragma once
#include "SFML/Graphics.hpp"
#include <string>
#include "../include/Constants.h"

class Word {
public:
    Word(const std::string& text, const sf::Font& font, float speed, const sf::Vector2f& position,
         unsigned int fontSize = GameConstants::DEFAULT_FONT_SIZE);

    virtual ~Word() = default;

    bool isBeingActivelyTyped() const { return isActiveTypingTarget; }
    void setAsActiveTypingTarget(bool active) { isActiveTypingTarget = active; }
    virtual bool isLinked() const;
    virtual const std::vector<sf::Vector2f>& getLinkPoints() const;
    virtual void update(float deltaTime);
    virtual void draw(sf::RenderWindow& window) const;
    virtual bool isOutOfBoundsRight() const;
    virtual bool isOutOfBounds() const;
    virtual void processInput(sf::Uint32 unicode, bool highlightTyping);
    virtual void updateTextColor(bool highlightTyping);
    virtual const std::string& getText() const;

    bool isBeingTyped() const;
    bool isComplete() const;
    const std::string& getCurrentInput() const;
    const sf::Vector2f& getPosition() const;
    float getSpeed() const;
    int getMistakesCount() const;

    virtual void setText(const std::string& newText);
    const sf::Text& getSfText() const;
    sf::Text& getMutableSfText();
    bool getTypedCorrectly() const;
    void setTypedCorrectly(bool value);
    void setSpeed(float value);
    void setPosition(const sf::Vector2f& value);
    size_t getCurrentWordIndex() const;
    void setCurrentWordIndex(size_t value);
    void incrementMistakesCount();
    void clearCurrentInput();
    void appendToCurrentInput(char c);
    void popBackCurrentInput();

protected:
    void updateColorBasedOnPosition();



protected:
    bool isActiveTypingTarget = false;
    std::string text;
    std::string originalText;
    std::string currentInput;
    float speed;
    sf::Text sfText;
    sf::Vector2f position;
    bool typedCorrectly;
    int mistakesCount;
    size_t currentWordIndex = 0;

public:
    void resetTyping() {
        currentInput.clear();
        setAsActiveTypingTarget(false);
    }
};