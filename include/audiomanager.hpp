#pragma once

#include <SFML/Audio.hpp>
#include <map>


class AudioManager
{
    private:
        std::map<std::size_t, sf::Sound> sounds;
        std::size_t nextId = 0;
        int volume = 100;

    public:
        AudioManager();
        void update();
        std::size_t addSound(sf::SoundBuffer& soundBuffer);
        void pauseAllSounds();
        void resumeAllSounds();
        void stopSound(std::size_t soundId);
        void stopAllSounds();
        int getVolume();
        void setVolume(int volume);
};