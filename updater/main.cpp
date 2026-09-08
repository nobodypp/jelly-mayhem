#include "updatemanager.hpp"
#include <iostream>


int main()
{
    UpdateManager updates;
    updates.checkForUpdate();
    std::cout << updates.getUpdateAvailable() << "\n";
    return 0;
}