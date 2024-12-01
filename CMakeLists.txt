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

find_package(PNG REQUIRED)

# A fő program forrása
add_executable(png_metadata_reader main.cpp)

# Adjuk hozzá a libpng és wxWidgets include mappáit
target_include_directories(png_metadata_reader PRIVATE 
    ${PNG_INCLUDE_DIRS}
    ${wxWidgets_INCLUDE_DIRS}
)

# Linkeljük a szükséges könyvtárakat
target_link_libraries(png_metadata_reader PRIVATE 
    ${PNG_LIBRARIES}
    ${wxWidgets_LIBRARIES}
)

# Szükség esetén adjunk hozzá platformspecifikus beállításokat wxWidgets-hez
if(wxWidgets_USE_FILE)
    include(${wxWidgets_USE_FILE})
endif()
