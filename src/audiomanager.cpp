#include "audiomanager.hpp"

AudioManager::AudioManager()
    : nextId(0)
{}

void AudioManager::update()
{
    for (auto i = sounds.begin(); i != sounds.end();)
    {
        if (i->second.getStatus() == sf::Sound::Status::Stopped) i = sounds.erase(i);
        else ++i;
    }
}

std::size_t AudioManager::addSound(sf::SoundBuffer &soundBuffer)
{
    sounds.insert({nextId, sf::Sound{soundBuffer}});
    sounds.at(nextId).play();
    return nextId++;
}

void AudioManager::pauseAllSounds()
{
    for (auto& sound : sounds)
    {
        if (sound.second.getStatus() == sf::Sound::Status::Playing) sound.second.pause();
    }
}

void AudioManager::resumeAllSounds()
{
    for (auto& sound : sounds)
    {
        if (sound.second.getStatus() == sf::Sound::Status::Paused) sound.second.play();
    }
}

void AudioManager::stopSound(std::size_t soundId)
{
    if (sounds.contains(soundId))
    {
        sounds.at(soundId).stop();
        sounds.erase(soundId);
    }
}

void AudioManager::stopAllSounds() { sounds.clear(); }
