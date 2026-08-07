#pragma once

#include "damagetext.hpp"
#include "drawable.hpp"
#include "texturemanager.hpp"
#include <vector>


class DamageTextManager : public Drawable
{
    private:
        std::vector<DamageText> texts;
        TextureManager& textures;
    
    public:
        DamageTextManager(TextureManager& textures);
        template<typename... Args>
        void addText(Args&&... args) { texts.emplace_back(textures, std::forward<Args>(args)...); }
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
};