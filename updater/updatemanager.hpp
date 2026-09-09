#pragma once

#include "version.hpp"
#include <string>
#include <curl/curl.h>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <cstdio>
#include <functional>
#include <filesystem>
#include <cstdlib>
#include <iostream>
#include <miniz.h>
#include <windows.h>
#include <fstream>


class UpdateManager
{
    private:
        static size_t writeCallBack(char *data, size_t size, size_t numberOfElements, void *userData);
        static int progresCallBack(void* userData, curl_off_t totalDownload, curl_off_t downloaded);
        static bool isNewerVersion(const std::string& current, const std::string& latest);
        const char* apiUrl = "https://api.github.com/repos/nobodypp/jelly-mayhem/releases/latest";
        std::filesystem::path executableDirectory;
        std::string downloadUrl = "";
        std::string latestVersion = "";
        bool updateAvailable = false;
        std::filesystem::path downloadedFile = "";
        std::filesystem::path extractDirectory = "";
        std::filesystem::path extractedRoot = "";

    public:
        UpdateManager();
        bool checkForUpdate();
        void downloadUpdate();
        void extractUpdate();
        bool getUpdateAvailable();
        void writeUpdateManifest();
};