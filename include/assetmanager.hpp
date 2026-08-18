#pragma once

#include <SFML/Graphics.hpp>
#include "animation.hpp"
#include <vector>
#include <filesystem>
#include <iostream>


class AssetManager
{
    private:
        void loadFromLocation(std::vector<sf::Texture>& vector, std::string filePrefix);

    public:
        AssetManager();

        sf::Texture playerCorpseDefaultTexture;
        sf::Texture playerLegsDefaultTexture;
        sf::Texture playerHandsDefaultTexture;

        sf::Texture groundTexture;

        sf::Texture bottleTexture;

        sf::Texture jellyDefault;

        std::vector<sf::Texture> legsRunning;
        std::vector<sf::Texture> bottleThrowing;
        std::vector<sf::Texture> bottleBreaking;
        std::vector<sf::Texture> jellyWalking;
        std::vector<sf::Texture> jellyDying;
        std::vector<sf::Texture> jellyShooting;
        std::vector<sf::Texture> jellyKnockback;
        std::vector<sf::Texture> starFly;
        std::vector<sf::Texture> starExplode;
        std::vector<sf::Texture> jellyBiting;
        std::vector<sf::Texture> bottleHit;

        sf::Font font;
};