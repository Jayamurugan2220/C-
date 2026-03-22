#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

Game::Game() : window(sf::VideoMode(800, 600), "Space Battle Game"), 
               currentState(GameState::START), currentLevel(Level::LEVEL_1),
               score(0), enemySpawnTimer(0.0f), difficultyMultiplier(1.0f) {
    
    window.setFramerateLimit(60);
    
    // Initialize level configurations
    initializeLevels();
    
    // Setup first level
    setupLevel(Level::LEVEL_1);
    
    setupUI();
    checkForCustomImages();
    
    // Initialize random seed
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // Initialize player
    player.setPosition(400.0f, 500.0f);
    
    // Initialize power-up system
    powerUpSpawnChance = 0.3f; // 30% chance to spawn power-up on enemy death
    rapidFireTimer = 0.0f;
    hasRapidFire = false;
    
    window.setFramerateLimit(60); // Limit to 60 FPS
}

void Game::run() {
    sf::Clock clock;
    
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        
        processEvents();
        
        if (currentState == GameState::PLAYING) {
            update(deltaTime);
        }
        
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                else if (currentState == GameState::START) {
                    if (event.key.code == sf::Keyboard::Space) {
                        // Reset everything for new game
                        score = 0;
                        currentLevel = Level::LEVEL_1;
                        enemySpawnTimer = 0.0f;
                        
                        // Clear all enemies and bullets
                        enemies.clear();
                        bullets.clear();
                        
                        // Reset player
                        player.setPosition(400.0f, 500.0f);
                        player.heal(player.getMaxHealth()); // Full health restore
                        
                        // Setup first level
                        setupLevel(Level::LEVEL_1);
                        currentState = GameState::PLAYING;
                        
                        std::cout << "New game started! Score: " << score << ", Level: 1" << std::endl;
                    }
                }
                else if (currentState == GameState::GAME_OVER) {
                    if (event.key.code == sf::Keyboard::Space) {
                        // Reset everything for new game
                        score = 0;
                        currentLevel = Level::LEVEL_1;
                        enemySpawnTimer = 0.0f;
                        
                        // Clear all enemies and bullets
                        enemies.clear();
                        bullets.clear();
                        
                        // Reset player
                        player.setPosition(400.0f, 500.0f);
                        player.heal(player.getMaxHealth()); // Full health restore
                        
                        // Setup first level
                        setupLevel(Level::LEVEL_1);
                        currentState = GameState::PLAYING;
                        
                        std::cout << "Game restarted! Score: " << score << ", Level: 1" << std::endl;
                    }
                }
                else if (currentState == GameState::LEVEL_COMPLETE) {
                    if (event.key.code == sf::Keyboard::Space) {
                        nextLevel();
                        currentState = GameState::PLAYING;
                    }
                }
                break;
        }
    }
    
    // Handle continuous input during gameplay
    if (currentState == GameState::PLAYING) {
        float moveSpeed = 225.0f; // Player movement speed (increased by 50% to 225)
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player.move(-moveSpeed * 0.016f, 0); // Assuming 60 FPS
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player.move(moveSpeed * 0.016f, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            player.move(0, -moveSpeed * 0.016f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            player.move(0, moveSpeed * 0.016f);
        }
        
        // Shoot with Left CTRL key (primary shooting control)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            if (player.canShoot()) {
                sf::FloatRect playerBounds = player.getBounds();
                float bulletX = playerBounds.left + (playerBounds.width / 2.0f);
                float bulletY = playerBounds.top;
                bullets.emplace_back(BulletType::PLAYER, bulletX, bulletY, 0.0f, -1.0f);
                player.resetFireTimer();
                std::cout << "Player shot with Left CTRL! Bullets: " << bullets.size() << std::endl;
            }
        }
        
        // Alternative shoot with Right CTRL key
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
            if (player.canShoot()) {
                sf::FloatRect playerBounds = player.getBounds();
                float bulletX = playerBounds.left + (playerBounds.width / 2.0f);
                float bulletY = playerBounds.top;
                bullets.emplace_back(BulletType::PLAYER, bulletX, bulletY, 0.0f, -1.0f);
                player.resetFireTimer();
                std::cout << "Player shot with Right CTRL! Bullets: " << bullets.size() << std::endl;
            }
        }
        
        // Backup shooting with Space key (for testing)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && currentState == GameState::PLAYING) {
            if (player.canShoot()) {
                sf::FloatRect playerBounds = player.getBounds();
                float bulletX = playerBounds.left + (playerBounds.width / 2.0f);
                float bulletY = playerBounds.top;
                bullets.emplace_back(BulletType::PLAYER, bulletX, bulletY, 0.0f, -1.0f);
                player.resetFireTimer();
                std::cout << "Player shot with Space! Bullets: " << bullets.size() << std::endl;
            }
        }
    }
}

