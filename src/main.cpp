#include "gui/Application.h"
#include "common/Program.h"

int Main(const std::vector<std::string>& args)
{
    Application app;
    app.Run();

    return EXIT_SUCCESS;
}