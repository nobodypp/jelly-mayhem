#pragma once

#include <filesystem>
#include <windows.h>
#include <stdexcept>
#include <string>
#include <fstream>
#include <iostream>

class Bootstrapper
{
    private:
        std::filesystem::path executableDirectory;

        std::filesystem::path readUpdateManifest();
        void installUpdate(const std::filesystem::path& updateRoot);
        void cleanupUpdate(const std::filesystem::path& udpateRoot);
        void launchGame();

    public:
        Bootstrapper();
        int runUpdater();
        int run();
};