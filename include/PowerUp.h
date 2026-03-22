#ifndef POWERUP_H
#define POWERUP_H

#include <SFML/Graphics.hpp>
#include <random>

enum class PowerUpType {
    RAPID_FIRE,    // ✨ - Rate of fire increase
    HEALTH,        // 🧚‍♀️ - 20% health increase
    COUNT  // For random selection
};

class PowerUp {
private:
    sf::Sprite sprite;
    sf::Texture texture;
    PowerUpType type;
    sf::Vector2f position;
    float speed;
    bool active;
    float lifetime;
    
    std::mt19937 rng;
    
    void createFallbackTexture();
    void loadUserTextures();

public:
    PowerUp(float x = 0.0f, float y = 0.0f);
    
    // Movement methods
    void update(float deltaTime);
    
    // Status methods
    bool isActive() const;
    void setActive(bool active);
    bool isExpired() const;
    
    // Position methods
    sf::Vector2f getPosition() const;
    void setPosition(float x, float y);
    
    // Type methods
    PowerUpType getType() const;
    std::string getTypeName() const;
    std::string getDescription() const;
    
    // Rendering
    void render(sf::RenderWindow& window);
    
    // Bounds for collision detection
    sf::FloatRect getBounds() const;
    
    // Check if power-up is out of screen bounds
    bool isOutOfBounds(float screenWidth, float screenHeight) const;
    
    // Apply power-up effect (returns duration in seconds)
    float applyEffect();
    
    // Get sprite for collision detection
    sf::Sprite getSprite() const;
};

#endif // POWERUP_H
