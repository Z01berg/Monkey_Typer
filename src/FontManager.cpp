#include "../include/FontManager.h"
#include <fmt/core.h>
#include <filesystem>
#include <random>
#include <algorithm>

namespace fs = std::filesystem;

FontManager::FontManager() : defaultFont(nullptr) {}

void FontManager::loadFonts(const std::string& defaultFontPath, const std::string& fallbackDefaultFontPath, const std::string& fontsDirectory) {
    defaultFont = std::make_shared<sf::Font>();
    if (!defaultFont->loadFromFile(defaultFontPath)) {
        if (!defaultFont->loadFromFile(fallbackDefaultFontPath)) {
            fmt::print(stderr, "Failed to load default font from {} or {}\n", defaultFontPath, fallbackDefaultFontPath);
            exit(1); // Critical error if default font cannot be loaded
        }
    }
    availableFonts[fs::path(defaultFontPath).filename().string()] = defaultFont;

    try {
        if (fs::exists(fontsDirectory) && fs::is_directory(fontsDirectory)) {
            for (const auto& entry : fs::directory_iterator(fontsDirectory)) {
                if (entry.is_regular_file()) {
                    std::string ext = entry.path().extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                    if (ext == ".ttf" || ext == ".otf") {
                        auto font = std::make_shared<sf::Font>();
                        if (font->loadFromFile(entry.path().string())) {
                            availableFonts[entry.path().filename().string()] = font;
                            fmt::print("Loaded font: {}\n", entry.path().filename().string());
                        }
                    }
                }
            }
        } else {
            fs::create_directory(fontsDirectory);
            fmt::print("Created '{}' directory for custom fonts.\n", fontsDirectory);
        }
    } catch (const std::exception& e) {
        fmt::print(stderr, "Error scanning fonts directory '{}': {}\n", fontsDirectory, e.what());
    }
}

std::shared_ptr<sf::Font> FontManager::getDefaultFont() const {
    return defaultFont;
}

std::shared_ptr<sf::Font> FontManager::getFont(const std::string& name) const {
    auto it = availableFonts.find(name);
    if (it != availableFonts.end()) {
        return it->second;
    }
    return defaultFont;
}

const std::unordered_map<std::string, std::shared_ptr<sf::Font>>& FontManager::getAvailableFonts() const {
    return availableFonts;
}

std::shared_ptr<sf::Font> FontManager::getRandomFont() const {
    if (availableFonts.empty()) return defaultFont; // Should not happen if loadFonts is called

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, availableFonts.size() - 1);

    auto it = availableFonts.begin();
    std::advance(it, dist(gen));
    return it->second;
}