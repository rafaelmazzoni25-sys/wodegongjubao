#pragma once

#include "Scene.hpp"

#include <filesystem>
#include <ostream>

namespace muexporter {

class SceneExporter {
public:
    static void writeJson(const Scene &scene, std::ostream &stream);
    static void writeJson(const Scene &scene, const std::filesystem::path &filePath);
};

} // namespace muexporter
