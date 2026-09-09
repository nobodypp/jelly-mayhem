#include "updatemanager.hpp"

UpdateManager::UpdateManager()
{
    // Find executable directory
    wchar_t buffer[MAX_PATH];

    const DWORD length = GetModuleFileNameW(
        nullptr,
        buffer,
        MAX_PATH
    );

    if (length == 0)
        throw std::runtime_error("Failed to get executable path");

    executableDirectory = std::filesystem::path(buffer).parent_path();
}

bool UpdateManager::checkForUpdate()
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

    latestVersion = json.at("tag_name").get<std::string>();
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

        if (name.ends_with("-win64.zip"))
        {
            downloadUrl = asset.at("browser_download_url").get<std::string>();
            break;
        }

    }
    std::cout << downloadUrl << "\n";

    return updateAvailable;
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

    downloadedFile = std::filesystem::temp_directory_path() / "jelly-mayhem-update.zip";

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


void UpdateManager::extractUpdate()
{
    if (downloadedFile.empty()) throw std::runtime_error("No update file downloaded");

    if (!std::filesystem::exists(downloadedFile)) throw std::runtime_error("Downloaded update file does not exist");

    extractDirectory = std::filesystem::temp_directory_path() / "jelly-mayhem-update";

    std::filesystem::remove_all(extractDirectory);
    std::filesystem::create_directories(extractDirectory);

    std::cout << "Extracting update...\n";
    std::cout << "Downloaded file: " << downloadedFile << '\n';
    std::cout << "Extract directory: " << extractDirectory << '\n';

    mz_zip_archive zip{};
    
    if (!mz_zip_reader_init_file(&zip, downloadedFile.string().c_str(), 0))
    {
        throw std::runtime_error("Failed to open ZIP archive: " + downloadedFile.string());
    }

    const mz_uint fileCount = mz_zip_reader_get_num_files(&zip);

    std::cout << "Files in archive: " << fileCount << '\n';

    for (mz_uint i = 0; i < fileCount; ++i)
    {
        mz_zip_archive_file_stat fileStat{};

        if (!mz_zip_reader_file_stat(&zip, i, &fileStat))
        {
            mz_zip_reader_end(&zip);

            throw std::runtime_error("Failed to read ZIP entry information");
        }

        const std::filesystem::path relativePath = fileStat.m_filename;

        const std::filesystem::path outputPath = extractDirectory / relativePath;

        std::cout << "  Extracting: " << relativePath << '\n';

        if (mz_zip_reader_is_file_a_directory(&zip, i))
        {
            std::filesystem::create_directories(outputPath);
            continue;
        }

        std::filesystem::create_directories(outputPath.parent_path());

        if (!mz_zip_reader_extract_to_file(&zip, i, outputPath.string().c_str(), 0))
        {
            mz_zip_reader_end(&zip);

            throw std::runtime_error("Failed to extract file: " + relativePath.string());
        }
    }

    mz_zip_reader_end(&zip);

    extractedRoot = extractDirectory / ("Jelly-Mayhem-" + latestVersion + "-win64");

    if (!std::filesystem::exists(extractedRoot))
    {
        throw std::runtime_error("Extracted update directory does not exist: " + extractedRoot.string());
    }

    std::cout << "Update extracted to:\n";
    std::cout << extractedRoot << '\n';
}

bool UpdateManager::getUpdateAvailable() { return updateAvailable; }

void UpdateManager::writeUpdateManifest()
{
    if (extractedRoot.empty()) throw std::runtime_error("Cannot write update manifest: extracted root is empty");
    
    if (!std::filesystem::exists(extractedRoot)) throw std::runtime_error("Cannot write update manifest: extracted root does not exist");
    
    const auto manifestPath = extractDirectory / "update.info";

    std::ofstream file(manifestPath);

    if (!file) throw std::runtime_error("Failed to create update manifest: " + manifestPath.string());

    file << extractedRoot.string() << '\n';

    if (!file) throw std::runtime_error("Failed to write update manifest: " + manifestPath.string());
    
    std::cout << "Update manifest written to:\n" << manifestPath << '\n';
}

size_t UpdateManager::writeCallBack(char *data, size_t size, size_t numberOfElements, void *userData)
{
    const size_t totalSize = size * numberOfElements;
    auto* response = static_cast<std::string*>(userData);
    response->append(data, totalSize);
    return totalSize;
}

int UpdateManager::progresCallBack(void *userData, curl_off_t totalDownload, curl_off_t downloaded)
{
    if (totalDownload <= 0) return 0;
    const double progress = static_cast<double>(downloaded) / static_cast<double>(totalDownload);
    auto* callback = static_cast<std::function<void(double)>*>(userData);
    if (*callback) (*callback)(progress);
    return 0;
}

bool UpdateManager::isNewerVersion(const std::string &current, const std::string &latest)
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
