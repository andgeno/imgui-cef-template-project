message(STATUS "Fetching imgui ...")

set(CMAKE_CXX_STANDARD 20)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()

add_library(imgui
    ${imgui_SOURCE_DIR}/imgui.cpp ${imgui_SOURCE_DIR}/imgui.h
    ${imgui_SOURCE_DIR}/imconfig.h ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp ${imgui_SOURCE_DIR}/imgui_internal.h
    ${imgui_SOURCE_DIR}/imgui_tables.cpp ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imstb_rectpack.h ${imgui_SOURCE_DIR}/imstb_textedit.h
    ${imgui_SOURCE_DIR}/imstb_truetype.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl.h ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp

    ${imgui_SOURCE_DIR}/imgui_browser.cpp
    ${imgui_SOURCE_DIR}/sdl_keyboard_utils.h
    )

target_include_directories(imgui
    PUBLIC
    ${imgui_SOURCE_DIR}
    ${CEF_ROOT}
    ${CEF_ROOT}/include
    )


## NOTE: https://github.com/microsoft/vcpkg/issues/332
#add_library(cef INTERFACE)
#find_library(CEF_LIBRARY_DEBUG libcef_dll_wrapper)
#string(REPLACE "/debug/lib/" "/lib/" CEF_LIBRARY_RELEASE ${CEF_LIBRARY_DEBUG})
#find_path(CEF_INCLUDE_DIR include/cef_app.h)
#find_path(CEF_SHARE share/cef-prebuilt)
#set(CEF_SHARE "${CEF_SHARE}/share/cef-prebuilt")
#target_link_libraries(cef INTERFACE "$<IF:$<CONFIG:Debug>,${CEF_LIBRARY_DEBUG},${CEF_LIBRARY_RELEASE}>")
#target_link_libraries(cef INTERFACE "${CEF_SHARE}/$<CONFIG>/cef_sandbox.lib")
#target_link_libraries(cef INTERFACE "${CEF_SHARE}/$<CONFIG>/libcef.lib")
#target_include_directories(cef INTERFACE ${CEF_INCLUDE_DIR})


target_link_libraries(imgui
    PUBLIC
    SDL2::SDL2

    glad::glad

    #        cef_binary_${CEF_VERSION}_windows64/${CMAKE_BUILD_TYPE}/libcef
    #        cef_binary_${CEF_VERSION}_windows64/${CMAKE_BUILD_TYPE}/cef_sandbox

    #        libcef_dll_wrapper
    )

FetchContent_MakeAvailable(imgui)
