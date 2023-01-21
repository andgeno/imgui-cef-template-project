#pragma once
#include <SDL.h>
#include <memory>
#include <string>
#include <vector>
#include "Core/Window.hpp"

#include <sqlite3.h>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <curl/curl.h>

namespace App {

    enum class ExitStatus : int
    {
        SUCCESS = 0,
        FAILURE = 1
    };

    class Application
    {
    private:
        struct State
        {
            bool running{false};
            bool minimized{false};
            bool showSomePanel{true};
            bool showInGameBrowserWindow{false};
        };

    private:
        ExitStatus m_exitStatus{ExitStatus::SUCCESS};
        std::shared_ptr<Window> m_window{nullptr};
        State m_state{};

        int m_argCount{0};
        std::vector<std::string> m_args{};

    public:
        explicit Application(const std::string& title);
        ~Application();

        Application(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application other) = delete;
        Application& operator=(Application&& other) = delete;

        void SetCommandLineArgs(const int argc, const char* argv[]);
        [[nodiscard]] int GetCommandLineArgCount() const;
        [[nodiscard]] std::string GetCommandLineArg(const int index) const;
        [[nodiscard]] std::vector<std::string> GetCommandLineArgs() const;

        ExitStatus Run();
        void Stop();

        void OnEvent(const SDL_WindowEvent& event);
        void OnResized(const SDL_WindowEvent& event);
        void OnMinimized();
        void OnShown();
        void OnClose();

    private:
        void InitDatabase();

        void SetTheme() const;

        void Login(bool& bSuccess, std::string& responseJson);

        // DEBUG ---------------------------------------------------------------
        void Tests();
        void TestJson();
        int TestXml();
        void TestCurl();
        // DEBUG ---------------------------------------------------------------
    };

}