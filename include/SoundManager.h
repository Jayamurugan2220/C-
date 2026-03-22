#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <memory>

enum class SoundType {
    PLAYER_SHOOT,
    ENEMY_SHOOT,
    EXPLOSION,
    POWERUP_PICKUP,
    POWERUP_ACTIVATE,
    SHIELD_HIT,
    BACKGROUND_MUSIC,
    GAME_OVER,
    LEVEL_COMPLETE,
    COUNT
};

class SoundManager {
private:
    std::map<SoundType, std::unique_ptr<sf::SoundBuffer>> soundBuffers;
    std::map<SoundType, std::unique_ptr<sf::Sound>> sounds;
    std::unique_ptr<sf::Music> backgroundMusic;
    
    bool soundEnabled;
    float masterVolume;
    float musicVolume;
    
    void createFallbackSounds();
    void loadUserSounds();
    
    // Helper methods for creating synthetic sounds
    bool createToneBuffer(sf::SoundBuffer& buffer, float frequency, float duration);
    bool createNoiseBuffer(sf::SoundBuffer& buffer, float duration);
    bool createAscendingToneBuffer(sf::SoundBuffer& buffer, float duration);
    bool createDescendingToneBuffer(sf::SoundBuffer& buffer, float duration);

public:
    SoundManager();
    ~SoundManager();
    
    // Initialize all sounds
    bool initialize();
    
    // Play specific sound
    void playSound(SoundType type);
    
    // Play background music
    void playBackgroundMusic(bool loop = true);
    
    // Stop background music
    void stopBackgroundMusic();
    
    // Volume control
    void setMasterVolume(float volume); // 0.0 to 1.0
    void setMusicVolume(float volume);   // 0.0 to 1.0
    float getMasterVolume() const;
    float getMusicVolume() const;
    
    // Sound control
    void setSoundEnabled(bool enabled);
    bool isSoundEnabled() const;
    
    // Cleanup
    void cleanup();
};

#endif // SOUNDMANAGER_H
