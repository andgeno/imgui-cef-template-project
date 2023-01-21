set(NAME "Core")

include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
    Core/Log.cpp
    Core/Log.hpp
    Core/Instrumentor.hpp
    Core/Application.cpp
    Core/Application.hpp
    Core/Window.cpp
    Core/Window.hpp
    Core/StringUtils.h
    )


find_package(fmt CONFIG REQUIRED)

target_include_directories(${NAME}
    PRIVATE
    PUBLIC
    ${ZSERGE_WEBVIEW_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

target_compile_features(${NAME} PRIVATE cxx_std_17)
target_link_libraries(${NAME}
    PRIVATE
    project_warnings
    fmt::fmt

    PUBLIC
    #        spdlog
    #        SDL2::SDL2
    #        glad
    imgui

    implot::implot

    pugixml pugixml::shared pugixml::pugixml
    nlohmann_json::nlohmann_json
    unofficial::sqlite3::sqlite3
    CURL::libcurl
    spdlog::spdlog spdlog::spdlog_header_only

    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>

    #        imgui::imgui
    glad::glad

    #    unofficial::webview2::webview2

    vendor/cef/third_party/cef/cef_binary_109.1.16+g454cbc2+chromium-109.0.5414.87_windows64/${CMAKE_BUILD_TYPE}/libcef
    #    cef_binary_${CEF_VERSION}_windows64/${CMAKE_BUILD_TYPE}/libcef
    #        cef_binary_${CEF_VERSION}_windows64/${CMAKE_BUILD_TYPE}/cef_sandbox

    vendor/cef/libcef_dll_wrapper/libcef_dll_wrapper
    #    libcef_dll_wrapper
    )

