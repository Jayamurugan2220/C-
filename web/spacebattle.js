// Space Battle Game - JavaScript Version
class SpaceBattleGame {
    constructor() {
        this.canvas = document.getElementById('gameCanvas');
        this.ctx = this.canvas.getContext('2d');
        
        // Game state
        this.gameState = 'MENU'; // MENU, PLAYING, LEVEL_COMPLETE, GAME_OVER
        this.currentLevel = 1;
        this.score = 0;
        this.targetScore = 300;
        
        // Game objects
        this.player = null;
        this.enemies = [];
        this.bullets = [];
        
        // Input handling
        this.keys = {};
        this.mobileInput = { move: null, shooting: false };
        
        // Game timing
        this.lastTime = 0;
        this.enemySpawnTimer = 0;
        this.enemySpawnInterval = 2000; // milliseconds
        
        // Level configurations
        this.levelConfigs = {
            1: { targetScore: 300, enemyCount: 5, spawnRate: 2000, playerHealth: 100, enemySpeed: 100 },
            2: { targetScore: 700, enemyCount: 8, spawnRate: 1500, playerHealth: 200, enemySpeed: 150 },
            3: { targetScore: 1200, enemyCount: 12, spawnRate: 1000, playerHealth: 500, enemySpeed: 200 }
        };
        
        this.setupEventListeners();
    }
    
    setupEventListeners() {
        // Keyboard controls
        document.addEventListener('keydown', (e) => {
            this.keys[e.key] = true;
            
            if (e.key === ' ' || e.key === 'Space') {
                e.preventDefault();
            }
        });
        
        document.addEventListener('keyup', (e) => {
            this.keys[e.key] = false;
        });
        
        // Prevent scrolling with arrow keys
        window.addEventListener('keydown', (e) => {
            if(['ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight', ' '].includes(e.key)) {
                e.preventDefault();
            }
        });
    }
    
    init() {
        this.setupLevel(this.currentLevel);
        this.gameState = 'PLAYING';
        this.lastTime = performance.now();
        this.gameLoop();
    }
    
    setupLevel(level) {
        const config = this.levelConfigs[level];
        this.currentLevel = level;
        this.targetScore = config.targetScore;
        this.enemySpawnInterval = config.spawnRate;
        
        // Create player
        this.player = new Player(400, 500, config.playerHealth);
        
        // Clear existing enemies and bullets
        this.enemies = [];
        this.bullets = [];
        this.enemySpawnTimer = 0;
        
        // Spawn initial enemies
        for (let i = 0; i < config.enemyCount; i++) {
            this.spawnEnemy();
        }
        
        // Update UI
        this.updateUI();
        
        console.log(`Level ${level}: Player health set to ${config.playerHealth}`);
    }
    
    spawnEnemy() {
        const x = Math.random() * 700 + 50;
        const y = -50;
        const config = this.levelConfigs[this.currentLevel];
        
        // Random enemy type
        const types = ['BASIC', 'FAST', 'HEAVY'];
        const type = types[Math.floor(Math.random() * types.length)];
        
        this.enemies.push(new Enemy(type, x, y, config.enemySpeed));
    }
    
    gameLoop(currentTime = 0) {
        if (this.gameState !== 'PLAYING') return;
        
        const deltaTime = (currentTime - this.lastTime) / 1000; // Convert to seconds
        this.lastTime = currentTime;
        
        this.update(deltaTime);
        this.render();
        
        requestAnimationFrame((time) => this.gameLoop(time));
    }
    
