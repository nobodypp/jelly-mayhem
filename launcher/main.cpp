#include "launcher.hpp"

#include <iostream>
#include <exception>

int main()
{
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