#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <SFML/System.hpp>
#include "paths.hpp"
#include "drawable.hpp"

namespace ScoreKey
{
    constexpr std::string_view kills = "0";
    constexpr std::string_view time = "1";
    constexpr std::string_view group_perk = "2";
    constexpr std::string_view knockout_perk = "3";
    constexpr std::string_view dodger_perk = "4";
    constexpr std::string_view healing_perk = "5";
    constexpr std::string_view single_perk = "6";
    constexpr std::string_view increasing_perk = "7";
    constexpr std::string_view reload_perk = "8";
    constexpr std::string_view group_size = "9";
    constexpr std::string_view single_damage = "10";
    constexpr std::string_view bottles_thrown = "11";
    constexpr std::string_view damage_dealt = "12";
    constexpr std::string_view health_healed = "13";
}

class ScoreManager
{
    private:
        const std::filesystem::path scoresPath = Paths::saveDirectory() / "scores.json";
        nlohmann::json highScores = {
            { ScoreKey::kills, {{"description", "Max kills"}, {"value", 0}} },
            { ScoreKey::time, {{"description", "Max time"}, {"value", 0}} },
            { ScoreKey::group_perk, {{"description", "Highest \"crowd control\" perk level"}, {"value", 0}} },
            { ScoreKey::knockout_perk, {{"description", "Highest \"knockout crits\" perk level"}, {"value", 0}} },
            { ScoreKey::dodger_perk, {{"description", "Highest \"dodger\" perk level"}, {"value", 0}} },
            { ScoreKey::healing_perk, {{"description", "Highest \"healing upgrade\" perk level"}, {"value", 0}} },
            { ScoreKey::single_perk, {{"description", "Highest \"single kill boost\" perk level"}, {"value", 0}} },
            { ScoreKey::increasing_perk, {{"description", "Highest \"increasing damage\" perk level"}, {"value", 0}} },
            { ScoreKey::reload_perk, {{"description", "Highest \"faster reload\" perk level"}, {"value", 0}} },
            { ScoreKey::group_size, {{"description", "Biggest group of enemies hit with one bottle"}, {"value", 0}} },
            { ScoreKey::single_damage, {{"description", "Highest damage inflicted with one bottle"}, {"value", 0.f}} },
            { ScoreKey::bottles_thrown, {{"description", "Max bottles thrown"}, {"value", 0}} },
            { ScoreKey::damage_dealt, {{"description", "Highest damage dealt"}, {"value", 0}} },
            { ScoreKey::health_healed, {{"description", "Highest health healed"}, {"value", 0}} }
        };
        nlohmann::json currentScores = highScores;
        sf::Time gameTime = sf::Time::Zero;

        void loadScores();
        void saveScores();

    public:
        ScoreManager();
        void update(sf::Time deltaTime);
        void reset();
        std::string getScoresList();
        void registerKill();
        void updateCurrentScore(std::string_view scoreKey, int value);
        void registerDamage(int damage);
        void registerHeal(int healed);
        void registerBottleThrown();
        void registerGroupHit(int groupSize);
        std::string getGameTime(bool showCurrentScore = true);
        int getCurrentScore(std::string_view scoreKey);
};