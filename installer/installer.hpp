#pragma once

#include <string>
#include <stdexcept>
#include <cstdio>
#include <functional>
#include <filesystem>
#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <fstream>


class Installer
{
    private:
        std::filesystem::path executableDirectory;

    public:
        Installer();
        std::filesystem::path readUpdateManifest();
        void installUpdate(const std::filesystem::path& updateRoot);
        void cleanupUpdate(const std::filesystem::path& udpateRoot);
};