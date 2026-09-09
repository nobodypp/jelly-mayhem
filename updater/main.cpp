#include "updatemanager.hpp"
#include <iostream>


int main()
{
    UpdateManager updater;
    if (updater.checkForUpdate())
    {
        updater.downloadUpdate();
        updater.extractUpdate();
        updater.writeUpdateManifest();
    }
    return 0;
}