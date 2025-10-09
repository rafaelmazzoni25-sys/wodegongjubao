cmake_minimum_required(VERSION 3.16)
project(MuExporter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(muexporter
    src/main.cpp
    src/PluginManager.cpp
    src/Scene.cpp
    src/SoulSceneImporter.cpp
    src/SceneExporter.cpp
    src/SceneVisualizer.cpp)

target_include_directories(muexporter PRIVATE include)

target_compile_features(muexporter PRIVATE cxx_std_20)
