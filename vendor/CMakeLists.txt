include(FetchContent)

FetchContent_Declare(
    imgui

    ## Original imgui lib
    #  GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
    #  GIT_TAG 88dfd85e9296fc03ac858f66cc6507ba443294ef # docking-latest

    ## Patched imgui lib: includes new CEF browser component
    GIT_REPOSITORY "https://github.com/andgeno/imgui-cef-patched.git"
    #    GIT_REPOSITORY "file://D://dev//tmp//imgui-cef-patched"
    GIT_TAG docking-with-cef
)
add_subdirectory(imgui)


add_subdirectory(cef)
