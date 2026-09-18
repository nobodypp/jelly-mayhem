#include "scoremanager.hpp"

void ScoreManager::loadScores()
{
    if (!std::filesystem::exists(scoresPath))
    {
        saveScores();
        return;
    }
    std::ifstream file(scoresPath);
    if (!file) throw std::runtime_error("Failed to open score file");
    file >> highScores;
}

void ScoreManager::saveScores()
{
    std::ofstream file(scoresPath);
    if (!file) throw std::runtime_error("Failed to create score file");
    file << highScores.dump(4);
}

ScoreManager::ScoreManager()
{
    loadScores();
}

void ScoreManager::update(sf::Time deltaTime)
{
    gameTime += deltaTime;
    currentScores.at(ScoreKey::time).at("value") = static_cast<int>(gameTime.asSeconds());
    for (auto& [key, score]: highScores.items())
    {
        if (currentScores.at(key).at("value").get<int>() > score.at("value").get<int>())
        {
            highScores.at(key).at("value") = currentScores.at(key).at("value");
            saveScores();
        }
    }
}

void ScoreManager::reset()
{
    for (auto& [key, score]: currentScores.items()) score.at("value") = 0;
    gameTime = sf::Time::Zero;
}

std::string ScoreManager::getScoresList()
{
    std::string text = "";
    for (auto& [key, score]: highScores.items())
    {
        text += score.at("description").get<std::string>() + ": ";
        if (key == ScoreKey::time) text += getGameTime(false);
        else text += std::to_string(score.at("value").get<int>());
        text += "\n";
    }
    return text;
}

void ScoreManager::registerKill()
{
    auto& value = currentScores.at(ScoreKey::kills).at("value");
    value = value.get<int>() + 1;
}

void ScoreManager::updateCurrentScore(std::string_view scoreKey, int value) { currentScores.at(scoreKey).at("value") = value; }

void ScoreManager::registerDamage(int damage)
{
    if (damage > currentScores.at(ScoreKey::single_damage).at("value").get<int>()) currentScores.at(ScoreKey::single_damage).at("value") = damage;
    auto& value = currentScores.at(ScoreKey::damage_dealt).at("value");
    value = value.get<int>() + damage;
}

void ScoreManager::registerHeal(int healed)
{
    auto& value = currentScores.at(ScoreKey::health_healed).at("value");
    value = value.get<int>() + healed;
}

void ScoreManager::registerBottleThrown()
{
    auto& value = currentScores.at(ScoreKey::bottles_thrown).at("value");
    value = value.get<int>() + 1;
}

void ScoreManager::registerGroupHit(int groupSize) { if (groupSize > currentScores.at(ScoreKey::group_size).at("value").get<int>()) currentScores.at(ScoreKey::group_size).at("value") = groupSize; }

std::string ScoreManager::getGameTime(bool showCurrentScore)
{
    std::string seconds, minutes;
    if (showCurrentScore)
    {
        seconds = std::to_string(currentScores.at(ScoreKey::time).at("value").get<int>() % 60);
        minutes = std::to_string(currentScores.at(ScoreKey::time).at("value").get<int>() / 60);
    }
    else
    {
        seconds = std::to_string(highScores.at(ScoreKey::time).at("value").get<int>() % 60);
        minutes = std::to_string(highScores.at(ScoreKey::time).at("value").get<int>() / 60);
    }
    if (seconds.length() == 1) seconds = "0" + seconds;
    if (minutes.length() == 1) minutes = "0" + minutes;
    return minutes + ":" + seconds;
}

int ScoreManager::getCurrentScore(std::string_view scoreKey) { return currentScores.at(scoreKey).at("value").get<int>(); }
