#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Collision.h"
#include "PowerUp.h"

// Game states enumeration
enum class GameState {
    START,
    PLAYING,
    LEVEL_COMPLETE,
    GAME_OVER
};

// Level system
enum class Level {
    LEVEL_1,
    LEVEL_2,
    LEVEL_3
};

struct LevelConfig {
    int targetScore;
    int enemyCount;
    float enemySpawnRate;
    float enemySpeed;
    std::string playerImage;
    std::string enemyImage;
    std::string bossImage;
};

class Game {
private:
    sf::RenderWindow window;
    GameState currentState;
    Level currentLevel;
    Player player;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    sf::Font font;
    sf::Text scoreText;
    sf::Text healthText;
    sf::Text levelText;
    sf::Text targetText;
    sf::Text gameStateText;
    
    // Level management
    std::map<Level, LevelConfig> levelConfigs;
    int score;
    int targetScore;
    float enemySpawnTimer;
    float enemySpawnInterval;
    float difficultyMultiplier;
    
    // Power-up system
    std::vector<PowerUp> powerUps;
    float powerUpSpawnChance;
    float rapidFireTimer;
    bool hasRapidFire;
    
    void initializeLevels();
    void setupLevel(Level level);
    void nextLevel();
    void checkLevelComplete();
    
    // Power-up methods
    void spawnPowerUp(float x, float y);
    void updatePowerUps(float deltaTime);
    void handlePowerUpCollisions();
    void applyPowerUpEffect(PowerUpType type);
    
    // Private methods
    void processEvents();
    void update(float deltaTime);
    void render();
    void spawnEnemy();
    void handleCollisions();
    void updateUI();
    void increaseDifficulty();
    void resetGame();
    void setupUI();
    void showUploadInstructions();
    void checkForCustomImages();
    
public:
    Game();
    void run();
    
    // Level management
    void setLevel(Level level);
    Level getCurrentLevel() const;
    int getTargetScore() const;
    bool isLevelComplete() const;
};

#endif // GAME_H
