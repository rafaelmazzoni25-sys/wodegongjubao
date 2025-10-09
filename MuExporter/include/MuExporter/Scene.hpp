#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace muexporter {

struct TerrainTile {
    float height;
};

struct Terrain {
    std::size_t width = 0;
    std::size_t height = 0;
    float cellSize = 1.0f;
    std::vector<TerrainTile> tiles;
};

struct SceneObject {
    std::string name;
    std::string mesh;
    float position[3]{};
    float rotation[3]{};
    float scale[3]{1.0f, 1.0f, 1.0f};
};

struct SceneMetadata {
    std::string name;
    std::string version;
};

struct Scene {
    SceneMetadata metadata;
    Terrain terrain;
    std::vector<SceneObject> objects;
};

} // namespace muexporter