    update(deltaTime) {
        // Handle input
        this.handleInput();
        
        // Update player
        this.player.update(deltaTime);
        
        // Keep player in bounds
        this.player.x = Math.max(20, Math.min(780, this.player.x));
        this.player.y = Math.max(20, Math.min(580, this.player.y));
        
        // Update enemies
        for (let i = this.enemies.length - 1; i >= 0; i--) {
            const enemy = this.enemies[i];
            enemy.update(deltaTime);
            
            // Remove enemies that are off screen
            if (enemy.y > 650) {
                this.enemies.splice(i, 1);
                continue;
            }
            
            // Enemy shooting
            if (enemy.shouldShoot()) {
                this.createBullet(enemy.x, enemy.y + 15, 0, 1, 'ENEMY');
                enemy.resetFireTimer();
            }
            
            // Check collision with player
            if (this.checkCollision(this.player, enemy)) {
                this.player.takeDamage(20);
                this.enemies.splice(i, 1);
                
                if (this.player.health <= 0) {
                    this.gameOver();
                    return;
                }
            }
        }
        
        // Update bullets
        for (let i = this.bullets.length - 1; i >= 0; i--) {
            const bullet = this.bullets[i];
            bullet.update(deltaTime);
            
            // Remove bullets that are off screen
            if (bullet.isOutOfBounds()) {
                this.bullets.splice(i, 1);
                continue;
            }
            
            // Check bullet collisions
            if (bullet.type === 'PLAYER') {
                for (let j = this.enemies.length - 1; j >= 0; j--) {
                    const enemy = this.enemies[j];
                    if (this.checkCollision(bullet, enemy)) {
                        this.enemies.splice(j, 1);
                        this.bullets.splice(i, 1);
                        this.score += 10;
                        this.updateUI();
                        break;
                    }
                }
            } else if (bullet.type === 'ENEMY') {
                if (this.checkCollision(bullet, this.player)) {
                    this.player.takeDamage(15);
                    this.bullets.splice(i, 1);
                    
                    if (this.player.health <= 0) {
                        this.gameOver();
                        return;
                    }
                }
            }
        }
        
        // Spawn new enemies
        this.enemySpawnTimer += deltaTime * 1000;
        if (this.enemySpawnTimer >= this.enemySpawnInterval) {
            this.spawnEnemy();
            this.enemySpawnTimer = 0;
        }
        
        // Check level completion
        if (this.score >= this.targetScore) {
            this.levelComplete();
        }
        
        this.updateUI();
    }
    
    handleInput() {
        // Player movement
        if (this.keys['ArrowLeft'] || this.mobileInput.move === 'left') {
            this.player.move(-300, 0);
        }
        if (this.keys['ArrowRight'] || this.mobileInput.move === 'right') {
            this.player.move(300, 0);
        }
        if (this.keys['ArrowUp'] || this.mobileInput.move === 'up') {
            this.player.move(0, -300);
        }
        if (this.keys['ArrowDown'] || this.mobileInput.move === 'down') {
            this.player.move(0, 300);
        }
        
        // Shooting
        if (this.keys[' '] || this.keys['Control'] || this.mobileInput.shooting) {
            if (this.player.canShoot()) {
                this.createBullet(this.player.x, this.player.y - 20, 0, -1, 'PLAYER');
                this.player.resetFireTimer();
            }
        }
    }
    
    createBullet(x, y, dirX, dirY, type) {
        this.bullets.push(new Bullet(type, x, y, dirX, dirY));
    }
    
    checkCollision(obj1, obj2) {
        return obj1.x < obj2.x + obj2.width &&
               obj1.x + obj1.width > obj2.x &&
               obj1.y < obj2.y + obj2.height &&
               obj1.y + obj1.height > obj2.y;
    }
    
    render() {
        // Clear canvas
        this.ctx.fillStyle = '#000';
        this.ctx.fillRect(0, 0, 800, 600);
        
        // Draw stars background
        this.drawStars();
        
        // Draw game objects
        this.player.render(this.ctx);
        
        this.enemies.forEach(enemy => enemy.render(this.ctx));
        this.bullets.forEach(bullet => bullet.render(this.ctx));
        
        // Draw game state messages
        if (this.gameState === 'LEVEL_COMPLETE') {
            this.drawLevelComplete();
        } else if (this.gameState === 'GAME_OVER') {
            this.drawGameOver();
        }
    }
    
