#pragma once

#include <cstdlib>
#include <filesystem>
#include <stdexcept>

#ifdef _WIN32
    #include <windows.h>
    #include <shlobj.h>
#endif

namespace Paths
{
    inline std::filesystem::path dataDirectory()
    {
#ifdef _WIN32

        PWSTR path = nullptr;
        const HRESULT result = SHGetKnownFolderPath(
            FOLDERID_LocalAppData,
            KF_FLAG_DEFAULT,
            nullptr,
            &path
        );
        if (FAILED(result)) throw std::runtime_error("Failed to get LocalAppData directory");
        std::filesystem::path dataPath = path;
        CoTaskMemFree(path);
        dataPath /= "Jelly-Mayhem";

#elif defined(__linux__)

        const char* xdgDataHome = std::getenv("XDG_DATA_HOME");
        std::filesystem::path dataPath;
        if (xdgDataHome && *xdgDataHome != '\0') dataPath = xdgDataHome;
        else
        {
            const char* home = std::getenv("HOME");
            if (!home || *home == '\0') throw std::runtime_error("HOME is not set");
            dataPath = home;
            dataPath /= ".local";
            dataPath /= "share";
        }
        dataPath /= "Jelly-Mayhem";

#else
        #error "Unsupported operating system"
#endif

        std::filesystem::create_directories(dataPath);
        return dataPath;
    }

    inline std::filesystem::path saveDirectory()
    {
        const auto path = dataDirectory() / "saves";
        std::filesystem::create_directories(path);
        return path;
    }

    inline std::filesystem::path configDirectory()
    {
#ifdef _WIN32

        // Na początek konfiguracja razem z danymi aplikacji.
        const auto path = dataDirectory() / "config";

#elif defined(__linux__)

        const char* xdgConfigHome = std::getenv("XDG_CONFIG_HOME");
        std::filesystem::path path;
        if (xdgConfigHome && *xdgConfigHome != '\0') path = xdgConfigHome;
        else
        {
            const char* home = std::getenv("HOME");
            if (!home || *home == '\0') throw std::runtime_error("HOME is not set");
            path = home;
            path /= ".config";
        }
        path /= "Jelly-Mayhem";

#else
        #error "Unsupported operating system"
#endif

        std::filesystem::create_directories(path);
        return path;
    }

    inline std::filesystem::path logDirectory()
    {
        const auto path = dataDirectory() / "logs";
        std::filesystem::create_directories(path);
        return path;
    }

    inline std::filesystem::path cacheDirectory()
    {
#ifdef _WIN32
        const auto path = dataDirectory() / "cache";
#elif defined(__linux__)

        const char* xdgCacheHome = std::getenv("XDG_CACHE_HOME");
        std::filesystem::path path;
        if (xdgCacheHome && *xdgCacheHome != '\0') path = xdgCacheHome;
        else
        {
            const char* home = std::getenv("HOME");
            if (!home || *home == '\0') throw std::runtime_error("HOME is not set");
            path = home;
            path /= ".cache";
        }
        path /= "Jelly-Mayhem";

#else

        #error "Unsupported operating system"

#endif

        std::filesystem::create_directories(path);
        return path;
    }
}