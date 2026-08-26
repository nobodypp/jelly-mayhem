#include "collisionmanager.hpp"


CollisionManager::CollisionManager(EnemyManager &enemies, ProjectileManager &projectiles, DamageTextManager &texts, PerkManager &perks)
    : enemies(enemies), 
      projectiles(projectiles), 
      texts(texts), 
      perks(perks)
{}

void CollisionManager::handleCollisions(Player &player)
{
    BottleCollisions(player);
    StarCollisions(player);
    meleeCollisions(player);
    enemiesAntiCrowd();
}

void CollisionManager::BottleCollisions(Player& player)
{
    for (std::size_t i = 0; i < projectiles.bottleCount(); i++)
    {
        auto& bottle = projectiles.bottleAt(i);
        
        if (bottle.isColliding())
        {
            int enemiesHit = 0;
            int enemiesKilled = 0;

            for (std::size_t j = 0; j < enemies.jelliesCount(); j++)
            {
                auto& jelly = enemies.jellyAt(j);

                if (!jelly.isDying() && jelly.isAlive() && bottle.getBounds().findIntersection(jelly.getBounds()))
                {
                    bottle.registerHit();

                    float distance = (bottle.getBounds().getCenter() - player.getBounds().getCenter()).length();
                    
                    // Calculate damage from base, perks etc.
                    float knockbackBonus = jelly.isDuringKnockback() ? perks.getKnockbackEnemyDamage() : 1.f;
                    float distanceBonus = perks.getDamageRampup(distance);
                    int damage = bottle.getDamage() * knockbackBonus * distanceBonus;

                    if (jelly.inflictDamage(damage))
                    {
                        // If jelly was killed
                        killCount++;
                        enemiesKilled++;

                        // Register perk objective
                        perks.snipeKill(distance);
                    }
                    
                    // Register perk objective
                    if (jelly.isDuringKnockback()) perks.knockbackEnemyHit();

                    // Add floating damage text
                    texts.addText(damage, true, jelly.getBounds(), knockbackBonus > 1.f ? "Knocked out! " : "");
                    
                    enemiesHit++;
                }
            }

            perks.bottleHitEnemyGroup(enemiesHit);
            if (enemiesHit == 1 && enemiesKilled == 1) perks.singleKill();
        }
    }
}

void CollisionManager::StarCollisions(Player& player)
{
    for (std::size_t i = 0; i < projectiles.starsCount(); i++)
    {
        auto& star = projectiles.starAt(i);

        if (star.isColliding())
        {
            if (star.getBounds().findIntersection(player.getBounds()))
            {
                star.registerHit();
                player.inflictDamage(star.getDamage());
                texts.addText(star.getDamage(), false, player.getBounds());
            }
            // Check if star was dodged by player
            else if (distanceBetweenTwoRects(player.getBounds(), star.getBounds()) <= dodgeDistance) star.registerProximityToPlayer();
        }
    }
}

void CollisionManager::meleeCollisions(Player& player)
{
    for (std::size_t i = 0; i < enemies.jelliesCount(); i++)
    {
        auto& jelly = enemies.jellyAt(i);

        if (jelly.isColliding())
        {
            if (jelly.getBounds().findIntersection(player.getBounds()))
            {
                if (player.isHitting())
                {
                    jelly.registerKnockback(player.getBounds().position + player.getBounds().getCenter());
                    texts.addText(player.getMeleeDamage(), true, jelly.getBounds(), "Block! ");
                    if (jelly.inflictDamage(player.getMeleeDamage()))
                    {
                        // If killed
                        killCount++;

                        int healed = player.succesfullParry();
                        texts.addText(-healed, true, player.getBounds(), "Block kill! ");

                        perks.parryKill();
                    }
                }
                else
                {
                    jelly.registerHit();
                    player.inflictDamage(jelly.getDamage());
                    texts.addText(jelly.getDamage(), false, player.getBounds());
                }
            }
        }
    }
}

void CollisionManager::enemiesAntiCrowd()
{
    const std::size_t count = enemies.jelliesCount();

    for (std::size_t i = 0; i < count; ++i)
    {
        for (std::size_t j = i + 1; j < count; ++j)
        {
            auto& jelly1 = enemies.jellyAt(i);
            auto& jelly2 = enemies.jellyAt(j);

            sf::Vector2f distance =
                jelly1.getBounds().getCenter() -
                jelly2.getBounds().getCenter();

            float length = distance.length();

            if (length < minEnemiesDistance)
            {
                if (length == 0.f)
                    distance = {1.f, 0.f};
                else
                    distance /= length;

                float push = (minEnemiesDistance - length) / 2.f;

                jelly1.move(distance * push);
                jelly2.move(-distance * push);
            }
        }
    }
}

float CollisionManager::distanceBetweenTwoRects(const sf::FloatRect a, const sf::FloatRect b)
{
    float dx = std::max({
        a.position.x - (b.position.x + b.size.x),
        b.position.x - (a.position.x + a.size.x),
        0.f
    });

    float dy = std::max({
        a.position.y - (b.position.y + b.size.y),
        b.position.y - (a.position.y + a.size.y),
        0.f
    });

    return std::sqrt(dx * dx + dy * dy);
}

int CollisionManager::getKillCount() { return killCount; }