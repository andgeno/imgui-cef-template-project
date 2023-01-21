message(STATUS "Fetching CEF ...")

# FetchContent_MakeAvailable(cef)

set(CEF_RUNTIME_LIBRARY_FLAG "/MD")
SET(USE_SANDBOX OFF)

set(CMAKE_CONFIGURATION_TYPES Debug Release)

# Use folders in the resulting project files.
set_property(GLOBAL PROPERTY OS_FOLDERS ON)

## Specify the CEF distribution version.
## https://cef-builds.spotifycdn.com/index.html
#set(CEF_VERSION "106.0.26+ge105400+chromium-106.0.5249.91")   ## 2022-10-04
set(CEF_VERSION "109.1.16+g454cbc2+chromium-109.0.5414.87")    ## 2023-01-24

# Determine the platform.
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    if("${PROJECT_ARCH}" STREQUAL "arm64")
        set(CEF_PLATFORM "macosarm64")
    elseif("${PROJECT_ARCH}" STREQUAL "x86_64")
        set(CEF_PLATFORM "macosx64")
    elseif("${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "arm64")
        set(PROJECT_ARCH "arm64")
        set(CEF_PLATFORM "macosarm64")
    else()
        set(PROJECT_ARCH "x86_64")
        set(CEF_PLATFORM "macosx64")
    endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm")
        set(CEF_PLATFORM "linuxarm")
    elseif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
        set(CEF_PLATFORM "linuxarm64")
    elseif(CMAKE_SIZEOF_VOID_P MATCHES 8)
        set(CEF_PLATFORM "linux64")
    else()
        message(FATAL_ERROR "Linux x86 32-bit builds are discontinued.")
    endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    if("${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}" STREQUAL "ARM64")
        set(CEF_PLATFORM "windowsarm64")
    elseif(CMAKE_SIZEOF_VOID_P MATCHES 8)
        set(CEF_PLATFORM "windows64")
    else()
        set(CEF_PLATFORM "windows32")
    endif()
endif()

# Add this project's cmake/ directory to the module path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Download and extract the CEF binary distribution (executes DownloadCEF.cmake).
include(DownloadCEF.cmake)
DownloadCEF("${CEF_PLATFORM}" "${CEF_VERSION}" "${CMAKE_CURRENT_BINARY_DIR}/third_party/cef")

# Add the CEF binary distribution's cmake/ directory to the module path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

# Load the CEF configuration (executes FindCEF.cmake).
find_package(CEF REQUIRED)

# Include the libcef_dll_wrapper target (executes libcef_dll/CMakeLists.txt).
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
