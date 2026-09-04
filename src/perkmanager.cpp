#include "perkmanager.hpp"


PerkManager::PerkManager(AssetManager& assets, AudioManager& audio)
    : singleKillReward(false),
      assets(assets),
      audio(audio),
      perks{
          Perk{"Crowd control", "", "Increased bottle area", [](unsigned int level) { return 10; }},
          Perk{"Knockout crits", "Hit a knocked out enemy", "Increased damage against knocked out enemies", [](unsigned int level) { return (level + 1) * 5; }},
          Perk{"Dodger", "Dodge stars", "Faster movement speed", [](unsigned int level) { return 20 + level * 10; }},
          Perk{"Healing upgrade", "Do a block kill", "Increased healing"},
          Perk{"Single kill boost", "Kill with a bottle that only hit one enemy", "After a single kill the next bottle has increased damage"},
          Perk{"Increasing damage", "Kill from a distance", "Damage increases with distance"},
          Perk{"Faster reload", "Hit at close range", "Faster reload speed", [](unsigned int level) { return 15 + level * 10; }}
      }
{
    getPerk(PerkId::Group).updateObjective("Hit a group of " + std::to_string(4 + getPerk(PerkId::Group).getLevel() * 2) + " enemies with one bottle");
}

void PerkManager::registerGroupBottleHit(unsigned int enemiesNumber)
{
    if (enemiesNumber >= 4 + getPerk(PerkId::Group).getLevel() * 2) increasePerk(PerkId::Group);
}

void PerkManager::registerKnockbackHit() { increasePerk(PerkId::Knockback); }

void PerkManager::registerStarDodged() { increasePerk(PerkId::Dodger); }

void PerkManager::registerBlockKill() { increasePerk(PerkId::Parry); }

void PerkManager::registerSingleKill()
{
    // If already have perk, next shot will be boosted
    if (getPerk(PerkId::Single).getLevel() >= 1) singleKillReward = true;
    
    increasePerk(PerkId::Single);
}

void PerkManager::registerKill(float distance)
{
    if (distance >= longDistance) increasePerk(PerkId::Sniper);
}

void PerkManager::registerHit(float distance)
{
    if (distance <= closeDistance) increasePerk(PerkId::Close); 
}

float PerkManager::getBottleBoundsScale() { return 1.f + getPerk(PerkId::Group).getLevel() * 0.3f; }

float PerkManager::getKnockbackEnemyDamage() { return 1.f + getPerk(PerkId::Knockback).getLevel() * 2.0f; }

float PerkManager::getPlayerSpeedMultiplier() { return 1.f + getPerk(PerkId::Dodger).getLevel() * 0.3f; }

float PerkManager::getHealingMultiplier() { return 1.f + getPerk(PerkId::Parry).getLevel() * 0.25f; }

bool PerkManager::isNextBottleBoosted() { return singleKillReward; }

float PerkManager::claimBoostedBottle()
{
    float reward = singleKillReward ? (1.f + getPerk(PerkId::Single).getLevel() * 1.f) : 1.f;
    singleKillReward = false;
    return reward;
}

float PerkManager::getDamageRampup(float distance)
{
    return distance / longDistance * getPerk(PerkId::Sniper).getLevel() * 0.5f + 1.f;
}

float PerkManager::getReloadSpeedMultiplier()
{
    return 1.f + getPerk(PerkId::Close).getLevel() * 0.3f;
}

std::string PerkManager::getNextAnnouncement()
{
    if (announcements.empty()) return "";

    std::string result = announcements.front();
    announcements.pop();
    return result;
}

void PerkManager::reset() { for (auto& perk : perks) perk.reset(); }

std::tuple<std::string, std::string, std::string> PerkManager::getPerkInfo(std::size_t id)
{
    const auto& perk = perks.at(id % perks.size());

    return {
        perk.getName(),
        perk.getObjective(),
        perk.getReward()
    };
}

void PerkManager::increasePerk(PerkId id)
{
    if (getPerk(id).increaseObjective())
    {
        if (id == PerkId::Group) getPerk(id).updateObjective("Hit a group of " + std::to_string(4 + getPerk(PerkId::Group).getLevel() * 2) + " enemies with one bottle");

        audio.addSound(assets.perkSound);
        announcements.push(getPerk(id).getName() + " unlocked!");
    }
}