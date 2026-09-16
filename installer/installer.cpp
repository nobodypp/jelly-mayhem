#include "installer.hpp"

std::filesystem::path Installer::readUpdateManifest()
{
    const auto manifestPath = std::filesystem::temp_directory_path() / "jelly-mayhem-update" / "update.info";

    std::ifstream file(manifestPath);

    if (!file) throw std::runtime_error("Failed to open update manifest: " + manifestPath.string());
    
    std::string root;

    if (!std::getline(file, root)) throw std::runtime_error("Failed to read update manifest");

    if (root.empty()) throw std::runtime_error("Update manifest contains an empty path");

    const std::filesystem::path updateRoot(root);

    if (!std::filesystem::exists(updateRoot)) throw std::runtime_error("Update root does not exist: " + updateRoot.string());

    if (!std::filesystem::is_directory(updateRoot)) throw std::runtime_error("Update root is not a directory: " + updateRoot.string());

    return updateRoot;
}

void Installer::installUpdate(const std::filesystem::path &updateRoot)
{
    std::cout << "Installing update from:\n" << updateRoot << '\n';

    for (const auto& entry : std::filesystem::recursive_directory_iterator(updateRoot))
    {
        const auto filename = entry.path().filename();

        if (filename == "jelly-mayhem-installer.exe" || filename == "jelly-mayhem-launcher.exe") continue;

        const auto relativePath = std::filesystem::relative(entry.path(), updateRoot);

        const auto destination = executableDirectory / relativePath;

        if (entry.is_directory())
        {
            std::filesystem::create_directories(destination);
            continue;
        }

        if (!entry.is_regular_file()) continue;

        std::filesystem::create_directories(destination.parent_path());

        const BOOL result = CopyFileW(entry.path().c_str(), destination.c_str(), FALSE);

        if (!result)
        {
            const DWORD error = GetLastError();
            throw std::runtime_error("Failed to copy file: " + entry.path().string() + " -> " + destination.string() + " (Win32 error: " + std::to_string(error) + ")");
        }

        std::cout << "Installed: " << relativePath << '\n';
    }
}

void Installer::cleanupUpdate(const std::filesystem::path &updateRoot)
{
    const auto stagingDirectory = updateRoot.parent_path();

    std::error_code ec;
    std::filesystem::remove_all(stagingDirectory, ec);

    if (ec) std::cerr << "Warning: failed to remove update staging directory: " << stagingDirectory << " (" << ec.message() << ")\n";
}

Installer::Installer()
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