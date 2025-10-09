#include "MuExporter/SceneExporter.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace muexporter {
namespace {
void writeIndent(std::ostream &stream, int level) {
    for (int i = 0; i < level; ++i) {
        stream << "  ";
    }
}

void writeArray(std::ostream &stream, const float *values, std::size_t count) {
    stream << '[';
    for (std::size_t i = 0; i < count; ++i) {
        if (i != 0) {
            stream << ',';
        }
        stream << values[i];
    }
    stream << ']';
}
} // namespace

void SceneExporter::writeJson(const Scene &scene, std::ostream &stream) {
    stream << std::fixed << std::setprecision(3);
    stream << "{\n";

    writeIndent(stream, 1);
    stream << "\"metadata\": {\n";
    writeIndent(stream, 2);
    stream << "\"name\": \"" << scene.metadata.name << "\",\n";
    writeIndent(stream, 2);
    stream << "\"version\": \"" << scene.metadata.version << "\"\n";
    writeIndent(stream, 1);
    stream << "},\n";

    writeIndent(stream, 1);
    stream << "\"terrain\": {\n";
    writeIndent(stream, 2);
    stream << "\"width\": " << scene.terrain.width << ",\n";
    writeIndent(stream, 2);
    stream << "\"height\": " << scene.terrain.height << ",\n";
    writeIndent(stream, 2);
    stream << "\"cellSize\": " << scene.terrain.cellSize << ",\n";
    writeIndent(stream, 2);
    stream << "\"heights\": [";
    for (std::size_t i = 0; i < scene.terrain.tiles.size(); ++i) {
        if (i != 0) {
            stream << ',';
        }
        stream << scene.terrain.tiles[i].height;
    }
    stream << "]\n";
    writeIndent(stream, 1);
    stream << "},\n";

    writeIndent(stream, 1);
    stream << "\"objects\": [\n";
    for (std::size_t i = 0; i < scene.objects.size(); ++i) {
        const auto &object = scene.objects[i];
        writeIndent(stream, 2);
        stream << "{\n";
        writeIndent(stream, 3);
        stream << "\"name\": \"" << object.name << "\",\n";
        writeIndent(stream, 3);
        stream << "\"mesh\": \"" << object.mesh << "\",\n";
        writeIndent(stream, 3);
        stream << "\"position\": ";
        writeArray(stream, object.position, 3);
        stream << ",\n";
        writeIndent(stream, 3);
        stream << "\"rotation\": ";
        writeArray(stream, object.rotation, 3);
        stream << ",\n";
        writeIndent(stream, 3);
        stream << "\"scale\": ";
        writeArray(stream, object.scale, 3);
        stream << "\n";
        writeIndent(stream, 2);
        stream << "}";
        if (i + 1 != scene.objects.size()) {
            stream << ',';
        }
        stream << "\n";
    }
    writeIndent(stream, 1);
    stream << "]\n";

    stream << "}\n";
}

void SceneExporter::writeJson(const Scene &scene, const std::filesystem::path &filePath) {
    std::ofstream stream(filePath);
    if (!stream) {
        throw std::runtime_error("Failed to open output file: " + filePath.string());
    }
    writeJson(scene, stream);
}

} // namespace muexporter
