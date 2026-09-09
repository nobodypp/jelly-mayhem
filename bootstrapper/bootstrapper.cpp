#include "bootstrapper.hpp"

std::filesystem::path Bootstrapper::readUpdateManifest()
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

void Bootstrapper::installUpdate(const std::filesystem::path &updateRoot)
{
    std::cout << "Installing update from:\n" << updateRoot << '\n';

    for (const auto& entry : std::filesystem::recursive_directory_iterator(updateRoot))
    {
        const auto filename = entry.path().filename();

        if (filename == "jelly-mayhem-bootstrapper.exe")
        {
            continue;
        }

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

void Bootstrapper::cleanupUpdate(const std::filesystem::path &updateRoot)
{
    const auto stagingDirectory = updateRoot.parent_path();

    std::error_code ec;
    std::filesystem::remove_all(stagingDirectory, ec);

    if (ec) std::cerr << "Warning: failed to remove update staging directory: " << stagingDirectory << " (" << ec.message() << ")\n";
}

void Bootstrapper::launchGame()
{
    const auto gamePath = executableDirectory / "jelly-mayhem.exe";

    const HINSTANCE result = ShellExecuteW(
        nullptr,
        L"open",
        gamePath.c_str(),
        nullptr,
        executableDirectory.c_str(),
        SW_SHOWNORMAL
    );

    if (reinterpret_cast<std::intptr_t>(result) <= 32) throw std::runtime_error("Failed to start jelly-mayhem.exe");
}

Bootstrapper::Bootstrapper()
{
    wchar_t buffer[MAX_PATH];

    const DWORD length = GetModuleFileNameW(
        nullptr,
        buffer,
        MAX_PATH
    );

    if (length == 0) throw std::runtime_error("Failed to get bootstrapper executable path");

    executableDirectory = std::filesystem::path(buffer).parent_path();
}

int Bootstrapper::runUpdater()
{
    const auto updaterPath = executableDirectory / "jelly-mayhem-updater.exe";

    if (!std::filesystem::exists(updaterPath)) throw std::runtime_error("Updater executable does not exist: " + updaterPath.string());

    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);

    PROCESS_INFORMATION processInfo{};

    std::wstring commandLine = L"\"" + updaterPath.wstring() + L"\"";

    const BOOL result = CreateProcessW(
        nullptr,
        commandLine.data(),
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        executableDirectory.c_str(),
        &startupInfo,
        &processInfo
    );

    if (!result) throw std::runtime_error("Failed to start updater");
    

    const DWORD waitResult = WaitForSingleObject(
        processInfo.hProcess,
        INFINITE
    );

    if (waitResult != WAIT_OBJECT_0)
    {
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);

        throw std::runtime_error("Failed while waiting for updater");
    }

    DWORD exitCode = 0;

    if (!GetExitCodeProcess(processInfo.hProcess, &exitCode))
    {
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);

        throw std::runtime_error("Failed to get updater exit code");
    }

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);

    return static_cast<int>(exitCode);
}

int Bootstrapper::run()
{
    const int updaterExitCode = runUpdater();

    std::cout << "Updater exited with code: " << updaterExitCode << '\n';

    if (updaterExitCode != 0)
    {
        std::cerr << "Updater failed. Update will not be installed." << '\n';
        return updaterExitCode;
    }

    const auto manifestPath = std::filesystem::temp_directory_path() / "jelly-mayhem-update" / "update.info"; 

    if (!std::filesystem::exists(manifestPath))
    {
        std::cout << "No update manifest found. " "Starting game." << '\n';
        launchGame();
        return 0;
    }

    const auto updateRoot = readUpdateManifest();
    installUpdate(updateRoot);
    cleanupUpdate(updateRoot);
    launchGame();
    return 0;
}
