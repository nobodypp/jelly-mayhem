#include "launcher.hpp"

#include <iostream>
#include <exception>
#ifdef _WIN32
    #include <windows.h>
    #include <cstdio>
#endif


int main()
{
#ifdef _WIN32
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
        FILE* stream;
        freopen_s(&stream, "CONOUT$", "w", stdout);
        freopen_s(&stream, "CONOUT$", "w", stderr);
    }
#endif
    try
    {
        Launcher launcher;
        if (launcher.checkForUpdate())
        {
            launcher.downloadUpdate();
            launcher.extractUpdate();
            launcher.writeUpdateManifest();
            launcher.runInstaller();
        }
        launcher.launchGame();
        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    
}