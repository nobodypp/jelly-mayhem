#pragma once

#include "drawable.hpp"
#include "entity.hpp"
#include <memory>
#include <concepts>

class Manager : public Drawable
{
    protected:
        template <typename T>
        requires std::derived_from<T, Entity>
        void updateVector(std::vector<std::unique_ptr<T>>& entities, sf::Time deltaTime)
        {
            for (std::size_t i = 0; i < entities.size();)
            {
                if (entities[i]->isAlive())
                {
                    entities[i]->update(deltaTime);
                    i++;
                }
                else
                {
                    entities.erase(entities.begin() + i);
                }
            } 
        }
        
        template <typename T>
        requires std::derived_from<T, Entity>
        void renderVector(std::vector<std::unique_ptr<T>>& entities, sf::RenderWindow& window)
        {
            for (auto& entity : entities)
            {
                entity->render(window);
            }
        }
};