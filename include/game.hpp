#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "player.hpp"
#include "ground.hpp"
#include "assetmanager.hpp"
#include "projectilemanager.hpp"
#include "enemymanager.hpp"
#include "collisionmanager.hpp"
#include "playerui.hpp"
#include "damagetextmanager.hpp"
#include "random.hpp"
#include "gamestate.hpp"
#include "perkmanager.hpp"
#include "audiomanager.hpp"

class GameManager
{
    private:
        static constexpr int winWidth = 1500, winHeight = 1000;
        sf::RenderWindow gameWindow{sf::VideoMode{{winWidth, winHeight}}, ""};
        sf::View playerView{gameWindow.getDefaultView()};
        GameState currentState = GameState::Play;
        sf::View uiView{playerView};
        sf::Clock clock;
        RandomGenerator randomizer;
        AssetManager assets;
        AudioManager audio;
        PerkManager perks{assets, audio};
        PlayerUI ui{assets, perks, audio, currentState};
        ProjectileManager projectiles{assets, perks, audio};
        Player player{assets, projectiles, perks, audio};
        EnemyManager enemies{assets, player, projectiles, randomizer, perks, audio};
        DamageTextManager texts{assets};
        CollisionManager collisions{enemies, projectiles, texts, perks};
        Ground ground{assets};

        float windowZoom = 1.f;

        void handleEvents();
        void update(sf::Time deltaTime);
        void render();
        void updateViews();
        void changeState(GameState state);

    public:
        GameManager();
        void GameLoop();
};