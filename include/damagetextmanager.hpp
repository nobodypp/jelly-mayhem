#pragma once

#include "damagetext.hpp"
#include "drawable.hpp"
#include "assetmanager.hpp"
#include <vector>


class DamageTextManager : public Drawable
{
    private:
        std::vector<DamageText> texts;
        AssetManager& assets;
    
    public:
        DamageTextManager(AssetManager& assets);
        template<typename... Args>
        void addText(Args&&... args) { texts.emplace_back(assets, std::forward<Args>(args)...); }
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow& window) override;
        void reset();
};