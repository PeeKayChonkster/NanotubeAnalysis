#include "app.hpp"
#include <iostream>
#include "prim_exception.hpp"


#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define WINDOW_NAME "Nanotubes analysis"

int main()
{
    nano::App app(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME);
    int exitcode = 0;
    try
    {
        int exitcode = app.run();
    }
    catch(const prim::Exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

    return exitcode;
}