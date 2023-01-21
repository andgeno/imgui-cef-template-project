#include "Application.hpp"
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl.h>
#include <glad/glad.h>
#include <imgui.h>

#include "Core/Instrumentor.hpp"
#include "StringUtils.h"

namespace App {

    Application::Application(const std::string& title)
    {
        APP_PROFILE_FUNCTION();

        // Setup CEF
        int cefResult = ImGui_ImplSDL2_CefInit(0, nullptr);
        if(cefResult >= 0)
        {
            return;
        }

        if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
        {
            APP_ERROR("Error: %s\n", SDL_GetError());
            m_exitStatus = ExitStatus::FAILURE;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

        m_window = std::make_shared<Window>(Window::Settings{title});

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        SetTheme();

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForOpenGL(m_window->GetNativeWindow(), m_window->GetNativeContext());
        ImGui_ImplOpenGL3_Init("#version 410 core");

        InitDatabase();
    }

    Application::~Application()
    {
        APP_PROFILE_FUNCTION();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_Quit();
    }

    ExitStatus App::Application::Run()
    {
        APP_PROFILE_FUNCTION();

        if(m_exitStatus == ExitStatus::FAILURE)
        {
            return m_exitStatus;
        }

        Tests();

        m_state.running = true;

        const ImGuiIO& io{ImGui::GetIO()};

        while(m_state.running)
        {
            APP_PROFILE_SCOPE("MainLoop");

            SDL_Event event{};
            while(SDL_PollEvent(&event) != 0)
            {
                APP_PROFILE_SCOPE("EventPolling");

                ImGui_ImplSDL2_ProcessEvent(&event);

                if(event.type == SDL_QUIT)
                {
                    Stop();
                }

                if(event.type == SDL_WINDOWEVENT
                   && event.window.windowID == SDL_GetWindowID(m_window->GetNativeWindow()))
                {
                    OnEvent(event.window);
                }
            }

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            if(!m_state.minimized)
            {
                ImGui::DockSpaceOverViewport();

                if(ImGui::BeginMainMenuBar())
                {
                    if(ImGui::BeginMenu("File"))
                    {
                        if(ImGui::MenuItem("Exit", "Cmd+Q"))
                        {
                            Stop();
                        }
                        ImGui::EndMenu();
                    }
                    if(ImGui::BeginMenu("View"))
                    {
                        ImGui::MenuItem("Some Panel", nullptr, &m_state.showSomePanel);
                        ImGui::EndMenu();
                    }

                    ImGui::EndMainMenuBar();
                }
            }

            if(m_state.showInGameBrowserWindow)
            {
                ImGui::ShowBrowserWindow(&m_state.showInGameBrowserWindow, ImGui_ImplSDL2_GetCefTexture());
            }

            // Whatever GUI to implement here ...
            if(m_state.showSomePanel)
            {
                ImGui::Begin("Some panel", &m_state.showSomePanel);
                // NOLINTNEXTLINE
                ImGui::Text("Hello World");
                ImGui::Checkbox("In Game Browser", &m_state.showInGameBrowserWindow);

                if(ImGui::Button("Login"))
                {
                    bool bSuccess;
                    std::string responseJson;
                    Login(bSuccess, responseJson);
                    printf("Login: bSuccess<%s> json<%s>\n", BOOL_TO_STRING(bSuccess), responseJson.c_str());
                }

                ImGui::End();
            }

            // Rendering
            ImGui::Render();
            glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
            glClearColor(0.5F, 0.5F, 0.5F, 1.00F);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            if((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
            {
                SDL_Window* backup_current_window{SDL_GL_GetCurrentWindow()};
                SDL_GLContext backup_current_context{SDL_GL_GetCurrentContext()};
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
            }

            SDL_GL_SwapWindow(m_window->GetNativeWindow());
        }

        return m_exitStatus;
    }

    void App::Application::Stop()
    {
        m_state.running = false;
    }

    void Application::OnEvent(const SDL_WindowEvent& event)
    {
        APP_PROFILE_FUNCTION();

        switch(event.event)
        {
        case SDL_WINDOWEVENT_CLOSE: return OnClose();
        case SDL_WINDOWEVENT_MINIMIZED: return OnMinimized();
        case SDL_WINDOWEVENT_SHOWN: return OnShown();
        case SDL_WINDOWEVENT_RESIZED: return OnResized(event);
        }
    }

    // Can be static, but will serve as an example call, so ignore.
    // NOLINTNEXTLINE
    void Application::OnResized([[maybe_unused]] const SDL_WindowEvent& event)
    {
        APP_DEBUG("RESIZE {} {}", event.data1, event.data2);
    }

    void Application::OnMinimized()
    {
        m_state.minimized = true;
    }

    void Application::OnShown()
    {
        m_state.minimized = false;
    }

    void Application::OnClose()
    {
        Stop();
    }

    void Application::SetTheme() const
    {
        APP_PROFILE_FUNCTION();

        ImGuiIO& io{ImGui::GetIO()};
        ImGuiStyle& style{ImGui::GetStyle()};

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard
                          | ImGuiConfigFlags_DockingEnable
                          | ImGuiConfigFlags_ViewportsEnable;

        // ImGui font
        const float font_scaling_factor{m_window->GetScale()};
        const float font_size{18.0F * font_scaling_factor};

        io.Fonts->AddFontFromFileTTF("assets/fonts/Manrope/Manrope-Regular.ttf", font_size);
        io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Manrope/Manrope-Regular.ttf", font_size);
        io.FontGlobalScale = 1.0F / font_scaling_factor;

        style.WindowRounding = 5.3F;
        style.GrabRounding = style.FrameRounding = 2.3F;
        style.ScrollbarRounding = 5.0F;
        style.FrameBorderSize = 1.0F;
        style.ItemSpacing.y = 6.5F;

        if((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
        {
            style.WindowRounding = 0.0F;
            style.Colors[ImGuiCol_WindowBg].w = 1.0F;
        }

        // Theme setup ...
        // auto* colors{static_cast<ImVec4*>(style.Colors)};
        // colors[ImGuiCol_Text] = {0.73333335F, 0.73333335F, 0.73333335F, 1.00F};
        // ...
    }

    void Application::SetCommandLineArgs(const int argc, const char* argv[])
    {
        m_argCount = argc;
        m_args = std::vector<std::string>(m_argCount);
        for(int i = 0; i < argc; ++i)
        {
            m_args.emplace_back(argv[i]);
        }
    }

    int Application::GetCommandLineArgCount() const
    {
        return m_argCount;
    }

    std::string Application::GetCommandLineArg(const int index) const
    {
        assert(index >= 0 && index < m_argCount);
        return m_args[index];
    }

    std::vector<std::string> Application::GetCommandLineArgs() const
    {
        return m_args;
    }

    void Application::InitDatabase()
    {
        sqlite3* db = nullptr;
        [[maybe_unused]] char* zErrMsg = nullptr;
        int rc = 0;

        rc = sqlite3_open("test.db", &db);
        if(rc != 0)
        {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            return;
        }

        fprintf(stdout, "Opened database successfully\n");

        sqlite3_close(db);
    }

    void Application::Tests()
    {
        TestJson();
        TestXml();
        TestCurl();
    }

    void Application::TestJson()
    {
        using json = nlohmann::json;

        json ex1 = json::parse(R"(
    {
      "pi": 3.141,
      "happy": true
    }
    )");

        printf("json: %s\n", to_string(ex1).c_str());
    }

    int Application::TestXml()
    {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file("xgconsole.xml");
        if(!result)
        {
            return -1;
        }

        for(pugi::xml_node tool: doc.child("Profile").child("Tools").children("Tool"))
        {
            int timeout = tool.attribute("Timeout").as_int();

            if(timeout > 0)
            {
                printf("Tool %s has timeout %d\n", tool.attribute("Filename").value(), timeout);
            }
        }

        return 0;
    }

    void Application::TestCurl()
    {
        printf("\n");
        CURL* curl;
        CURLcode res;
        curl = curl_easy_init();
        if(curl)
        {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
            curl_easy_setopt(curl, CURLOPT_URL, "https://api.chucknorris.io/jokes/random");
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
            struct curl_slist* headers = NULL;
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            res = curl_easy_perform(curl);
            res = res;
            int x = 1;
            x++;
        }
        curl_easy_cleanup(curl);
        printf("\n");
    }
    void Application::Login(bool& bSuccess, std::string& responseJson)
    {
        bSuccess = false;
        responseJson = "";

        // TODO ...
    }
}