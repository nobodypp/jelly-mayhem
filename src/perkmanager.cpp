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
}

void PerkManager::bottleHitEnemyGroup(unsigned int enemiesNumber)
{
    if (enemiesNumber >= 4 + perks.at("group").getLevel() * 2)
    {
        if (perks.at("group").increaseObjective()) addAnouncement("Crowd control", "group");
    }
}

void PerkManager::knockbackEnemyHit()
{
    if (perks.at("knockback").increaseObjective()) addAnouncement("Knockout crits", "knockback");   
}

void PerkManager::starDodged()
{
    if (perks.at("dodger").increaseObjective()) addAnouncement("Dodger", "dodger");
}

void PerkManager::parryKill()
{
    if (perks.at("parry").increaseObjective()) addAnouncement("Healing upgrade", "parry");
}

void PerkManager::singleKill()
{
    // If already have perk, next shot will be boosted
    if (perks.at("single").getLevel() >= 1) singleKillReward = true;
    
    if (perks.at("single").increaseObjective()) addAnouncement("Single kill boost", "single");
}

void PerkManager::snipeKill(float distance)
{
    if (distance >= longDistance)
    {
        if (perks.at("sniper").increaseObjective()) addAnouncement("Increasing damage", "sniper");
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

std::string PerkManager::getAnnouncement()
{
    if (announcements.empty()) return "";

    std::string result = announcements.front();
    announcements.pop();
    return result;
}


void PerkManager::addAnouncement(std::string name, std::string key)
{
    audio.addSound(assets.perkSound);
    announcements.push(name + " " + (perks.at(key).getLevel() == 1 ? "" : std::to_string(perks.at(key).getLevel()) + " ") + "unlocked!");    
}