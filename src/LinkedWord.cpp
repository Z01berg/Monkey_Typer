#include "../include/LinkedWord.h"
#include "../include/Constants.h"

LinkedWord::LinkedWord(const std::vector<std::string>& words,
                       const sf::Font& font, float speed,
                       const sf::Vector2f& position, unsigned int fontSize,
                       const std::vector<float>& yPositions)
        : Word(words[0], font, speed, position, fontSize), words(words), yPositions(yPositions)
{
    wordTexts.clear();
    for (const auto& word : words) {
        sf::Text text(word, font, fontSize);
        text.setFillColor(sf::Color(150, 150, 150));    // Gray for blocked words
        text.setOutlineColor(sf::Color::Black);
        text.setOutlineThickness(1);
        wordTexts.push_back(text);
    }

    // Only the first word is active initially
    wordTexts[0].setFillColor(sf::Color(100, 150, 255)); // Blue for active word
    updatePositions();
}

bool LinkedWord::isLinked() const { return true; }

const std::vector<sf::Vector2f>& LinkedWord::getLinkPoints() const {
    return linkPoints;
}

void LinkedWord::update(float deltaTime) {
    setPosition(getPosition() + sf::Vector2f(getSpeed() * deltaTime, 0));
    updatePositions();
    updateColorBasedOnPosition();
}

void LinkedWord::processInput(sf::Uint32 unicode, bool highlightTyping) {
    if (getTypedCorrectly()) return;

    // Only process input for the current active word
    if (unicode == 8) { // Backspace
        if (!getCurrentInput().empty()) {
            popBackCurrentInput();
        }
        return;
    }

    // Allow typing any character, but only count correct ones
    if (getCurrentInput().length() < words[currentPartIndex].length()) {
        appendToCurrentInput(static_cast<char>(unicode));
        if (tolower(unicode) != tolower(words[currentPartIndex][getCurrentInput().length() - 1])) {
            incrementMistakesCount();
        }
    }

    // Check if current word is completed
    if (getCurrentInput() == words[currentPartIndex]) {
        if (currentPartIndex + 1 < static_cast<int>(words.size())) {
            // Move to next word in sequence
            currentPartIndex++;
            setCurrentPart(currentPartIndex);
            // Push all words back slightly
            setPosition(getPosition() + sf::Vector2f(-20.0f, 0)); // <<< THIS LINE
        } else {
            // All parts completed
            setTypedCorrectly(true);
        }
    }
}

void LinkedWord::setCurrentPart(int index) {
    currentPartIndex = index;
    setText(words[currentPartIndex]);
    clearCurrentInput();
    updatePositions();
    
    // Update colors for all words
    for (size_t i = 0; i < wordTexts.size(); ++i) {
        if (i < static_cast<size_t>(currentPartIndex)) {
            // Completed words - green
            wordTexts[i].setFillColor(sf::Color(100, 255, 100));
        } else if (i == static_cast<size_t>(currentPartIndex)) {
            // Current active word - blue
            wordTexts[i].setFillColor(sf::Color(100, 150, 255));
        } else {
            // Future words - gray
            wordTexts[i].setFillColor(sf::Color(150, 150, 150));
        }
    }
}

bool LinkedWord::isOutOfBoundsRight() const {
    if (wordTexts.empty()) return false;
    return wordTexts.back().getPosition().x > GameConstants::WINDOW_WIDTH;
}

bool LinkedWord::isOutOfBounds() const {
    return wordTexts.back().getPosition().x > GameConstants::WINDOW_WIDTH;
}

void LinkedWord::updateTextColor(bool highlightTyping) {
    // NO MORE HERE in setCurrentPart
}

void LinkedWord::updateColorBasedOnPosition() {
    for (size_t i = 0; i < wordTexts.size(); ++i) {
        if (i < static_cast<size_t>(currentPartIndex)) {
            // Completed words - make them invisible
            wordTexts[i].setFillColor(sf::Color::Transparent);
        } else if (i == static_cast<size_t>(currentPartIndex)) {
            // Current active word
            if (wordTexts[i].getPosition().x > GameConstants::WINDOW_WIDTH * 0.7f) {
                // Warning color when near right edge
                wordTexts[i].setFillColor(sf::Color(255, 100, 100)); // Red
            } else {
                wordTexts[i].setFillColor(sf::Color(100, 150, 255)); // Blue
            }
        } else {
            // Future words
            if (wordTexts[i].getPosition().x > GameConstants::WINDOW_WIDTH * 0.7f) {
                // Warning color when near right edge
                wordTexts[i].setFillColor(sf::Color(200, 100, 100, 150)); // Semi-transparent red
            } else {
                wordTexts[i].setFillColor(sf::Color(150, 150, 150)); // Gray
            }
        }
    }
}

void LinkedWord::draw(sf::RenderWindow& window) const {
    // Draw connecting lines
    if (linkPoints.size() > 1) {
        sf::VertexArray lines(sf::PrimitiveType::LinesStrip, linkPoints.size());
        for (size_t i = 0; i < linkPoints.size(); ++i) {
            lines[i].position = linkPoints[i];
            if (i <= static_cast<size_t>(currentPartIndex)) {
                // Only draw lines up to the current word
                lines[i].color = (i < static_cast<size_t>(currentPartIndex)) ?
                                sf::Color::Transparent : // Invisible for completed words
                                sf::Color(100, 150, 255); // Blue for current word
            } else {
                lines[i].color = sf::Color(150, 150, 150); // Gray for future words
            }
        }
        window.draw(lines);
    }

    // Draw all word parts with outline
    for (const auto& text : wordTexts) {
        if (text.getFillColor() != sf::Color::Transparent) {
            // Draw black outline first (thicker)
            sf::Text outline = text;
            outline.setOutlineColor(sf::Color::Black);
            outline.setOutlineThickness(3);
            window.draw(outline);

            // Then draw the main text
            window.draw(text);
        }
    }
}

const std::string& LinkedWord::getText() const {
    return words[currentPartIndex];
}

void LinkedWord::updatePositions() {
    linkPoints.clear();
    if (wordTexts.empty()) return;

    float x = getPosition().x;
    float baseY = getPosition().y;

    for (size_t i = 0; i < wordTexts.size(); ++i) {
        // Set position for each word part, using custom Y position if available
        float y = yPositions.size() > i ? yPositions[i] : baseY;
        wordTexts[i].setPosition(x, y);

        // Calculate center point for connections
        sf::FloatRect bounds = wordTexts[i].getLocalBounds();
        linkPoints.emplace_back(
                x + bounds.width / 2,
                y + bounds.height / 2
        );

        x += bounds.width + 30.0f;
    }
}