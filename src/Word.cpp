#include "../include/Word.h"
#include "../include/Constants.h"
#include "../include/LinkedWord.h"

Word::Word(const std::string& text, const sf::Font& font, float speed, const sf::Vector2f& position,
           unsigned int fontSize)
        : text(text), originalText(text), speed(speed), position(position),
          typedCorrectly(false), mistakesCount(0) {
    sfText.setFont(font);
    sfText.setCharacterSize(fontSize);
    sfText.setFillColor(GameConstants::DEFAULT_TEXT_COLOR);
    sfText.setString(text);
    sfText.setPosition(position);
}

bool Word::isLinked() const { return false; }

const std::vector<sf::Vector2f>& Word::getLinkPoints() const {
    static const std::vector<sf::Vector2f> empty;
    return empty;
}

void Word::update(float deltaTime) {
    position.x += speed * deltaTime;
    sfText.setPosition(position);
    updateColorBasedOnPosition();
}

void Word::draw(sf::RenderWindow& window) const {
    window.draw(sfText);
}

bool Word::isOutOfBoundsRight() const {
    // Word is considered out of bounds only when its LEFT edge passes the right screen edge
    return position.x > GameConstants::WINDOW_WIDTH;
}

bool Word::isOutOfBounds() const {
    // For general cleanup - word is completely off either side
    return (position.x > GameConstants::WINDOW_WIDTH) ||
           (position.x + sfText.getLocalBounds().width < 0);
}

void Word::processInput(sf::Uint32 unicode, bool highlightTyping) {
    if (isLinked()) {
        // Delegate to LinkedWord's implementation
        static_cast<LinkedWord*>(this)->processInput(unicode, highlightTyping);
        return;
    }

    if (typedCorrectly) return;

    if (unicode == 8) { // Backspace
        if (!currentInput.empty()) {
            currentInput.pop_back();
        }
    } else if (unicode == 27) { // Esc
        resetTyping();
    }else if (unicode >= 32 && unicode <= 126) { // Printable ASCII
        if (currentInput.length() < text.length()) {
            currentInput += static_cast<char>(unicode);
        }
    }

    bool matches = true;
    for (size_t i = 0; i < currentInput.length(); ++i) {
        if (i >= text.length() || currentInput[i] != text[i]) {
            matches = false;
            break;
        }
    }

    if (matches) {
        if (currentInput.length() == text.length()) {
            typedCorrectly = true;
        }
    } else {
        mistakesCount++;
    }

    updateTextColor(highlightTyping);
}

void Word::updateTextColor(bool highlightTyping) {
    if (typedCorrectly) {
        sfText.setFillColor(GameConstants::COMPLETED_COLOR);
    } else if (isBeingTyped() && highlightTyping) {
        sfText.setFillColor(GameConstants::HIGHLIGHT_COLOR);
    } else {
        updateColorBasedOnPosition();
    }
    sfText.setString(text);
}

const std::string& Word::getText() const { return text; }

bool Word::isBeingTyped() const {
    return !typedCorrectly && currentInput.length() > 0;
}

bool Word::isComplete() const {
    return typedCorrectly;
}

const std::string& Word::getCurrentInput() const { return currentInput; }

const sf::Vector2f& Word::getPosition() const { return position; }

float Word::getSpeed() const { return speed; }

int Word::getMistakesCount() const { return mistakesCount; }

void Word::setText(const std::string& newText) {
    text = newText;
    sfText.setString(newText);
}

const sf::Text& Word::getSfText() const { return sfText; }

sf::Text& Word::getMutableSfText() { return sfText; }

bool Word::getTypedCorrectly() const { return typedCorrectly; }

void Word::setTypedCorrectly(bool value) { typedCorrectly = value; }

void Word::setSpeed(float value) { speed = value; }

void Word::setPosition(const sf::Vector2f& value) {
    position = value;
    sfText.setPosition(position);
}

size_t Word::getCurrentWordIndex() const { return currentWordIndex; }

void Word::setCurrentWordIndex(size_t value) { currentWordIndex = value; }

void Word::incrementMistakesCount() { mistakesCount++; }

void Word::clearCurrentInput() { currentInput.clear(); }

void Word::appendToCurrentInput(char c) { currentInput += c; }

void Word::popBackCurrentInput() { if (!currentInput.empty()) currentInput.pop_back(); }

void Word::updateColorBasedOnPosition() {
    if (speed > 0) {
        float distanceFromEdge = GameConstants::WINDOW_WIDTH - (position.x);
        float redFactor = std::max(0.0f, 1.0f - (distanceFromEdge / (GameConstants::WINDOW_WIDTH * 0.6f)));

        if (!typedCorrectly && !isBeingTyped()) {
            sf::Color wordColor(
                    255,
                    static_cast<sf::Uint8>(255 * (1.0f - redFactor)),
                    static_cast<sf::Uint8>(255 * (1.0f - redFactor))
            );
            sfText.setFillColor(wordColor);
        }
    } else if (speed < 0) {
        float distanceFromEdge = position.x;
        float redFactor = std::max(0.0f, 1.0f - (distanceFromEdge / (GameConstants::WINDOW_WIDTH * 0.3f)));

        if (!typedCorrectly && !isBeingTyped()) {
            sf::Color wordColor(
                    255,
                    static_cast<sf::Uint8>(255 * (1.0f - redFactor)),
                    static_cast<sf::Uint8>(255 * (1.0f - redFactor))
            );
            sfText.setFillColor(wordColor);
        }
    }
}