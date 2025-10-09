#include "MuExporter/SceneVisualizer.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace muexporter {

namespace {

char heightToGlyph(float normalizedHeight)
{
    static constexpr const char *gradient = " .:-=+*#%@";
    constexpr std::size_t gradientCount = 10;

    float clamped = std::clamp(normalizedHeight, 0.0f, 1.0f);
    std::size_t index = static_cast<std::size_t>(std::round(clamped * (gradientCount - 1)));
    return gradient[index];
}

struct ObjectMarker {
    std::string label;
    std::size_t occurrences = 0;
};

} // namespace

std::string renderScenePreview(const Scene &scene, const VisualizationOptions &options)
{
    const auto &terrain = scene.terrain;
    if (terrain.tiles.empty() || terrain.width == 0 || terrain.height == 0) {
        return "<no terrain data available>";
    }

    std::vector<float> heights;
    heights.reserve(terrain.tiles.size());
    for (const auto &tile : terrain.tiles) {
        heights.push_back(tile.height);
    }

    auto [minIt, maxIt] = std::minmax_element(heights.begin(), heights.end());
    float minHeight = (minIt != heights.end()) ? *minIt : 0.0f;
    float maxHeight = (maxIt != heights.end()) ? *maxIt : 0.0f;
    float range = std::max(maxHeight - minHeight, 0.0001f);

    std::size_t renderWidth = terrain.width;
    std::size_t renderHeight = terrain.height;
    if (options.maxWidth > 0 && renderWidth > options.maxWidth) {
        float scale = static_cast<float>(options.maxWidth) / static_cast<float>(renderWidth);
        renderWidth = options.maxWidth;
        renderHeight = static_cast<std::size_t>(std::max(1.0f, std::round(scale * renderHeight)));
    }

    std::vector<char> buffer(renderWidth * renderHeight, ' ');

    for (std::size_t z = 0; z < renderHeight; ++z) {
        for (std::size_t x = 0; x < renderWidth; ++x) {
            std::size_t sampleX = static_cast<std::size_t>(std::round(static_cast<float>(x) * (terrain.width - 1) / std::max<std::size_t>(renderWidth - 1, 1)));
            std::size_t sampleZ = static_cast<std::size_t>(std::round(static_cast<float>(z) * (terrain.height - 1) / std::max<std::size_t>(renderHeight - 1, 1)));
            std::size_t idx = sampleZ * terrain.width + sampleX;
            float normalized = (terrain.tiles[idx].height - minHeight) / range;
            buffer[z * renderWidth + x] = heightToGlyph(normalized);
        }
    }

    std::unordered_map<std::size_t, ObjectMarker> markerMap;

    if (options.showObjects) {
        for (const auto &object : scene.objects) {
            float posX = object.position[0];
            float posZ = object.position[2];
            if (terrain.cellSize <= 0.0f) {
                continue;
            }

            std::size_t tileX = static_cast<std::size_t>(std::round(posX / terrain.cellSize));
            std::size_t tileZ = static_cast<std::size_t>(std::round(posZ / terrain.cellSize));
            if (tileX >= terrain.width || tileZ >= terrain.height) {
                continue;
            }

            std::size_t renderX = static_cast<std::size_t>(std::round(static_cast<float>(tileX) * (renderWidth - 1) / std::max<std::size_t>(terrain.width - 1, 1)));
            std::size_t renderZ = static_cast<std::size_t>(std::round(static_cast<float>(tileZ) * (renderHeight - 1) / std::max<std::size_t>(terrain.height - 1, 1)));
            std::size_t renderIdx = renderZ * renderWidth + renderX;

            auto &marker = markerMap[renderIdx];
            if (marker.occurrences == 0) {
                marker.label = object.name.empty() ? object.mesh : object.name;
            }
            ++marker.occurrences;
            buffer[renderIdx] = marker.occurrences > 1 ? '+' : 'O';
        }
    }

    std::ostringstream oss;
    oss << "Scene: " << scene.metadata.name;
    if (!scene.metadata.version.empty()) {
        oss << " (" << scene.metadata.version << ")";
    }
    oss << '\n';
    oss << "Terrain: " << terrain.width << "x" << terrain.height << " tiles";
    oss << ", min height " << minHeight << ", max height " << maxHeight << "\n";

    for (std::size_t z = 0; z < renderHeight; ++z) {
        for (std::size_t x = 0; x < renderWidth; ++x) {
            oss << buffer[z * renderWidth + x];
        }
        oss << '\n';
    }

    if (options.showObjects && !markerMap.empty()) {
        oss << "Legend:\n";
        for (const auto &[idx, marker] : markerMap) {
            std::size_t x = idx % renderWidth;
            std::size_t z = idx / renderWidth;
            oss << " - (" << x << ", " << z << ") " << marker.label;
            if (marker.occurrences > 1) {
                oss << " (" << marker.occurrences << " objects)";
            }
            oss << '\n';
        }
    }

    return oss.str();
}

} // namespace muexporter

