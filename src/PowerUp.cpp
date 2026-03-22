#include "PowerUp.h"
#include <iostream>
#include <string>
#include <algorithm>

PowerUp::PowerUp(float x, float y) : position(x, y), speed(50.0f), active(true), lifetime(5.0f) {
    // Random power-up type
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, static_cast<int>(PowerUpType::COUNT) - 1);
    type = static_cast<PowerUpType>(dist(rd));
    
    // Create simple colored rectangle based on type
    createFallbackTexture();
    
    sprite.setTexture(texture);
    sprite.setPosition(position);
    sprite.setOrigin(15.0f, 15.0f); // Center of 30x30 power-up
    
    std::cout << "PowerUp created: " << getTypeName() << " at position (" << x << ", " << y << ")" << std::endl;
}

void PowerUp::createFallbackTexture() {
    // Create 30x30 texture for power-up
    texture.create(30, 30);
    sf::Image image;
    
    // Color based on power-up type
    sf::Color color;
    switch (type) {
        case PowerUpType::RAPID_FIRE:
            color = sf::Color::Yellow;  // ✨ - Yellow for rapid fire
            break;
        case PowerUpType::HEALTH:
            color = sf::Color::Green;    // 🧚‍♀️ - Green for health
            break;
        default:
            color = sf::Color::White;
            break;
    }
    
    image.create(30, 30, color);
    texture.update(image);
}

void PowerUp::update(float deltaTime) {
    if (!active) return;
    
    // Move power-up downward slowly
    position.y += speed * deltaTime;
    sprite.setPosition(position);
    
    // Decrease lifetime
    lifetime -= deltaTime;
    
    // Deactivate if expired
    if (lifetime <= 0) {
        active = false;
    }
}

bool PowerUp::isActive() const {
    return active;
}

void PowerUp::setActive(bool active) {
    this->active = active;
}

bool PowerUp::isExpired() const {
    return lifetime <= 0;
}

sf::Vector2f PowerUp::getPosition() const {
    return position;
}

void PowerUp::setPosition(float x, float y) {
    position.x = x;
    position.y = y;
    sprite.setPosition(position);
}

PowerUpType PowerUp::getType() const {
    return type;
}

std::string PowerUp::getTypeName() const {
    switch (type) {
        case PowerUpType::RAPID_FIRE:
            return "✨ Rapid Fire";
        case PowerUpType::HEALTH:
            return "🧚‍♀️ Health Boost";
        default:
            return "Unknown";
    }
}

std::string PowerUp::getDescription() const {
    switch (type) {
        case PowerUpType::RAPID_FIRE:
            return "Increases fire rate for 5 seconds";
        case PowerUpType::HEALTH:
            return "Restores 20% of max health";
        default:
            return "Unknown power-up";
    }
}

void PowerUp::render(sf::RenderWindow& window) {
    if (active) {
        window.draw(sprite);
    }
}

sf::FloatRect PowerUp::getBounds() const {
    return sprite.getGlobalBounds();
}

bool PowerUp::isOutOfBounds(float screenWidth, float screenHeight) const {
    return position.y > screenHeight + 50.0f;
}

float PowerUp::applyEffect() {
    switch (type) {
        case PowerUpType::RAPID_FIRE:
            return 5.0f; // 5 seconds duration
        case PowerUpType::HEALTH:
            return 0.0f; // Instant effect
        default:
            return 0.0f;
    }
}

sf::Sprite PowerUp::getSprite() const {
    return sprite;
}
