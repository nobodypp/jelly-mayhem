#include "updatemanager.hpp"
#include <iostream>

void UpdateManager::checkForUpdate()
{
    CURL* curl = curl_easy_init();

    if (!curl)
        throw std::runtime_error("Failed to initialize CURL");

    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, apiUrl );

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallBack );

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Jelly-Mayhem");

    const CURLcode result = curl_easy_perform(curl);

    const auto json = nlohmann::json::parse(response);

    if (result != CURLE_OK)
    {
        std::cerr << "CURL error: " << curl_easy_strerror(result) << '\n';
        curl_easy_cleanup(curl);
        throw std::runtime_error(curl_easy_strerror(result));
    }

    std::string latestVersion = json.at("tag_name").get<std::string>();
    if (!latestVersion.empty() && latestVersion[0] == 'v') latestVersion.erase(0, 1);
    updateAvailable = isNewerVersion(JELLY_MAYHEM_VERSION, latestVersion);
    if (latestVersion != JELLY_MAYHEM_VERSION)
    {
        std::cout << "Update available!\n";
        std::cout << "Current: " << JELLY_MAYHEM_VERSION << '\n';
        std::cout << "Latest:  " << latestVersion << '\n';
    }
    else
    {
        std::cout << "Game is up to date.\n";
    }

    curl_easy_cleanup(curl);

    for (const auto& asset : json.at("assets"))
    {
        const std::string name = asset.at("name").get<std::string>();

    #ifdef _WIN32

        if (name.ends_with("-win64.exe"))
        {
            downloadUrl = asset.at("browser_download_url").get<std::string>();
            break;
        }

    #elif defined(__linux__)

        if (name.ends_with("-Linux.deb"))
        {
            downloadUrl = asset.at("browser_download_url").get<std::string>();
            break;
        }

    #endif
    }
    std::cout << downloadUrl << "\n";
}

void UpdateManager::downloadUpdate()
{
    if (!updateAvailable)
    {
        std::cout << "No update available.\n";
        return;
    }

    CURL* curl = curl_easy_init();

    if (!curl) throw std::runtime_error("Failed to initialize CURL");

#ifdef _WIN32

    downloadedFile = std::filesystem::temp_directory_path() / "jelly-mayhem-update.exe";

#elif defined(__linux__)

    downloadedFile = std::filesystem::temp_directory_path() / "jelly-mayhem-update.deb";

#endif

    FILE* file = std::fopen(downloadedFile.string().c_str(), "wb");
    
    if (!file)
    {
        curl_easy_cleanup(curl);
        throw std::runtime_error("Failed to open output file");
    }

    curl_easy_setopt(curl, CURLOPT_URL, downloadUrl.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, nullptr);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Jelly-Mayhem");

    std::function<void(double)> progressCallback = [](double progress){std::cout << "Download: " << progress * 100.0 << "%\n";};

    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, UpdateManager::progresCallBack);

    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &progressCallback);

    const CURLcode result = curl_easy_perform(curl);

    std::fclose(file);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK) throw std::runtime_error(curl_easy_strerror(result));
}

void UpdateManager::installUpdate()
{
    if (downloadedFile.empty()) throw std::runtime_error("No update file downloaded");
    std::cout << "Ready to install: " << downloadedFile << '\n';
}

bool UpdateManager::getUpdateAvailable() { return updateAvailable; }

bool UpdateManager::isNewerVersion(const std::string& current, const std::string& latest)
{
    std::stringstream currentStream(current);
    std::stringstream latestStream(latest);

    int currentMajor, currentMinor, currentPatch, latestMajor, latestMinor, latestPatch;
    char separator;

    currentStream >> currentMajor >> separator  >> currentMinor >> separator >> currentPatch;

    latestStream >> latestMajor  >> separator >> latestMinor >> separator >> latestPatch;

    if (latestMajor != currentMajor) return latestMajor > currentMajor;

    if (latestMinor != currentMinor) return latestMinor > currentMinor;

    return latestPatch > currentPatch;
}