void Game::update(float deltaTime) {
    // Update player
    player.update(deltaTime);
    
    // Update power-ups
    updatePowerUps(deltaTime);
    
    // Update enemies
    for (auto& enemy : enemies) {
        enemy.update(deltaTime);
        
        // Check if enemy should shoot
        if (enemy.shouldShoot()) {
            // Create enemy bullet with correct positioning
            sf::FloatRect enemyBounds = enemy.getBounds();
            float bulletX = enemyBounds.left + (enemyBounds.width / 2.0f); // Center of enemy
            float bulletY = enemyBounds.top + enemyBounds.height; // Bottom of enemy
            bullets.emplace_back(BulletType::ENEMY, bulletX, bulletY, 0.0f, 1.0f);
            enemy.resetFireTimer();
            std::cout << "Enemy shot! Total bullets: " << bullets.size() << std::endl;
            std::cout << "Enemy bounds: X=" << enemyBounds.left << ", Y=" << enemyBounds.top 
                     << ", W=" << enemyBounds.width << ", H=" << enemyBounds.height << std::endl;
            std::cout << "Enemy bullet spawn: X=" << bulletX << ", Y=" << bulletY << std::endl;
        }
    }
    
    // Update bullets
    for (auto& bullet : bullets) {
        bullet.update(deltaTime);
    }
    
    // Spawn enemies based on level configuration
    enemySpawnTimer += deltaTime;
    const auto& config = levelConfigs[currentLevel];
    if (enemySpawnTimer >= config.enemySpawnRate) {
        spawnEnemy();
        enemySpawnTimer = 0.0f;
    }
    
    // Handle collisions
    handleCollisions();
    
    // Remove dead enemies and inactive bullets
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e) { return !e.isAlive(); }), enemies.end());
    
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [this](const Bullet& b) { return !b.isActive() || b.isOutOfBounds(800, 600); }), bullets.end());
    
    // Check game over condition
    if (!player.isAlive()) {
        currentState = GameState::GAME_OVER;
    }
    
    // Check level completion
    checkLevelComplete();
    
    // Update UI
    updateUI();
}

void Game::render() {
    window.clear(sf::Color::Black);
    
    if (currentState == GameState::START) {
        gameStateText.setString("SPACE BATTLE\n\nPress SPACE to Start\n\nArrow Keys: Move\nCTRL: Shoot (Primary)\nSpace: Shoot (Backup)\nESC: Exit");
        window.draw(gameStateText);
    }
    else if (currentState == GameState::PLAYING) {
        player.render(window);
        
        for (auto& enemy : enemies) {
            enemy.render(window);
        }
        
        for (auto& bullet : bullets) {
            bullet.render(window);
        }
        
        // Render power-ups
        for (auto& powerUp : powerUps) {
            powerUp.render(window);
        }
        
        window.draw(scoreText);
        window.draw(healthText);
        window.draw(levelText);
        window.draw(targetText);
    }
    else if (currentState == GameState::LEVEL_COMPLETE) {
        gameStateText.setString("LEVEL COMPLETE!\n\nScore: " + std::to_string(score) + 
                               "\n\nPress SPACE to Continue");
        window.draw(gameStateText);
    }
    else if (currentState == GameState::GAME_OVER) {
        gameStateText.setString("GAME OVER\n\nScore: " + std::to_string(score) + 
                               "\n\nPress SPACE to Play Again\nESC to Exit");
        window.draw(gameStateText);
    }
    
    window.display();
}