    drawStars() {
        this.ctx.fillStyle = '#fff';
        for (let i = 0; i < 50; i++) {
            const x = (i * 73) % 800;
            const y = (i * 37) % 600;
            const size = (i % 3) + 1;
            this.ctx.fillRect(x, y, size, size);
        }
    }
    
    drawLevelComplete() {
        this.ctx.fillStyle = 'rgba(0, 0, 0, 0.7)';
        this.ctx.fillRect(0, 0, 800, 600);
        
        this.ctx.fillStyle = '#00ffff';
        this.ctx.font = 'bold 48px Arial';
        this.ctx.textAlign = 'center';
        this.ctx.fillText('LEVEL COMPLETE!', 400, 250);
        
        this.ctx.font = '24px Arial';
        this.ctx.fillText(`Score: ${this.score}`, 400, 320);
        this.ctx.fillText('Press SPACE to continue', 400, 380);
    }
    
    drawGameOver() {
        this.ctx.fillStyle = 'rgba(0, 0, 0, 0.7)';
        this.ctx.fillRect(0, 0, 800, 600);
        
        this.ctx.fillStyle = '#ff0000';
        this.ctx.font = 'bold 48px Arial';
        this.ctx.textAlign = 'center';
        this.ctx.fillText('GAME OVER', 400, 250);
        
        this.ctx.fillStyle = '#fff';
        this.ctx.font = '24px Arial';
        this.ctx.fillText(`Final Score: ${this.score}`, 400, 320);
        this.ctx.fillText('Press SPACE to return to menu', 400, 380);
    }
    
    levelComplete() {
        this.gameState = 'LEVEL_COMPLETE';
        
        setTimeout(() => {
            if (this.currentLevel < 3) {
                this.currentLevel++;
                this.setupLevel(this.currentLevel);
                this.gameState = 'PLAYING';
            } else {
                this.gameComplete();
            }
        }, 2000);
    }
    
    gameComplete() {
        this.gameState = 'GAME_OVER';
        // You can add a special victory screen here
    }
    
    gameOver() {
        this.gameState = 'GAME_OVER';
    }
    
    updateUI() {
        document.getElementById('gameHealth').textContent = this.player.health;
        document.getElementById('gameScore').textContent = this.score;
        document.getElementById('gameLevel').textContent = this.currentLevel;
        document.getElementById('gameTarget').textContent = this.targetScore;
    }
}

// Player class
class Player {
    constructor(x, y, maxHealth) {
        this.x = x;
        this.y = y;
        this.width = 40;
        this.height = 40;
        this.speed = 225;
        this.health = maxHealth;
        this.maxHealth = maxHealth;
        this.fireRate = 0.1;
        this.fireTimer = 0;
    }
    
    update(deltaTime) {
        if (this.fireTimer > 0) {
            this.fireTimer -= deltaTime;
        }
    }
    
    move(dx, dy) {
        this.x += dx;
        this.y += dy;
    }
    
    canShoot() {
        return this.fireTimer <= 0;
    }
    
    resetFireTimer() {
        this.fireTimer = this.fireRate;
    }
    
    takeDamage(amount) {
        this.health -= amount;
        if (this.health < 0) this.health = 0;
    }
    
    render(ctx) {
        // Draw player as a blue rectangle (ship)
        ctx.fillStyle = '#0080ff';
        ctx.fillRect(this.x - 20, this.y - 20, 40, 40);
        
        // Draw ship details
        ctx.fillStyle = '#00ffff';
        ctx.fillRect(this.x - 5, this.y - 25, 10, 15);
        ctx.fillRect(this.x - 15, this.y - 10, 30, 5);
    }
}

