#pragma once

#include "version.hpp"
#include <string>
#include <curl/curl.h>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <cstdio>
#include <functional>
#include <filesystem>


class UpdateManager
{
    private:
        const char* apiUrl = "https://api.github.com/repos/nobodypp/jelly-mayhem/releases/latest";
        static size_t writeCallBack(char *data, size_t size, size_t numberOfElements, void *userData)
        {
            const size_t totalSize = size * numberOfElements;
            auto* response = static_cast<std::string*>(userData);
            response->append(data, totalSize);
            return totalSize;
        }
        static int progresCallBack(void* userData, curl_off_t totalDownload, curl_off_t downloaded)
        {
            if (totalDownload <= 0) return 0;
            const double progress = static_cast<double>(downloaded) / static_cast<double>(totalDownload);
            auto* callback = static_cast<std::function<void(double)>*>(userData);
            if (*callback) (*callback)(progress);
            return 0;
        }
        static bool isNewerVersion(const std::string& current, const std::string& latest);
        std::string downloadUrl = "";
        bool updateAvailable = false;
        std::filesystem::path downloadedFile;

    public:
        UpdateManager() = default;
        void checkForUpdate();
        void downloadUpdate();
        void installUpdate();
        bool getUpdateAvailable();
};