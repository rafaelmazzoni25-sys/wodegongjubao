#pragma once

#include "Scene.hpp"

#include <string>

namespace muexporter {

struct VisualizationOptions {
    bool showObjects = true;
    std::size_t maxWidth = 120;
};

// Creates a textual preview of the terrain heightmap and optional objects.
// The output is suitable for console rendering.
std::string renderScenePreview(const Scene &scene, const VisualizationOptions &options = {});

} // namespace muexporter

