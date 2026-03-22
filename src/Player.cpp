#include "Player.h"
#include <iostream>

Player::Player() : speed(225.0f), health(100), maxHealth(100), fireRate(0.1f), fireTimer(0.0f) {
    // Use simple colored rectangle for stability
    texture.create(40, 40);
    sf::Image image;
    image.create(40, 40, sf::Color::Blue);
    texture.update(image);
    sprite.setTexture(texture);
    sprite.setPosition(400.0f, 500.0f);
    sprite.setOrigin(20.0f, 20.0f);
    
    std::cout << "Player initialized with 🚀" << std::endl;
}

void Player::move(float offsetX, float offsetY) {
    sprite.move(offsetX, offsetY);
}

void Player::update(float deltaTime) {
    // Update fire timer
    if (fireTimer > 0.0f) {
        fireTimer -= deltaTime;
    }
}

bool Player::canShoot() {
    return fireTimer <= 0.0f;
}

void Player::resetFireTimer() {
    fireTimer = fireRate;
}

void Player::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}

void Player::heal(int amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

int Player::getHealth() const {
    return health;
}

int Player::getMaxHealth() const {
    return maxHealth;
}

void Player::setMaxHealth(int maxHealth) {
    this->maxHealth = maxHealth;
    // Ensure current health doesn't exceed new max
    if (health > this->maxHealth) {
        health = this->maxHealth;
    }
}

bool Player::isAlive() const {
    return health > 0;
}

void Player::setFireRate(float rate) {
    fireRate = rate;
}

float Player::getFireRate() const {
    return fireRate;
}

sf::Vector2f Player::getPosition() const {
    return sprite.getPosition();
}

void Player::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

void Player::render(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect Player::getBounds() const {
    return sprite.getGlobalBounds();
}

sf::Sprite Player::getSprite() const {
    return sprite;
}

void Player::setTexture(const std::string& texturePath) {
    loadTexture(texturePath);
}

bool Player::loadTexture(const std::string& texturePath) {
    if (texture.loadFromFile(texturePath)) {
        sprite.setTexture(texture);
        
        // Center the origin for consistent positioning
        sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
        
        std::cout << "Successfully loaded player texture: " << texturePath << std::endl;
        std::cout << "Texture size: " << texture.getSize().x << "x" << texture.getSize().y << std::endl;
        std::cout << "Origin set to: " << texture.getSize().x / 2.0f << "," << texture.getSize().y / 2.0f << std::endl;
        return true;
    } else {
        std::cout << "Failed to load player texture: " << texturePath << std::endl;
        // Create fallback texture
        createFallbackTexture();
        return false;
    }
}

void Player::createFallbackTexture() {
    // Create a simple colored rectangle as fallback
    texture.create(40, 40);
    sf::Image image;
    image.create(40, 40, sf::Color::Blue);
    texture.update(image);
    sprite.setTexture(texture);
    sprite.setOrigin(20.0f, 20.0f);
}

void Player::promptUserUpload() {
    std::cout << "To upload custom player texture:" << std::endl;
    std::cout << "1. Place your image file in the game directory" << std::endl;
    std::cout << "2. Name it 'player.png'" << std::endl;
    std::cout << "3. Restart the game" << std::endl;
}