void Game::spawnEnemy() {
    const auto& config = levelConfigs[currentLevel];
    
    // Random position at top of screen
    float x = static_cast<float>(rand() % 700 + 50);
    float y = -50.0f;
    
    // Determine enemy type based on level
    EnemyType type;
    int randType = rand() % 100;
    
    switch (currentLevel) {
        case Level::LEVEL_1:
            // Level 1: Only 👽 aliens
            type = EnemyType::BASIC;
            std::cout << "Level 1: Spawning 👽 alien" << std::endl;
            break;
            
        case Level::LEVEL_2:
            // Level 2: 👾 and 🤖 enemies
            if (randType < 50) {
                type = EnemyType::BASIC;  // 👾
                std::cout << "Level 2: Spawning 👾 enemy" << std::endl;
            } else {
                type = EnemyType::FAST;   // 🤖
                std::cout << "Level 2: Spawning 🤖 enemy" << std::endl;
            }
            break;
            
        case Level::LEVEL_3:
            // Level 3: 👹 and 👺 enemies
            if (randType < 40) {
                type = EnemyType::BASIC;  // 👹
                std::cout << "Level 3: Spawning 👹 enemy" << std::endl;
            } else if (randType < 80) {
                type = EnemyType::FAST;   // 👺
                std::cout << "Level 3: Spawning 👺 enemy" << std::endl;
            } else {
                type = EnemyType::HEAVY;  // Heavy version of 👹/👺
                std::cout << "Level 3: Spawning heavy 👹/👺 enemy" << std::endl;
            }
            break;
            
        default:
            type = EnemyType::BASIC;
            break;
    }
    
    // Create enemy with level-specific image
    Enemy enemy(type, x, y);
    
    // Set enemy texture based on level
    if (!config.enemyImage.empty()) {
        enemy.setTexture(config.enemyImage);
    }
    
    // Set enemy speed based on level configuration
    enemy.setSpeed(config.enemySpeed * difficultyMultiplier);
    
    enemies.push_back(enemy);
}

void Game::handleCollisions() {
    // Player-Enemy collisions
    for (auto& enemy : enemies) {
        if (Collision::checkSpriteCollision(player.getSprite(), enemy.getSprite())) {
            player.takeDamage(20);
            enemy.takeDamage(enemy.getHealth()); // Destroy enemy on collision
        }
    }
    
    // Bullet-Enemy collisions
    for (auto& bullet : bullets) {
        if (bullet.getType() == BulletType::PLAYER) {
            for (auto& enemy : enemies) {
                if (Collision::checkBulletSpriteCollision(bullet, enemy.getSprite())) {
                    enemy.takeDamage(bullet.getDamage());
                    bullet.setActive(false);
                    
                    if (!enemy.isAlive()) {
                        score += enemy.getScoreValue();
                        // Spawn power-up at enemy death location
                        sf::FloatRect enemyBounds = enemy.getBounds();
                        spawnPowerUp(enemyBounds.left + enemyBounds.width/2, enemyBounds.top + enemyBounds.height/2);
                    }
                }
            }
        }
    }
    
    // Bullet-Player collisions
    for (auto& bullet : bullets) {
        if (bullet.getType() == BulletType::ENEMY) {
            if (Collision::checkBulletSpriteCollision(bullet, player.getSprite())) {
                player.takeDamage(bullet.getDamage());
                bullet.setActive(false);
            }
        }
    }
    
    // Power-up collisions
    handlePowerUpCollisions();
}

