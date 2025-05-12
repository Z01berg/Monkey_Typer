#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <memory>

class FontManager {
public:
    FontManager();

    // Loads the default font and scans a directory for other fonts.
    void loadFonts(const std::string& defaultFontPath, const std::string& fallbackDefaultFontPath, const std::string& fontsDirectory);

    // Returns a shared pointer to the default font.
    std::shared_ptr<sf::Font> getDefaultFont() const;

    // Returns a shared pointer to a font by its name.
    std::shared_ptr<sf::Font> getFont(const std::string& name) const;

    // Returns a map of all available fonts.
    const std::unordered_map<std::string, std::shared_ptr<sf::Font>>& getAvailableFonts() const;

    // Returns a random font from the available fonts.
    std::shared_ptr<sf::Font> getRandomFont() const;

private:
    std::shared_ptr<sf::Font> defaultFont;
    std::unordered_map<std::string, std::shared_ptr<sf::Font>> availableFonts;
};