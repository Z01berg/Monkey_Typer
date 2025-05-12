#include "../include/Button.h"

Button::Button() : isHovered(false) {}

Button::Button(const std::string& textStr, const sf::Font& font, unsigned int fontSize, const sf::Vector2f& initialPosition)
        : text(textStr, font, fontSize), position(initialPosition), isHovered(false) {
    this->text.setFillColor(sf::Color::White);
    this->text.setPosition(this->position); // Use the member 'position'
    updateBounds();
}

void Button::updateBounds() { // bounds refers to the background rectangle, which includes padding.

    // The text's global bounds are used as a base.
    sf::FloatRect textGlobalBounds = text.getGlobalBounds();

    bounds.left = textGlobalBounds.left - 10;
    bounds.top = textGlobalBounds.top - 5;
    bounds.width = textGlobalBounds.width + 20;
    bounds.height = textGlobalBounds.height + 10;

    background.setSize(sf::Vector2f(bounds.width, bounds.height));
    background.setPosition(bounds.left, bounds.top); // Set background position
    background.setFillColor(sf::Color(60, 60, 70, 200));
    background.setOutlineColor(isHovered ? sf::Color::Yellow : sf::Color(120, 120, 140));
    background.setOutlineThickness(2);
}

void Button::setPosition(const sf::Vector2f& pos) { // This 'pos' becomes the top-left for the text.
    position = pos;
    text.setPosition(position);
    updateBounds();
}

void Button::setText(const std::string& newText) {
    text.setString(newText); // The text's origin (setPosition) might need adjustment if its local bounds change significantly,
    updateBounds();
}

void Button::update(const sf::Vector2f& mousePos) {
    bool wasHoveredPrior = isHovered;
    isHovered = bounds.contains(mousePos);

    if (wasHoveredPrior != isHovered) {
        background.setOutlineColor(isHovered ? sf::Color::Yellow : sf::Color(120, 120, 140));
    }
}

void Button::draw(sf::RenderWindow& window) const {
    window.draw(background);
    window.draw(text);
}

bool Button::contains(const sf::Vector2f& point) const {
    return bounds.contains(point);
}

sf::FloatRect Button::getGlobalBounds() const {
    return background.getGlobalBounds();
}

sf::Vector2f Button::getPosition() const {
    return background.getPosition();
}