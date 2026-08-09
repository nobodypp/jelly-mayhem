#include "collisionmanager.hpp"


CollisionManager::CollisionManager(EnemyManager& enemies, ProjectileManager& projectiles, DamageTextManager& texts)
    : enemies(enemies), 
      projectiles(projectiles), 
      texts(texts)
{}

void CollisionManager::handleCollisions(Player& player)
{
    BottleCollisions();
    StarCollisions(player);
    meleeCollisions(player);
    enemiesAntiCrowd();
}

void CollisionManager::BottleCollisions()
{
    for (std::size_t i = 0; i < projectiles.bottleCount(); i++)
    {
        if (projectiles.bottleAt(i).isColliding())
        {
            for (std::size_t j = 0; j < enemies.jelliesCount(); j++)
            {
                if (projectiles.bottleAt(i).getBounds().findIntersection(enemies.jellyAt(j).getBounds()))
                {
                    projectiles.bottleAt(i).registerHit();
                    enemies.jellyAt(j).inflictDamage(projectiles.bottleAt(i).getDamage());
                    texts.addText(projectiles.bottleAt(i).getDamage(), true, enemies.jellyAt(i).getBounds());
                }
            }
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
                    bool killed = enemies.jellyAt(i).inflictDamage(player.getMeleeDamage());
                    texts.addText(player.getMeleeDamage(), true, enemies.jellyAt(i).getBounds(), "Blok! ");
                    if (killed)
                    {
                        player.succesfullParry();
                        texts.addText(20, true, player.getBounds(), "Kill blokiem! ");
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