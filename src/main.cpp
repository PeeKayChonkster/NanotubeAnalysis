#include "app.hpp"


#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000
#define WINDOW_NAME "Nanotubes analysis"

int main()
{
    nano::App app(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME);

    return app.run();
}