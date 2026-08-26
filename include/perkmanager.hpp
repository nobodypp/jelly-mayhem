#pragma once

#include <map>
#include <iostream>
#include <functional>
#include "assetmanager.hpp"
#include <SFML/Audio.hpp>
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
        };

        std::map<std::string, PerkManager::Perk> perks;
        bool singleKillReward;
        static constexpr float longDistance = 500.f;
        AssetManager& assets;
        sf::Sound perkSound;
        std::queue<std::string> announcements;

    public:
        PerkManager(AssetManager& assets);
        void bottleHitEnemyGroup(unsigned int enemiesNumber);
        float getBottleBoundsScale();
        void knockbackEnemyHit();
        float getKnockbackEnemyDamage();
        void starDodged();
        float getPlayerSpeedMultiplier();
        void parryKill();
        float getHealingMultiplier();
        void singleKill();
        bool isNextBottleBoosted();
        float claimBoostedBottle();
        void snipeKill(float distance);
        float getDamageRampup(float distance);
        std::string getAnnouncement();
};