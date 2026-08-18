#include "../include/assetmanager.hpp"


AssetManager::AssetManager()
    : font("assets/fonts/CascadiaCode-VariableFont_wght.ttf")
{
    if (!playerCorpseDefaultTexture.loadFromFile("assets/textures/player/corpse.png")) std::cout << "Tekstura nie istnieje!";
    if (!playerLegsDefaultTexture.loadFromFile("assets/textures/player/legs-standing.png")) std::cout << "Tekstura nie istnieje!";
    if (!playerHandsDefaultTexture.loadFromFile("assets/textures/player/bottle-standing.png")) std::cout << "Tekstura nie istnieje!";
    if (!groundTexture.loadFromFile("assets/textures/ground.png")) std::cout << "Tekstura nie istnieje!";
    if (!bottleTexture.loadFromFile("assets/textures/bottle/bottle.png")) std::cout << "Tekstura nie istnieje!";
    if (!jellyDefault.loadFromFile("assets/textures/jelly/default.png")) std::cout << "Tekstura nie istnieje!";

    if (!bottleBreakSound.loadFromFile("assets/sounds/bottle/break.mp3")) std::cout << "Dźwięk nie istnieje!";
    if (!jellyBiteSound.loadFromFile("assets/sounds/jelly/bite.mp3")) std::cout << "Dźwięk nie istnieje!";
    if (!jellyDieSound.loadFromFile("assets/sounds/jelly/die.wav")) std::cout << "Dźwięk nie istnieje!";
    if (!jellyShootSound.loadFromFile("assets/sounds/jelly/shoot.ogg")) std::cout << "Dźwięk nie istnieje!";
    if (!jellyKnockbackSound.loadFromFile("assets/sounds/jelly/knockback.wav")) std::cout << "Dźwięk nie istnieje!";
    if (!playerDieSound.loadFromFile("assets/sounds/player/die.mp3")) std::cout << "Dźwięk nie istnieje!";
    if (!playerHitSound.loadFromFile("assets/sounds/player/hit.flac")) std::cout << "Dźwięk nie istnieje!";
    if (!starHitSound.loadFromFile("assets/sounds/star/hit.mp3")) std::cout << "Dźwięk nie istnieje!";

    groundTexture.setRepeated(true);

    loadFromLocation(legsRunning, "assets/textures/player/leg-run/leg-run-");
    loadFromLocation(bottleThrowing, "assets/textures/player/bottle-throw/throw-");
    loadFromLocation(bottleBreaking, "assets/textures/bottle/break/break-");
    loadFromLocation(jellyWalking, "assets/textures/jelly/walk/walk-");
    loadFromLocation(jellyDying, "assets/textures/jelly/die/die-");
    loadFromLocation(jellyShooting, "assets/textures/jelly/shoot/shoot-");
    loadFromLocation(starFly, "assets/textures/star/fly/projectile-");
    loadFromLocation(starExplode, "assets/textures/star/explode/projectile-");
    loadFromLocation(jellyBiting, "assets/textures/jelly/bite/bite-");
    loadFromLocation(jellyKnockback, "assets/textures/jelly/knockback/knockback-");
    loadFromLocation(bottleHit, "assets/textures/player/bottle-hit/hit-");
}

void AssetManager::loadFromLocation(std::vector<sf::Texture>& vector, std::string filePrefix)
{
    int frameIndex = 0;
    do 
    {
        sf::Texture texture;
        if (!texture.loadFromFile(filePrefix + std::to_string(frameIndex) + ".png")) std::cout << "Tekstura " << filePrefix + std::to_string(frameIndex) + ".png" << " nie istnieje!\n";
        vector.push_back(std::move(texture));
        frameIndex++;
    } while (std::filesystem::exists(filePrefix + std::to_string(frameIndex) + ".png"));
}