void Game::updateUI() {
    scoreText.setString("Score: " + std::to_string(score));
    healthText.setString("Health: " + std::to_string(player.getHealth()));
    
    // Update level information
    std::string levelStr = "Level " + std::to_string(static_cast<int>(currentLevel) + 1);
    levelText.setString(levelStr);
    
    // Update target score
    targetText.setString("Target: " + std::to_string(targetScore));
}

void Game::increaseDifficulty() {
    // Increase spawn rate
    if (enemySpawnInterval > 0.5f) {
        enemySpawnInterval -= 0.2f;
    }
}

void Game::resetGame() {
    score = 0;
    currentLevel = Level::LEVEL_1;
    enemySpawnTimer = 0.0f;
    setupLevel(Level::LEVEL_1);
    // Don't set state to START here - let the calling method handle it
}

void Game::setLevel(Level level) {
    setupLevel(level);
}

Level Game::getCurrentLevel() const {
    return currentLevel;
}

int Game::getTargetScore() const {
    return targetScore;
}

bool Game::isLevelComplete() const {
    return score >= targetScore;
}

void Game::checkLevelComplete() {
    if (score >= targetScore && currentState == GameState::PLAYING) {
        currentState = GameState::LEVEL_COMPLETE;
    }
}

void Game::setupUI() {
    // Load font
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        // Try alternative fonts if Arial is not available
        if (!font.loadFromFile("C:/Windows/Fonts/calibri.ttf")) {
            std::cout << "Warning: Could not load default font. UI text may not display." << std::endl;
        }
    }
    
    // Setup UI text
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);
    
    healthText.setFont(font);
    healthText.setCharacterSize(20);
    healthText.setFillColor(sf::Color::White);
    healthText.setPosition(10, 40);
    
    // Setup level text
    levelText.setFont(font);
    levelText.setCharacterSize(20);
    levelText.setFillColor(sf::Color::Cyan);
    levelText.setPosition(10, 70);
    
    // Setup target text
    targetText.setFont(font);
    targetText.setCharacterSize(20);
    targetText.setFillColor(sf::Color::Yellow);
    targetText.setPosition(10, 100);
    
    gameStateText.setFont(font);
    gameStateText.setCharacterSize(40);
    gameStateText.setFillColor(sf::Color::White);
    gameStateText.setPosition(200, 250);
}

void Game::checkForCustomImages() {
    // This method can be implemented later for custom image detection
    std::cout << "Checking for custom game images..." << std::endl;
}

void Game::initializeLevels() {
    // Level 1: Target 300 score, basic enemies
    levelConfigs[Level::LEVEL_1] = {
        300,  // targetScore
        5,     // enemyCount
        2.0f,  // enemySpawnRate
        100.0f, // enemySpeed
        "d:/c++/sprite-space-shuttle-story-spacecraft-2d-computer-graphics-ship-space-craft.jpg", // playerImage
        "d:/c++/enemy.jpg",  // enemyImage
        ""  // bossImage (no boss in level 1)
    };
    
    // Level 2: Target 700 score, more enemies, faster
    levelConfigs[Level::LEVEL_2] = {
        700,  // targetScore
        8,     // enemyCount
        1.5f,  // enemySpawnRate
        150.0f, // enemySpeed
        "d:/c++/sprite-space-shuttle-story-spacecraft-2d-computer-graphics-ship-space-craft.jpg", // playerImage
        "d:/c++/enemy2.png", // enemyImage (harder to kill)
        ""  // bossImage (no boss in level 2)
    };
    
    // Level 3: Target 1200 score, many enemies, fastest + boss
    levelConfigs[Level::LEVEL_3] = {
        1200, // targetScore
        12,    // enemyCount
        1.0f,  // enemySpawnRate
        200.0f, // enemySpeed
        "d:/c++/sprite-space-shuttle-story-spacecraft-2d-computer-graphics-ship-space-craft.jpg", // playerImage
        "d:/c++/enemy2.png", // enemyImage (hardest)
        "d:/c++/bigboss.jpg" // bossImage (final boss)
    };
}

