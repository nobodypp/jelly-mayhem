#pragma once

#include <functional>
#include <queue>
#include <array>
#include <tuple>
#include "assetmanager.hpp"
#include "audiomanager.hpp"


class PerkManager
{
    private:
        class Perk
        {
            private:
                unsigned int objectiveCounter = 0;
                unsigned int level = 0;
                std::function<unsigned int(unsigned int)> requiredObjective;
                std::string name;
                std::string objective;
                std::string reward;
            
            public:
                Perk(std::string name, std::string objective, std::string reward, std::function<unsigned int(unsigned int)> requiredObjective = [](unsigned int level){return (level + 1) * 5;})
                  : requiredObjective(requiredObjective), 
                    objective(objective), 
                    reward(reward),
                    name(name)
                {}

                unsigned int getLevel() { return level; }

                bool increaseObjective()
                {
                    objectiveCounter++;
                    if (objectiveCounter >= requiredObjective(level))
                    {
                        level++;
                        objectiveCounter = 0;

                        return true;
                    }
                    return false;
                }

                void reset()
                {
                    objectiveCounter = 0;
                    level = 0;
                }

                std::string getName() const { return name + " " + std::to_string(level); }

                std::string getObjective() const { return objective + ": " + std::to_string(objectiveCounter) + "/" + std::to_string(requiredObjective(level)); }

                std::string getReward() const { return reward; }

                void updateObjective(std::string text) { objective = text; }
        };

        static constexpr float longDistance = 500.f;
        static constexpr float closeDistance = 180.f;

        enum class PerkId
        {
            Group,
            Knockback,
            Dodger,
            Parry,
            Single,
            Sniper,
            Close,

            Count
        };

        static constexpr std::size_t index(PerkId id)
        {
            return static_cast<std::size_t>(id);
        }

        std::array<Perk, static_cast<std::size_t>(PerkId::Count)> perks;

        Perk& getPerk(PerkId id)
        {
            return perks[index(id)];
        }

        AssetManager& assets;
        AudioManager& audio;
        bool singleKillReward;
        std::queue<std::string> announcements;

        void increasePerk(PerkId id);

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
        std::tuple<std::string, std::string, std::string> getPerkInfo(std::size_t id);      
};