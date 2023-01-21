#pragma once
#include <SDL.h>
#include <string>

namespace App {

    class Window
    {
    public:
        struct Settings
        {
            std::string title;
            const int width{1280};
            const int height{720};
        };

    private:
        SDL_Window* m_window;
        SDL_GLContext m_glContext;

    public:
        explicit Window(const Settings& settings);
        ~Window();

        Window(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window other) = delete;
        Window& operator=(Window&& other) = delete;

        [[nodiscard]] float GetScale() const;

        [[nodiscard]] SDL_Window* GetNativeWindow() const;
        [[nodiscard]] SDL_GLContext GetNativeContext() const;
    };

}