// Enemy class
class Enemy {
    constructor(type, x, y, speed) {
        this.type = type;
        this.x = x;
        this.y = y;
        this.width = 30;
        this.height = 30;
        this.speed = speed;
        this.direction = { x: (Math.random() - 0.5) * 0.5, y: 1 };
        
        // Set properties based on type
        switch(type) {
            case 'BASIC':
                this.health = 30;
                this.fireRate = 2.0;
                this.shootChance = 0.01;
                this.color = '#ff0000';
                break;
            case 'FAST':
                this.health = 20;
                this.fireRate = 1.5;
                this.shootChance = 0.02;
                this.color = '#ffff00';
                this.speed *= 2;
                break;
            case 'HEAVY':
                this.health = 50;
                this.fireRate = 3.0;
                this.shootChance = 0.015;
                this.color = '#ff00ff';
                this.width = 40;
                this.height = 40;
                this.speed *= 0.5;
                break;
        }
        
        this.fireTimer = 0;
    }
    
    update(deltaTime) {
        this.x += this.direction.x * this.speed * deltaTime;
        this.y += this.direction.y * this.speed * deltaTime;
        
        if (this.fireTimer > 0) {
            this.fireTimer -= deltaTime;
        }
    }
    
    shouldShoot() {
        return this.fireTimer <= 0 && Math.random() < this.shootChance;
    }
    
    resetFireTimer() {
        this.fireTimer = this.fireRate;
    }
    
    render(ctx) {
        ctx.fillStyle = this.color;
        ctx.fillRect(this.x - this.width/2, this.y - this.height/2, this.width, this.height);
        
        // Draw enemy details
        ctx.fillStyle = '#000';
        ctx.fillRect(this.x - 5, this.y - 5, 10, 10);
    }
}

// Bullet class
class Bullet {
    constructor(type, x, y, dirX, dirY) {
        this.type = type;
        this.x = x;
        this.y = y;
        this.width = 6;
        this.height = 12;
        this.direction = { x: dirX, y: dirY };
        
        if (type === 'PLAYER') {
            this.speed = 500;
            this.color = '#00ffff';
        } else {
            this.speed = 300;
            this.color = '#ff0000';
        }
    }
    
    update(deltaTime) {
        this.x += this.direction.x * this.speed * deltaTime;
        this.y += this.direction.y * this.speed * deltaTime;
    }
    
    isOutOfBounds() {
        return this.x < -50 || this.x > 850 || this.y < -50 || this.y > 650;
    }
    
    render(ctx) {
        ctx.fillStyle = this.color;
        ctx.fillRect(this.x - 3, this.y - 6, 6, 12);
    }
}

// Global game instance
let game = null;

// Menu functions
function startGame() {
    document.getElementById('menuScreen').classList.add('hidden');
    document.getElementById('gameScreen').classList.remove('hidden');
    
    if (!game) {
        game = new SpaceBattleGame();
    }
    
    game.init();
}

function backToMenu() {
    document.getElementById('menuScreen').classList.remove('hidden');
    document.getElementById('gameScreen').classList.add('hidden');
    
    if (game) {
        game.gameState = 'MENU';
    }
}

// Mobile controls
function mobileMove(direction) {
    if (game) {
        game.mobileInput.move = direction;
    }
}

function stopMobileMove() {
    if (game) {
        game.mobileInput.move = null;
    }
}

function mobileShoot() {
    if (game) {
        game.mobileInput.shooting = true;
    }
}

function stopMobileShoot() {
    if (game) {
        game.mobileInput.shooting = false;
    }
}

// Handle space key for level complete/game over
document.addEventListener('keydown', (e) => {
    if (game && game.gameState === 'LEVEL_COMPLETE' && e.key === ' ') {
        if (game.currentLevel < 3) {
            game.currentLevel++;
            game.setupLevel(game.currentLevel);
            game.gameState = 'PLAYING';
        } else {
            backToMenu();
        }
    }
    
    if (game && game.gameState === 'GAME_OVER' && e.key === ' ') {
        backToMenu();
    }
});

// Initialize menu display
document.addEventListener('DOMContentLoaded', () => {
    // Update initial menu display
    document.getElementById('targetScore').textContent = '300';
    document.getElementById('healthDisplay').textContent = '100';
    document.getElementById('levelDisplay').textContent = '1';
    document.getElementById('scoreDisplay').textContent = '0';
});
