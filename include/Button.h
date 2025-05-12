// --- START OF FILE Button.h ---
#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Button {
public:
    Button();
    Button(const std::string& text, const sf::Font& font, unsigned int fontSize, const sf::Vector2f& position);

    void setPosition(const sf::Vector2f& pos);
    void setText(const std::string& newText);
    void update(const sf::Vector2f& mousePos); // For hover effects
    void draw(sf::RenderWindow& window) const;
    bool contains(const sf::Vector2f& point) const;

    sf::Text text; // Keep public for direct access by MonkeyTyperGame if still needed, or make private too
    // sf::RectangleShape background; // Keep private

    // Add Getters
    sf::FloatRect getGlobalBounds() const; // Returns the background's global bounds
    sf::Vector2f getPosition() const;     // Returns the top-left position of the background

private:
    void updateBounds();

    sf::Vector2f position; // Top-left of the text content
    sf::RectangleShape background;
    sf::FloatRect bounds; // Global bounds of the background
    bool isHovered;
};
// --- END OF FILE Button.h ---