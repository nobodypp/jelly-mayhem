#include "perkmanager.hpp"


PerkManager::PerkManager(AssetManager& assets)
    : singleKillReward(false), 
      assets(assets), 
      perkSound(assets.perkSound)
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
        if (perks.at("group").increaseObjective())
        {
            perkSound.play();
            std::cout << "Rozmiar butelki na poziomie " << perks.at("group").getLevel() << "\n";
        }
    }
}

void PerkManager::knockbackEnemyGotKilled()
{
    if (perks.at("knockback").increaseObjective())
    {
        perkSound.play();
        std::cout << "Kryty ogłuszone na poziomie  " << perks.at("knockback").getLevel() << "\n";
    }
}

void PerkManager::starDodged()
{
    if (perks.at("dodger").increaseObjective())
    {
        perkSound.play();
        std::cout << "Prędkość na poziomie " << perks.at("dodger").getLevel() << "\n";
    }
}

void PerkManager::parryKill()
{
    if (perks.at("parry").increaseObjective())
    {
        perkSound.play();
        std::cout << "Leczenie na poziomie " << perks.at("parry").getLevel() << "\n";
    }
}

void PerkManager::singleKill()
{
    if (perks.at("single").getLevel() >= 1)
    {
        singleKillReward = true;
    }
    if (perks.at("single").increaseObjective())
    {
        perkSound.play();
        std::cout << "Nagroda za single kill na poziomie " << perks.at("single").getLevel() << "\n";
    }
}

void PerkManager::snipeKill(float distance)
{
    if (distance >= longDistance)
    {
        if (perks.at("sniper").increaseObjective())
        {
            perkSound.play();
            std::cout << "Damage rampup na poziomie " << perks.at("sniper").getLevel() << "\n";
        }
    }
}

float PerkManager::getBottleBoundsScale() { return 1.f + perks.at("group").getLevel() * 0.3f; }

float PerkManager::getKnockbackEnemyDamage() { return 1.f + perks.at("knockback").getLevel() * 2.0f; }

float PerkManager::getPlayerSpeedMultiplier() { return 1.f + perks.at("dodger").getLevel() * 0.3f; }

float PerkManager::getHealingMultiplier() { return 1.f + perks.at("parry").getLevel() * 0.25f; }

bool PerkManager::isNextBottleBoosted() { return singleKillReward; }

float PerkManager::claimBoostedBottle()
{
    float reward = singleKillReward ? (1.f + perks.at("single").getLevel() * 1.f) : 1.f;
    singleKillReward = false;
    return reward;
}

float PerkManager::getDamageRampup(float distance) { return (distance >= longDistance ? perks.at("sniper").getLevel() * 1.f : 0.f) + 1.f; }
