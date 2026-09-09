#include "bootstrapper.hpp"

#include <iostream>
#include <exception>

int main()
{
    try
    {
        Bootstrapper bootstrapper;
        return bootstrapper.run();
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Bootstrapper error: " << exception.what() << '\n';
        return 1;
    }
}