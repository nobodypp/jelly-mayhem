#pragma once

#include <map>
#include <iostream>
#include <functional>


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
                Perk(std::function<unsigned int(unsigned int)> levelUpObjective = [](unsigned int level){return (level + 1) * 10;})
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

    public:
        PerkManager();
        void bottleHitEnemyGroup(unsigned int enemiesNumber);
        float getBottleBoundsScale();
        void knockbackEnemyGotKilled();
        float getKnockbackEnemyDamage();
        void starDodged();
        float getPlayerSpeedMultiplier();
        void parryKill();
        float getHealingMultiplier();
        void singleKill();
        float getRewardFromSingleKill();
        void snipeKill(float distance);
        float getDamageRampup(float distance);
};