#include "app.hpp"
#include <iostream>
#include "prim_exception.hpp"


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define WINDOW_NAME "Nanotubes analysis"

int main()
{
    int exitcode = 0;
    try
    {
        int exitcode = nano::App::run(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME);
    }
    catch(const prim::Exception& e)
    {
        std::cerr << e.what() << '\n';
        std::cin.get();
        exitcode = 1;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        std::cin.get();
        exitcode = 1;
    }
    
    nano::App::free();

    return exitcode;
}