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
        sf::Clock clock;
        AssetManager assets;
        AudioManager audio;
        sf::RenderWindow gameWindow;
        sf::View playerView;
        sf::View uiView;
        PerkManager perks;
        PlayerUI ui;
        Player player;
        ProjectileManager projectiles;
        EnemyManager enemies;
        DamageTextManager texts;
        CollisionManager collisions;
        Ground ground;
        RandomGenerator randomizer;

        GameState currentState;

        float windowZoom;

        void handleEvents();
        void update(sf::Time deltaTime);
        void render();
        void updateViews();
        void changeState(GameState state);

    public:
        GameManager();
        void GameLoop();
};