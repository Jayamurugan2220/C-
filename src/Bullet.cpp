#include "Bullet.h"
#include <iostream>
#include <vector>
#include <string>

Bullet::Bullet(BulletType type, float x, float y, float dirX, float dirY) 
    : type(type), active(true), damage(10) {
    
    // Set bullet properties based on type
    switch (type) {
        case BulletType::PLAYER:
            speed = 500.0f;
            damage = 10;
            break;
        case BulletType::ENEMY:
            speed = 300.0f;
            damage = 15;
            break;
    }
    
    direction.x = dirX;
    direction.y = dirY;
    
    // Use simple colored rectangle for stability
    texture.create(6, 12);
    sf::Image image;
    image.create(6, 12, type == BulletType::PLAYER ? sf::Color::Cyan : sf::Color::Red);
    texture.update(image);
    sprite.setTexture(texture);
    sprite.setPosition(x, y);
    sprite.setOrigin(3.0f, 6.0f);
    
    std::cout << "Bullet initialized with simple colored rectangle" << std::endl;
}

void Bullet::update(float deltaTime) {
    // Move the bullet
    sprite.move(direction.x * speed * deltaTime, direction.y * speed * deltaTime);
}

bool Bullet::isActive() const {
    return active;
}

void Bullet::setActive(bool active) {
    this->active = active;
}

BulletType Bullet::getType() const {
    return type;
}

int Bullet::getDamage() const {
    return damage;
}

sf::Vector2f Bullet::getPosition() const {
    return sprite.getPosition();
}

void Bullet::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

void Bullet::render(sf::RenderWindow& window) {
    if (active) {
        window.draw(sprite);
    }
}

sf::FloatRect Bullet::getBounds() const {
    return sprite.getGlobalBounds();
}

bool Bullet::isOutOfBounds(float screenWidth, float screenHeight) const {
    sf::Vector2f pos = sprite.getPosition();
    return (pos.x < -50 || pos.x > screenWidth + 50 || 
            pos.y < -50 || pos.y > screenHeight + 50);
}

void Bullet::loadUserBulletTextures() {
    // Try to load custom bullet textures from multiple locations
    std::vector<std::string> bulletPaths = {
        "bullet.png",
        "assets/bullet.png",
        "d:/c++/bullet.png",
        "d:/c++/SpaceBattleGame/bullet.png"
    };
    
    // Try player bullet textures
    if (type == BulletType::PLAYER) {
        std::vector<std::string> playerBulletPaths = {
            "player_bullet.png",
            "assets/player_bullet.png",
            "d:/c++/player_bullet.png",
            "d:/c++/SpaceBattleGame/player_bullet.png"
        };
        
        for (const auto& path : playerBulletPaths) {
            if (texture.loadFromFile(path)) {
                std::cout << "Successfully loaded player bullet texture: " << path << std::endl;
                return;
            }
        }
    }
    
    // Try enemy bullet textures
    if (type == BulletType::ENEMY) {
        std::vector<std::string> enemyBulletPaths = {
            "enemy_bullet.png",
            "assets/enemy_bullet.png",
            "d:/c++/enemy_bullet.png",
            "d:/c++/SpaceBattleGame/enemy_bullet.png"
        };
        
        for (const auto& path : enemyBulletPaths) {
            if (texture.loadFromFile(path)) {
                std::cout << "Successfully loaded enemy bullet texture: " << path << std::endl;
                return;
            }
        }
    }
    
    // Try generic bullet paths
    for (const auto& path : bulletPaths) {
        if (texture.loadFromFile(path)) {
            std::cout << "Successfully loaded bullet texture: " << path << std::endl;
            return;
        }
    }
    
    // Create fallback texture if no custom texture found
    createFallbackBulletTexture();
}

void Bullet::createFallbackBulletTexture() {
    // Create a simple colored rectangle as fallback
    texture.create(6, 12);
    sf::Image image;
    
    switch (type) {
        case BulletType::PLAYER:
            image.create(6, 12, sf::Color::Cyan);
            std::cout << "Created cyan player bullet fallback texture" << std::endl;
            break;
        case BulletType::ENEMY:
            image.create(6, 12, sf::Color::Red);
            std::cout << "Created red enemy bullet fallback texture" << std::endl;
            break;
    }
    
    texture.update(image);
}

void Bullet::promptUserUpload() {
    std::cout << "=== BULLET TEXTURE UPLOAD GUIDE ===" << std::endl;
    std::cout << "To add custom bullet textures:" << std::endl;
    std::cout << "1. Player bullets: Place 'player_bullet.png' in game directory" << std::endl;
    std::cout << "2. Enemy bullets: Place 'enemy_bullet.png' in game directory" << std::endl;
    std::cout << "3. Universal bullets: Place 'bullet.png' in game directory" << std::endl;
    std::cout << "4. Supported locations: Game folder, d:/c++, d:/c++/SpaceBattleGame/" << std::endl;
    std::cout << "5. Recommended size: 6x12 pixels" << std::endl;
    std::cout << "6. Restart game after adding images" << std::endl;
    std::cout << "======================================" << std::endl;
}
