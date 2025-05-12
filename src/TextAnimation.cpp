#include "../include/TextAnimation.h"
#include "../include/Constants.h"

void TextAnimation::update(float deltaTime) {
    timer += deltaTime;
    if (timer >= GameConstants::TYPE_ANIMATION_SPEED && currentChar < static_cast<int>(text.length())) {
        currentChar++;
        timer = 0;
    }

    if (currentChar >= static_cast<int>(text.length()) && timer >= GameConstants::ANIMATION_DURATION) {
        complete = true;
        currentChar = 0;
        timer = 0;
    }
}

std::string TextAnimation::getCurrentText() const {
    return text.substr(0, currentChar) + (currentChar < static_cast<int>(text.length()) ? "_" : "");
}

bool TextAnimation::isTypingPhase() const {
    return currentChar < static_cast<int>(text.length());
}

bool TextAnimation::isDisplayPhase() const {
    return currentChar == static_cast<int>(text.length()) && !complete;
}

void TextAnimation::reset() {
    timer = 0;
    currentChar = 0;
    complete = false;
}