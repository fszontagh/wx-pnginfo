cmake_minimum_required(VERSION 3.10)

# A projekt neve és nyelv specifikáció
project(PngMetadataReader LANGUAGES CXX)

# Beállítjuk a C++ szabványt
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Találjuk meg a wxWidgets és libpng könyvtárakat
find_package(wxWidgets QUIET COMPONENTS base)

# Ha wxWidgets nem található, próbáljuk meg a pkg-config segítségével
if(NOT wxWidgets_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(WX REQUIRED wxWidgets)
    if(WX_FOUND)
        set(wxWidgets_FOUND TRUE)
        set(wxWidgets_INCLUDE_DIRS ${WX_INCLUDE_DIRS})
        set(wxWidgets_LIBRARIES ${WX_LIBRARIES})
        include( "${wxWidgets_USE_FILE}" )
    else()
        message(FATAL_ERROR "wxWidgets not found! Please install wxWidgets or check pkg-config paths.")
    endif()
endif()


option(PNG_TESTS "Build PNG tests" OFF)
option(PNG_USE_GIT "Set the libpng version and build from git. Default is 1.6.44" OFF)
option(PNG_GIT_TAG "Set the libpng version and build from git. Default is 1.6.44" "v1.6.44")
option(PNG_STATIC "Build libpng as static library" OFF)


# disable tests
set(PNG_TESTS OFF)
# disable tools
set(PNG_TOOLS OFF)

set(PNG_LIBNAME "png")

if (PNG_STATIC)
    option(PNG_SHARED "Build libpng as a shared library" OFF)
    option(PNG_STATIC "Build libpng as a static library" ON)
    set(PNG_LIBNAME "png_static")
else()
    option(PNG_SHARED "Build libpng as a shared library" ON)
    option(PNG_STATIC "Build libpng as a static library" OFF)
    set(PNG_LIBNAME "png")
endif()


add_executable(png_metadata_reader main.cpp)


if (PNG_USE_GIT)

include(FetchContent)
FetchContent_Declare(
  libpng
  GIT_REPOSITORY "https://github.com/glennrp/libpng.git"
  GIT_TAG ${PNG_GIT_TAG}
)
message(STATUS "Using libpng ${PNG_GIT_TAG}")

FetchContent_MakeAvailable(libpng)

FetchContent_GetProperties(libpng)

if(NOT libpng_POPULATED)
  FetchContent_Populate(libpng)
  add_subdirectory(${libpng_SOURCE_DIR} ${libpng_BINARY_DIR})
endif()

else()
    message(STATUS "Using system libpng")
    find_package(PNG REQUIRED)
endif()


target_include_directories(png_metadata_reader PRIVATE 
    ${PNG_INCLUDE_DIRS} ${libpng_INCLUDE_DIRS}
    ${wxWidgets_INCLUDE_DIRS}
)

# Linkeljük a szükséges könyvtárakat
target_link_libraries(png_metadata_reader PRIVATE 
    ${wxWidgets_LIBRARIES}
    ${PNG_LIBRARIES} ${libpng_LIBRARIES} ${PNG_LIBNAME}
)


if(wxWidgets_USE_FILE)
    include(${wxWidgets_USE_FILE})
endif()
