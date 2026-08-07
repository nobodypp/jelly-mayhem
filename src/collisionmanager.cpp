#include "collisionmanager.hpp"


CollisionManager::CollisionManager(EnemyManager& enemies, ProjectileManager& projectiles, DamageTextManager& texts)
    : enemies(enemies), 
      projectiles(projectiles), 
      texts(texts)
{}

void CollisionManager::handleCollisions(Player& player)
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

    for (std::size_t i = 0; i < enemies.jelliesCount(); i++)
    {
        if (enemies.jellyAt(i).isColliding())
        {
            if (enemies.jellyAt(i).getBounds().findIntersection(player.getBounds()))
            {
                if (player.isHitting())
                {
                    enemies.jellyAt(i).registerKnockback(player.getBounds().position + player.getBounds().getCenter());
                    enemies.jellyAt(i).inflictDamage(player.getMeleeDamage());
                    texts.addText(player.getMeleeDamage(), true, enemies.jellyAt(i).getBounds(), "Blok! ");
                    if (enemies.jellyAt(i).isDead())
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