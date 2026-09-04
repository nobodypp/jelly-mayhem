#include "assetmanager.hpp"



AssetManager::AssetManager()
    : font(assetPath("assets/fonts/CascadiaCode-VariableFont_wght.ttf"))
{
    if (!playerCorpseDefaultTexture.loadFromFile(assetPath("assets/textures/player/corpse.png"))) std::cout << "Tekstura nie istnieje!";
    if (!playerLegsDefaultTexture.loadFromFile(assetPath("assets/textures/player/legs-standing.png"))) std::cout << "Tekstura nie istnieje!";
    if (!playerHandsDefaultTexture.loadFromFile(assetPath("assets/textures/player/bottle-standing.png"))) std::cout << "Tekstura nie istnieje!";
    if (!groundTexture.loadFromFile(assetPath("assets/textures/ground.png"))) std::cout << "Tekstura nie istnieje!";
    if (!bottleTexture.loadFromFile(assetPath("assets/textures/bottle/bottle.png"))) std::cout << "Tekstura nie istnieje!";
    if (!jellyDefault.loadFromFile(assetPath("assets/textures/jelly/default.png"))) std::cout << "Tekstura nie istnieje!";

    if (!bottleBreakSound.loadFromFile(assetPath("assets/sounds/bottle/break.mp3"))) std::cout << "Dźwięk nie istnieje!";
    if (!jellyBiteSound.loadFromFile(assetPath("assets/sounds/jelly/bite.mp3"))) std::cout << "Dźwięk nie istnieje!";
    if (!jellyDieSound.loadFromFile(assetPath("assets/sounds/jelly/die.mp3"))) std::cout << "Dźwięk nie istnieje!";
    if (!jellyShootSound.loadFromFile(assetPath("assets/sounds/jelly/shoot.mp3"))) std::cout << "Dźwięk nie istnieje!";
    if (!jellyKnockbackSound.loadFromFile(assetPath("assets/sounds/jelly/knockback.wav"))) std::cout << "Dźwięk nie istnieje!";
    if (!playerDieSound.loadFromFile(assetPath("assets/sounds/player/die.mp3"))) std::cout << "Dźwięk nie istnieje!";
    if (!playerHitSound.loadFromFile(assetPath("assets/sounds/player/hit.flac"))) std::cout << "Dźwięk nie istnieje!";
    if (!starHitSound.loadFromFile(assetPath("assets/sounds/star/hit.mp3"))) std::cout << "Dźwięk nie istnieje!";
    if (!perkSound.loadFromFile(assetPath("assets/sounds/ui/perk.mp3"))) std::cout << "Dźwięk nie istnieje!";
    if (!chargingHitSound.loadFromFile(assetPath("assets/sounds/player/charging.wav"))) std::cout << "Dźwięk nie istnieje!";
    if (!buttonSound.loadFromFile(assetPath("assets/sounds/ui/button.wav"))) std::cout << "Dźwięk nie istnieje!";

    groundTexture.setRepeated(true);

    loadFramesFromLocation(legsRunningFrames, "assets/textures/player/leg-run/leg-run-");
    loadFramesFromLocation(bottleThrowingFrames, "assets/textures/player/bottle-throw/throw-");
    loadFramesFromLocation(bottleBreakingFrames, "assets/textures/bottle/break/break-");
    loadFramesFromLocation(jellyWalkingFrames, "assets/textures/jelly/walk/walk-");
    loadFramesFromLocation(jellyDyingFrames, "assets/textures/jelly/die/die-");
    loadFramesFromLocation(jellyShootingFrames, "assets/textures/jelly/shoot/shoot-");
    loadFramesFromLocation(starFlyFrames, "assets/textures/star/fly/projectile-");
    loadFramesFromLocation(starExplodeFrames, "assets/textures/star/explode/projectile-");
    loadFramesFromLocation(jellyBitingFrames, "assets/textures/jelly/bite/bite-");
    loadFramesFromLocation(jellyKnockbackFrames, "assets/textures/jelly/knockback/knockback-");
    loadFramesFromLocation(bottleHitFrames, "assets/textures/player/bottle-hit/hit-");
    loadFramesFromLocation(bottleBarChargedFrames, "assets/textures/ui/charging-bar/bar-");
}

void AssetManager::loadFramesFromLocation(std::vector<sf::Texture>& vector, std::string filePrefix)
{
    int frameIndex = 0;
    do 
    {
        sf::Texture texture;
        const auto filename = filePrefix + std::to_string(frameIndex) + ".png";
        if (!texture.loadFromFile(assetPath(filename))) std::cout << "Tekstura " << filename << " nie istnieje!\n";
        vector.push_back(std::move(texture));
        frameIndex++;
    } while (std::filesystem::exists(filePrefix + std::to_string(frameIndex) + ".png"));
}


std::filesystem::path AssetManager::getExecutableDirectory()
{
#ifdef _WIN32

    std::array<wchar_t, 32768> buffer{};

    DWORD length = GetModuleFileNameW(
        nullptr,
        buffer.data(),
        static_cast<DWORD>(buffer.size())
    );

    if (length == 0)
        return {};

    return std::filesystem::path(buffer.data(), buffer.data() + length)
        .parent_path();

#elif defined(__linux__)

    return std::filesystem::canonical("/proc/self/exe").parent_path();

#else

    return std::filesystem::current_path();

#endif
}

std::filesystem::path AssetManager::findAssetsDirectory()
{
    const auto executableDirectory = getExecutableDirectory();

    // Development / Windows installation:
    // assets obok executable
    const auto localAssets = executableDirectory / "assets";

    if (std::filesystem::is_directory(localAssets))
        return localAssets;

#ifdef __linux__

    // Linux installation:
    // /usr/share/jelly-mayhem/assets
    const auto installedAssets =
        std::filesystem::path("/usr/share/jelly-mayhem/assets");

    if (std::filesystem::is_directory(installedAssets))
        return installedAssets;

#endif

    // Ostateczny fallback — obecne zachowanie
    return std::filesystem::path("assets");
}

std::filesystem::path AssetManager::assetPath(const std::filesystem::path& relativePath)
{
    static const auto assetsDirectory = findAssetsDirectory();
    const std::string path = relativePath.string();

    if (path.rfind("assets/", 0) == 0)
    {
        return assetsDirectory / path.substr(7);
    }

    return assetsDirectory / relativePath;
}