void Game::setupLevel(Level level) {
    currentLevel = level;
    const auto& config = levelConfigs[level];
    
    targetScore = config.targetScore;
    enemySpawnInterval = config.enemySpawnRate;
    difficultyMultiplier = 1.0f + (static_cast<int>(level) * 0.5f);
    
    // Increase player health based on level
    switch (level) {
        case Level::LEVEL_1:
            player.setMaxHealth(100);  // Default health
            player.heal(100);         // Full health for level 1
            std::cout << "Level 1: Player health set to 100" << std::endl;
            break;
        case Level::LEVEL_2:
            player.setMaxHealth(200);  // Increased health for level 2
            player.heal(200);         // Full health for level 2
            std::cout << "Level 2: Player health increased to 200" << std::endl;
            break;
        case Level::LEVEL_3:
            player.setMaxHealth(500);  // High health for level 3
            player.heal(500);         // Full health for level 3
            std::cout << "Level 3: Player health increased to 500" << std::endl;
            break;
    }
    
    // Clear existing enemies and bullets
    enemies.clear();
    bullets.clear();
    
    // Load level-specific textures for player
    if (!config.playerImage.empty()) {
        // Update player texture
        player.setTexture(config.playerImage);
    }
    
    // Spawn initial enemies for this level
    for (int i = 0; i < config.enemyCount; ++i) {
        spawnEnemy();
    }
    
    updateUI();
}

void Game::nextLevel() {
    // Reset score for next level
    score = 0;
    
    switch (currentLevel) {
        case Level::LEVEL_1:
            setupLevel(Level::LEVEL_2);
            break;
        case Level::LEVEL_2:
            setupLevel(Level::LEVEL_3);
            break;
        case Level::LEVEL_3:
            // Game completed!
            currentState = GameState::GAME_OVER;
            break;
    }
}

// Power-up system methods
void Game::spawnPowerUp(float x, float y) {
    // Random chance to spawn power-up
    if (static_cast<float>(rand()) / RAND_MAX < powerUpSpawnChance) {
        powerUps.emplace_back(x, y);
        std::cout << "Power-up spawned at (" << x << ", " << y << ")" << std::endl;
    }
}

void Game::updatePowerUps(float deltaTime) {
    // Update rapid fire timer
    if (hasRapidFire) {
        rapidFireTimer -= deltaTime;
        if (rapidFireTimer <= 0) {
            hasRapidFire = false;
            player.setFireRate(0.1f); // Reset to normal fire rate
            std::cout << "Rapid fire effect ended" << std::endl;
        }
    }
    
    // Update power-ups
    for (auto& powerUp : powerUps) {
        powerUp.update(deltaTime);
    }
    
    // Remove inactive or expired power-ups
    powerUps.erase(
        std::remove_if(powerUps.begin(), powerUps.end(),
            [](PowerUp& powerUp) {
                return !powerUp.isActive() || powerUp.isExpired() || powerUp.isOutOfBounds(800, 600);
            }),
        powerUps.end()
    );
}

void Game::handlePowerUpCollisions() {
    for (auto& powerUp : powerUps) {
        if (powerUp.isActive() && Collision::checkSpriteCollision(player.getSprite(), powerUp.getSprite())) {
            applyPowerUpEffect(powerUp.getType());
            powerUp.setActive(false);
            std::cout << "Player collected: " << powerUp.getTypeName() << std::endl;
        }
    }
}

void Game::applyPowerUpEffect(PowerUpType type) {
    switch (type) {
        case PowerUpType::RAPID_FIRE:
            hasRapidFire = true;
            rapidFireTimer = 5.0f; // 5 seconds duration
            player.setFireRate(0.05f); // Double fire rate
            std::cout << "✨ Rapid fire activated for 5 seconds!" << std::endl;
            break;
            
        case PowerUpType::HEALTH:
            int healthIncrease = static_cast<int>(player.getMaxHealth() * 0.2f); // 20% of max health
            player.heal(healthIncrease);
            std::cout << "🧚‍♀️ Health increased by " << healthIncrease << " HP!" << std::endl;
            break;
    }
}
