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
        if (projectiles.bottleAt(i).isColliding())
        {
            int enemiesHit = 0;
            int enemiesKilled = 0;
            for (std::size_t j = 0; j < enemies.jelliesCount(); j++)
            {
                if (projectiles.bottleAt(i).getBounds().findIntersection(enemies.jellyAt(j).getBounds()))
                {
                    projectiles.bottleAt(i).registerHit();

                    float distance = (projectiles.bottleAt(i).getBounds().getCenter() - player.getBounds().getCenter()).length();
                    
                    // Calculate damage from base, perks etc.
                    int damage = projectiles.bottleAt(i).getDamage() * (enemies.jellyAt(j).isDuringKnockback() ? perks.getKnockbackEnemyDamage() : 1.f) * perks.getDamageRampup(distance);

                    if (enemies.jellyAt(j).inflictDamage(damage))
                    {
                        // If jelly was killed
                        killCount++;
                        enemiesKilled++;

                        if (enemies.jellyAt(j).isDuringKnockback()) perks.knockbackEnemyGotKilled();
                        perks.snipeKill(distance);
                    }

                    texts.addText(damage, true, enemies.jellyAt(j).getBounds());

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
        if (projectiles.starAt(i).isColliding())
        {
            if (projectiles.starAt(i).getBounds().findIntersection(player.getBounds()))
            {
                projectiles.starAt(i).registerHit();
                player.inflictDamage(projectiles.starAt(i).getDamage());
                texts.addText(projectiles.starAt(i).getDamage(), false, player.getBounds());
            }
            // Check if star was dodged by player
            else if (distanceBetweenTwoRects(player.getBounds(), projectiles.starAt(i).getBounds()) <= dodgeDistance) projectiles.starAt(i).registerProximityToPlayer();
        }
    }
}

void CollisionManager::meleeCollisions(Player& player)
{
    for (std::size_t i = 0; i < enemies.jelliesCount(); i++)
    {
        if (enemies.jellyAt(i).isColliding())
        {
            if (enemies.jellyAt(i).getBounds().findIntersection(player.getBounds()))
            {
                if (player.isHitting())
                {
                    enemies.jellyAt(i).registerKnockback(player.getBounds().position + player.getBounds().getCenter());
                    texts.addText(player.getMeleeDamage(), true, enemies.jellyAt(i).getBounds(), "Blok! ");
                    if (enemies.jellyAt(i).inflictDamage(player.getMeleeDamage()))
                    {
                        // If killed
                        killCount++;

                        int healed = player.succesfullParry();
                        texts.addText(-healed, true, player.getBounds(), "Kill blokiem! ");

                        perks.parryKill();
                    }
                }
                else
                {
                    enemies.jellyAt(i).registerHit();
                    player.inflictDamage(enemies.jellyAt(i).getDamage());
                    texts.addText(enemies.jellyAt(i).getDamage(), false, player.getBounds());
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