cmake_minimum_required(VERSION 3.24)

include(cmake/AppleBuild.cmake)

project(cmake_with_vcpkg
    DESCRIPTION "Base gui project setup with SDL2 and OpenGL."
    LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF) ## Build STATIC LIBS
#set(BUILD_SHARED_LIBS ON)   ## Build DLLS

#add_definitions(-D_HAS_ITERATOR_DEBUGGING=0)

#add_compile_definitions(_ITERATOR_DEBUG_LEVEL=0)
#add_compile_definitions(_HAS_ITERATOR_DEBUGGING=0)

#add_definitions(-D_ITERATOR_DEBUG_LEVEL=2)
#add_compile_definitions($<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=2>)
#add_compile_definitions($<$<CONFIG:Debug>:_HAS_ITERATOR_DEBUGGING=1>)


set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MD")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MDd")


find_package(fmt CONFIG REQUIRED)
find_package(pugixml CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
#find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
#find_package(unofficial-webview2 CONFIG REQUIRED)
#find_path(ZSERGE_WEBVIEW_INCLUDE_DIRS "webview.h")


#message(STATUS "Fetching CEF ...")

# FetchContent_MakeAvailable(cef)

## Build CEF as DLL
#set(CEF_RUNTIME_LIBRARY_FLAG "/MD")

## NOTE: shut up msvc about exception handling for CEF
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

### Enable/Disable CEF Sandbox
##SET(USE_SANDBOX ON)
#SET(USE_SANDBOX OFF)

include(cmake/StandardProjectSettings.cmake)

# Link this "library" to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)


add_subdirectory(vendor)
add_subdirectory(src)





