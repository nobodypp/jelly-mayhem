#include "installer.hpp"


int main()
{
    try
    {
        Installer installer;
        const auto updateRoot = installer.readUpdateManifest();
        installer.installUpdate(updateRoot);
        installer.cleanupUpdate(updateRoot);

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Installer error: " << e.what() << '\n';
        return 1;
    }
}