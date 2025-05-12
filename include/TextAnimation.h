#pragma once
#include <string>

struct TextAnimation {
    std::string text;
    float timer = 0.0f;
    int currentChar = 0;
    bool complete = false;

    void update(float deltaTime);
    std::string getCurrentText() const;
    bool isTypingPhase() const;
    bool isDisplayPhase() const;
    void reset();
};