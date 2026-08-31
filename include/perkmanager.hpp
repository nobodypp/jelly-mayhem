#pragma once

#include <map>
#include <iostream>
#include <functional>
#include "assetmanager.hpp"
#include "audiomanager.hpp"
#include <queue>


class PerkManager
{
    private:
        class Perk
        {
            private:
                unsigned int objectiveRequired;
                unsigned int objectiveCounter = 0;
                unsigned int level = 0;
                std::function<unsigned int(unsigned int)> levelUpObjective;
            
            public:
                Perk(std::function<unsigned int(unsigned int)> levelUpObjective = [](unsigned int level){return (level + 1) * 5;})
                : objectiveRequired(levelUpObjective(0)), 
                  levelUpObjective(levelUpObjective)
                {}

                unsigned int getLevel() { return level; }

                bool increaseObjective()
                {
                    objectiveCounter++;
                    if (objectiveCounter >= objectiveRequired)
                    {
                        level++;
                        objectiveCounter = 0;
                        objectiveRequired = levelUpObjective(level);

                        return true;
                    }
                    return false;
                }

                void reset()
                {
                    objectiveCounter = 0;
                    level = 0;
                    objectiveRequired = levelUpObjective(level);
                }
        };

        static constexpr float longDistance = 500.f;
        static constexpr float closeDistance = 180.f;
        std::map<std::string, PerkManager::Perk> perks;
        AssetManager& assets;
        AudioManager& audio;
        bool singleKillReward;
        std::queue<std::string> announcements;

        void addAnouncement(std::string name, std::string key);

    public:
        PerkManager(AssetManager& assets, AudioManager& audio);
        void registerGroupBottleHit(unsigned int enemiesNumber);
        float getBottleBoundsScale();
        void registerKnockbackHit();
        float getKnockbackEnemyDamage();
        void registerStarDodged();
        float getPlayerSpeedMultiplier();
        void registerBlockKill();
        float getHealingMultiplier();
        void registerSingleKill();
        bool isNextBottleBoosted();
        float claimBoostedBottle();
        void registerKill(float distance);
        void registerHit(float distance);
        float getDamageRampup(float distance);
        float getReloadSpeedMultiplier();
        std::string getNextAnnouncement();
        void reset();

};