#include "updatemanager.hpp"
#include <iostream>


int main()
{
    UpdateManager updates;
    updates.checkForUpdate();
    if (updates.getUpdateAvailable())
    {
        updates.downloadUpdate();
        updates.installUpdate();
    }
    return 0;
}