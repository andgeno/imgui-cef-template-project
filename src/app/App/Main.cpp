#define SDL_MAIN_HANDLED

#include "Core/Application.hpp"
#include "Core/Instrumentor.hpp"
#include "Core/Log.hpp"

int main(const int argc, const char* argv[])
{
    try
    {
        APP_PROFILE_BEGIN_SESSION_WITH_FILE("App", "profile.json");

        {
            APP_PROFILE_SCOPE("Test scope");
            App::Application app{"App"};
            app.SetCommandLineArgs(argc, argv);
            app.Run();
        }

        APP_PROFILE_END_SESSION();
    }
    catch(std::exception& e)
    {
        APP_ERROR("Main process terminated with: {}", e.what());
    }

    return 0;
}
