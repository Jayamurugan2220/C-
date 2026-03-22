#ifndef GAMEENHANCED_H
#define GAMEENHANCED_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include <random>
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "PowerUp.h"
#include "SoundManager.h"
#include "Collision.h"

enum class GameState {
    START,
    PLAYING,
    PAUSED,
    GAME_OVER,
    LEVEL_COMPLETE
};

enum class WeaponType {
    SINGLE,
    DOUBLE,
    TRIPLE,
    RAPID,
    SPREAD
};

struct GameStats {
    int score = 0;
    int highScore = 0;
    int level = 1;
    int enemiesKilled = 0;
    int powerUpsCollected = 0;
    int shotsFired = 0;
    int accuracy = 0;
    float playTime = 0.0f;
};

class GameEnhanced {
private:
    // Window and rendering
    sf::RenderWindow window;
    sf::VideoMode videoMode;
    std::string windowTitle;
    
    // Game objects
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Bullet>> bullets;
    std::vector<std::unique_ptr<PowerUp>> powerUps;
    std::vector<std::unique_ptr<sf::Sprite>> particles;
    
    // Game state
    GameState currentState;
    GameStats stats;
    WeaponType currentWeapon;
    
    // Timing and difficulty
    sf::Clock gameClock;
    sf::Clock enemySpawnClock;
    sf::Clock powerUpSpawnClock;
    float deltaTime;
    float enemySpawnInterval;
    float powerUpSpawnInterval;
    float difficultyMultiplier;
    
    // UI elements
    sf::Font font;
    sf::Text scoreText;
    sf::Text healthText;
    sf::Text levelText;
    sf::Text gameStateText;
    sf::Text weaponText;
    sf::Text statsText;
    
    // Background
    sf::Sprite background;
    sf::Texture backgroundTexture;
    std::vector<sf::Sprite> stars;
    
    // Sound and music
    std::unique_ptr<SoundManager> soundManager;
    
    // Power-up effects
    float rapidFireTimer;
    float shieldTimer;
    float multiShotTimer;
    float speedBoostTimer;
    bool shieldActive;
    
    // Boss battle
    bool bossActive;
    std::unique_ptr<Enemy> boss;
    float bossHealth;
    float maxBossHealth;
    
    // Random number generation
    std::mt19937 rng;
    std::uniform_real_distribution<float> spawnDist;
    
    // Private methods
    void processEvents();
    void update(float deltaTime);
    void render();
    void spawnEnemy();
    void spawnPowerUp();
    void spawnBoss();
    void handleCollisions();
    void updateUI();
    void increaseDifficulty();
    void resetGame();
    void nextLevel();
    void createParticles(float x, float y, int count, sf::Color color);
    void updateParticles(float deltaTime);
    void createBackground();
    void updateBackground(float deltaTime);
    void applyPowerUp(PowerUpType type);
    void updatePowerUpEffects(float deltaTime);
    void updateWeapon();
    void calculateStats();
    void saveHighScore();
    void loadHighScore();
    
    // Rendering methods
    void renderGame();
    void renderUI();
    void renderBackground();
    void renderParticles();
    void renderBossHealth();

public:
    GameEnhanced();
    ~GameEnhanced();
    
    // Main game loop
    void run();
    
    // Game state management
    void startGame();
    void pauseGame();
    void resumeGame();
    void gameOver();
    void levelComplete();
    
    // Configuration
    void setWindowSize(int width, int height);
    void setWindowTitle(const std::string& title);
    void setDifficulty(float multiplier);
    
    // Statistics
    GameStats getStats() const;
    void resetStats();
    
    // Sound control
    void setSoundVolume(float volume);
    void setMusicVolume(float volume);
    void toggleSound();
    
    // Cheats for testing (remove in production)
    void addScore(int points);
    void setHealth(int health);
    void spawnPowerUpAt(float x, float y);
};

#endif // GAMEENHANCED_H
