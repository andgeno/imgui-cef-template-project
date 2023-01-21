#include "Window.hpp"
#include "Core/Instrumentor.hpp"
#include <glad/glad.h>

namespace App {

    Window::Window(const Settings& settings)
    {
        APP_PROFILE_FUNCTION();

        // Create window with graphics context
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        constexpr auto windowFlags{static_cast<SDL_WindowFlags>(
                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
        )};
        constexpr int windowCenterFlag{SDL_WINDOWPOS_CENTERED};

        m_window = SDL_CreateWindow(
                settings.title.c_str(),
                windowCenterFlag,
                windowCenterFlag,
                settings.width,
                settings.height,
                windowFlags);

        // NOLINTNEXTLINE
        m_glContext = SDL_GL_CreateContext(m_window);
        if(m_glContext == nullptr)
        {
            APP_ERROR("Could not create SDL OpenGL context.");
            return;
        }

        gladLoadGLLoader(SDL_GL_GetProcAddress);
        SDL_GL_MakeCurrent(m_window, m_glContext);
        SDL_GL_SetSwapInterval(1);  // Enable vsync
    }

    Window::~Window()
    {
        APP_PROFILE_FUNCTION();

        SDL_GL_DeleteContext(m_glContext);
        SDL_DestroyWindow(m_window);
    }

    float Window::GetScale() const
    {
        APP_PROFILE_FUNCTION();

        int windowWidth{0};
        int windowHeight{0};
        SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

        int pixelWidth{0};
        int pixelHeight{0};
        SDL_GL_GetDrawableSize(m_window, &pixelWidth, &pixelHeight);

        const auto scaleX{static_cast<float>(pixelWidth) / static_cast<float>(windowWidth)};

        return scaleX;
    }

    SDL_Window* Window::GetNativeWindow() const
    {
        return m_window;
    }

    SDL_GLContext Window::GetNativeContext() const
    {
        return m_glContext;
    }

}
