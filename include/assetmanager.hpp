#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "animation.hpp"
#include <vector>
#include <filesystem>
#include <iostream>
#include <array>
#ifdef _WIN32
    #include <windows.h>
#endif


class AssetManager
{
    private:
        static std::filesystem::path getExecutableDirectory();
        static std::filesystem::path findAssetsDirectory();
        static std::filesystem::path assetPath(const std::filesystem::path& relativePath);

        void loadFramesFromLocation(std::vector<sf::Texture>& vector, std::string filePrefix);

    public:
        AssetManager();

        sf::Texture playerCorpseDefaultTexture;
        sf::Texture playerLegsDefaultTexture;
        sf::Texture playerHandsDefaultTexture;

        sf::Texture groundTexture;

        sf::Texture bottleTexture;

        sf::Texture jellyDefault;

        std::vector<sf::Texture> legsRunningFrames;
        std::vector<sf::Texture> bottleThrowingFrames;
        std::vector<sf::Texture> bottleBreakingFrames;
        std::vector<sf::Texture> jellyWalkingFrames;
        std::vector<sf::Texture> jellyDyingFrames;
        std::vector<sf::Texture> jellyShootingFrames;
        std::vector<sf::Texture> jellyKnockbackFrames;
        std::vector<sf::Texture> starFlyFrames;
        std::vector<sf::Texture> starExplodeFrames;
        std::vector<sf::Texture> jellyBitingFrames;
        std::vector<sf::Texture> bottleHitFrames;
        std::vector<sf::Texture> bottleBarChargedFrames;

        sf::Font font;

        sf::SoundBuffer bottleBreakSound;
        sf::SoundBuffer jellyBiteSound;
        sf::SoundBuffer jellyKnockbackSound;
        sf::SoundBuffer jellyDieSound;
        sf::SoundBuffer jellyShootSound;
        sf::SoundBuffer playerDieSound;
        sf::SoundBuffer playerHitSound;
        sf::SoundBuffer starHitSound;
        sf::SoundBuffer perkSound;
        sf::SoundBuffer chargingHitSound;
        sf::SoundBuffer buttonSound;
};