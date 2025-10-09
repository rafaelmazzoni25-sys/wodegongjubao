#include "MuExporter/PluginManager.hpp"
#include "MuExporter/SceneExporter.hpp"
#include "MuExporter/SceneIO.hpp"
#include "MuExporter/SceneVisualizer.hpp"

#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

using namespace std::string_literals;

namespace muexporter {
namespace {
struct CommandLineOptions {
    std::filesystem::path pluginDirectory;
    std::filesystem::path mapDirectory;
    std::string mapFile;
    std::optional<std::filesystem::path> output;
    bool visualize = false;
    bool visualizeOnly = false;
    VisualizationOptions visualizationOptions;
};

CommandLineOptions parseArguments(int argc, char **argv) {
    CommandLineOptions options;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--plugins" && i + 1 < argc) {
            options.pluginDirectory = argv[++i];
        } else if (arg == "--maps" && i + 1 < argc) {
            options.mapDirectory = argv[++i];
        } else if (arg == "--map" && i + 1 < argc) {
            options.mapFile = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            options.output = std::filesystem::path(argv[++i]);
        } else if (arg == "--visualize") {
            options.visualize = true;
        } else if (arg == "--visualize-only") {
            options.visualize = true;
            options.visualizeOnly = true;
        } else if (arg == "--no-object-overlay") {
            options.visualizationOptions.showObjects = false;
        } else if (arg == "--preview-width" && i + 1 < argc) {
            options.visualizationOptions.maxWidth = static_cast<std::size_t>(std::stoul(argv[++i]));
        } else if (arg == "--help" || arg == "-h") {
            throw std::runtime_error("MuExporter usage:\n"
                                     "  --plugins <dir>   Directory containing *.plug descriptors\n"
                                     "  --maps <dir>      Directory containing map files\n"
                                     "  --map <file>      Map file to export\n"
                                     "  [--output <file>] Optional output file (stdout when omitted)\n"
                                     "  [--visualize]     Print an ASCII preview of the scene\n"
                                     "  [--visualize-only]Preview without exporting JSON\n"
                                     "  [--no-object-overlay] Hide objects in the preview\n"
                                     "  [--preview-width <n>] Clamp preview width to N characters");
        }
    }

    if (options.pluginDirectory.empty() || options.mapDirectory.empty() || options.mapFile.empty()) {
        throw std::runtime_error("Missing required arguments. Use --help for usage information.");
    }

    return options;
}

std::string detectFormat(const std::filesystem::path &file) {
    const auto ext = file.extension().string();
    if (!ext.empty() && ext.front() == '.') {
        return ext.substr(1);
    }
    return "";
}

const SceneImporterFactory &selectImporterFactory(const std::string &format) {
    if (format == "scene") {
        return getSoulSceneImporterFactory();
    }
    throw std::runtime_error("No importer registered for format: " + format);
}

} // namespace
} // namespace muexporter

int main(int argc, char **argv) {
    using namespace muexporter;
    try {
        const auto options = parseArguments(argc, argv);

        PluginManager pluginManager;
        pluginManager.loadDirectory(options.pluginDirectory);

        const auto mapPath = options.mapDirectory / options.mapFile;
        if (!std::filesystem::exists(mapPath)) {
            throw std::runtime_error("Map file not found: " + mapPath.string());
        }

        const auto format = detectFormat(mapPath);
        if (format.empty()) {
            throw std::runtime_error("Unable to determine map format for " + mapPath.string());
        }

        const auto descriptor = pluginManager.findByFormat(format);
        if (!descriptor) {
            throw std::runtime_error("No plugin descriptor found for format: " + format);
        }

        const auto &factory = selectImporterFactory(format);
        auto importer = factory.create(*descriptor);
        Scene scene = importer->importScene(mapPath);

        if (options.visualize) {
            const auto preview = renderScenePreview(scene, options.visualizationOptions);
            std::cout << preview << "\n";
        }

        if (!options.visualizeOnly) {
            if (options.output) {
                SceneExporter::writeJson(scene, *options.output);
            } else {
                SceneExporter::writeJson(scene, std::cout);
            }
        }

        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
