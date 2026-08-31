#include "perkmanager.hpp"


PerkManager::PerkManager(AssetManager &assets, AudioManager& audio)
    : singleKillReward(false),
      assets(assets),
      audio(audio)
{
    perks.insert({"group", Perk{[](unsigned int level){return 10;}}});
    perks.insert({"knockback", Perk{[](unsigned int level){return (level + 1) * 5;}}});
    perks.insert({"dodger", Perk{[](unsigned int level){return (level + 1) * 20;}}});
    perks.insert({"parry", Perk{}});
    perks.insert({"single", Perk{}});
    perks.insert({"sniper", Perk{}});
    perks.insert({"close", Perk{[](unsigned int level){return (level + 1) * 15;}}});
}

void PerkManager::registerGroupBottleHit(unsigned int enemiesNumber)
{
    if (enemiesNumber >= 4 + perks.at("group").getLevel() * 2)
    {
        if (perks.at("group").increaseObjective()) addAnouncement("Crowd control", "group");
    }
}

void PerkManager::registerKnockbackHit()
{
    if (perks.at("knockback").increaseObjective()) addAnouncement("Knockout crits", "knockback");   
}

void PerkManager::registerStarDodged()
{
    if (perks.at("dodger").increaseObjective()) addAnouncement("Dodger", "dodger");
}

void PerkManager::registerBlockKill()
{
    if (perks.at("parry").increaseObjective()) addAnouncement("Healing upgrade", "parry");
}

void PerkManager::registerSingleKill()
{
    // If already have perk, next shot will be boosted
    if (perks.at("single").getLevel() >= 1) singleKillReward = true;
    
    if (perks.at("single").increaseObjective()) addAnouncement("Single kill boost", "single");
}

void PerkManager::registerKill(float distance)
{
    if (distance >= longDistance)
    {
        if (perks.at("sniper").increaseObjective()) addAnouncement("Increasing damage", "sniper");
    }
}

void PerkManager::registerHit(float distance)
{
    if (distance <= closeDistance)
    {
        if (perks.at("close").increaseObjective()) addAnouncement("Faster reload", "close");
    }
}

float PerkManager::getBottleBoundsScale() { return 1.f + perks.at("group").getLevel() * 0.3f; }

float PerkManager::getKnockbackEnemyDamage() { return 1.f + perks.at("knockback").getLevel() * 2.0f; }

float PerkManager::getPlayerSpeedMultiplier() { return 1.f + perks.at("dodger").getLevel() * 0.4f; }

float PerkManager::getHealingMultiplier() { return 1.f + perks.at("parry").getLevel() * 0.25f; }

bool PerkManager::isNextBottleBoosted() { return singleKillReward; }

float PerkManager::claimBoostedBottle()
{
    float reward = singleKillReward ? (1.f + perks.at("single").getLevel() * 1.f) : 1.f;
    singleKillReward = false;
    return reward;
}

float PerkManager::getDamageRampup(float distance)
{
    return distance / longDistance * perks.at("sniper").getLevel() * 0.5f + 1.f;
}

float PerkManager::getReloadSpeedMultiplier()
{
    return 1.f + perks.at("close").getLevel() * 0.3f;
}

std::string PerkManager::getNextAnnouncement()
{
    if (announcements.empty()) return "";

    std::string result = announcements.front();
    announcements.pop();
    return result;
}

void PerkManager::reset()
{
    for (auto& perk : perks) perk.second.reset(); 
}

void PerkManager::addAnouncement(std::string name, std::string key)
{
    audio.addSound(assets.perkSound);
    announcements.push(name + " " + (perks.at(key).getLevel() == 1 ? "" : std::to_string(perks.at(key).getLevel()) + " ") + "unlocked!");    
}