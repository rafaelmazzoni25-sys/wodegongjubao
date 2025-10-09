#include "MuExporter/SceneIO.hpp"

#include <charconv>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace muexporter {
namespace {
class SoulSceneImporter final : public SceneImporter {
public:
    Scene importScene(const std::filesystem::path &file) override {
        std::ifstream stream(file);
        if (!stream) {
            throw std::runtime_error("Failed to open scene file: " + file.string());
        }

        Section current = Section::None;
        Scene scene;
        std::string line;
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
            if (line.front() == '[' && line.back() == ']') {
                current = parseSection(line.substr(1, line.size() - 2));
                continue;
            }

            switch (current) {
            case Section::Scene:
                parseSceneMetadata(line, scene);
                break;
            case Section::Terrain:
                parseTerrain(line, scene);
                break;
            case Section::Objects:
                parseObject(line, scene);
                break;
            case Section::None:
            default:
                break;
            }
        }

        validate(scene, file);
        return scene;
    }

private:
    enum class Section { None, Scene, Terrain, Objects };

    static Section parseSection(const std::string &name) {
        const auto lowered = toLower(name);
        if (lowered == "scene") {
            return Section::Scene;
        }
        if (lowered == "terrain") {
            return Section::Terrain;
        }
        if (lowered == "objects") {
            return Section::Objects;
        }
        return Section::None;
    }

    static std::string trim(const std::string &value) {
        const auto start = value.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            return {};
        }
        const auto end = value.find_last_not_of(" \t\r\n");
        return value.substr(start, end - start + 1);
    }

    static std::string toLower(const std::string &value) {
        std::string result(value.size(), '\0');
        for (std::size_t i = 0; i < value.size(); ++i) {
            result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(value[i])));
        }
        return result;
    }

    static void parseSceneMetadata(const std::string &line, Scene &scene) {
        const auto [key, value] = splitKeyValue(line);
        if (key == "name") {
            scene.metadata.name = value;
        } else if (key == "version") {
            scene.metadata.version = value;
        }
    }

    static void parseTerrain(const std::string &line, Scene &scene) {
        const auto [key, value] = splitKeyValue(line);
        if (key == "width") {
            scene.terrain.width = static_cast<std::size_t>(std::stoul(value));
        } else if (key == "height") {
            scene.terrain.height = static_cast<std::size_t>(std::stoul(value));
        } else if (key == "cell") {
            scene.terrain.cellSize = std::stof(value);
        } else if (key == "heights") {
            scene.terrain.tiles.clear();
            std::istringstream stream(value);
            std::string token;
            while (stream >> token) {
                TerrainTile tile{};
                tile.height = std::stof(token);
                scene.terrain.tiles.push_back(tile);
            }
        }
    }

    static void parseObject(const std::string &line, Scene &scene) {
        const auto [key, value] = splitKeyValue(line);
        if (key != "object") {
            return;
        }

        SceneObject object;
        std::istringstream stream(value);
        std::getline(stream, object.name, ',');
        std::getline(stream, object.mesh, ',');

        for (float &component : object.position) {
            component = readFloat(stream);
        }
        for (float &component : object.rotation) {
            component = readFloat(stream);
        }
        for (float &component : object.scale) {
            component = readFloat(stream, 1.0f);
        }

        scene.objects.push_back(object);
    }

    static std::pair<std::string, std::string> splitKeyValue(const std::string &line) {
        const auto separator = line.find('=');
        if (separator == std::string::npos) {
            throw std::runtime_error("Expected key=value pair: " + line);
        }
        auto key = trim(line.substr(0, separator));
        auto value = trim(line.substr(separator + 1));
        return {toLower(key), value};
    }

    static float readFloat(std::istringstream &stream, float fallback = 0.0f) {
        std::string token;
        if (!std::getline(stream, token, ',')) {
            return fallback;
        }
        return std::stof(trim(token));
    }

    static void validate(const Scene &scene, const std::filesystem::path &file) {
        if (scene.metadata.name.empty()) {
            throw std::runtime_error("Scene missing name in " + file.string());
        }
        if (scene.terrain.width * scene.terrain.height != scene.terrain.tiles.size()) {
            throw std::runtime_error("Terrain tile count mismatch in " + file.string());
        }
    }
};

class SoulSceneImporterFactory final : public SceneImporterFactory {
public:
    bool supports(const PluginDescriptor &descriptor) const override {
        return descriptor.type == "map_importer" && descriptor.format == "scene" &&
               descriptor.entryPoint == "SoulSceneImporter";
    }

    std::unique_ptr<SceneImporter> create(const PluginDescriptor &descriptor) const override {
        if (!supports(descriptor)) {
            throw std::runtime_error("Unsupported descriptor: " + descriptor.name);
        }
        return std::make_unique<SoulSceneImporter>();
    }
};

const SoulSceneImporterFactory g_factory;

} // namespace

const SceneImporterFactory &getSoulSceneImporterFactory() {
    return g_factory;
}

} // namespace muexporter
