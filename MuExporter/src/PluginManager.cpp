#include "MuExporter/PluginManager.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace muexporter {
namespace {
std::string trim(const std::string &value) {
    const auto start = value.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return {};
    }
    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}
} // namespace

void PluginManager::loadDirectory(const std::filesystem::path &directory) {
    m_descriptors.clear();
    if (!std::filesystem::exists(directory)) {
        throw std::runtime_error("Plugin directory does not exist: " + directory.string());
    }

    for (const auto &entry : std::filesystem::directory_iterator(directory)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        if (entry.path().extension() != ".plug") {
            continue;
        }
        m_descriptors.push_back(parseDescriptor(entry.path()));
    }
}

std::optional<PluginDescriptor> PluginManager::findByFormat(const std::string &format) const {
    for (const auto &descriptor : m_descriptors) {
        if (descriptor.format == format) {
            return descriptor;
        }
    }
    return std::nullopt;
}

PluginDescriptor PluginManager::parseDescriptor(const std::filesystem::path &file) const {
    std::ifstream stream(file);
    if (!stream) {
        throw std::runtime_error("Failed to open plugin descriptor: " + file.string());
    }

    PluginDescriptor descriptor;
    descriptor.sourcePath = file;
    std::string line;

    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        const auto separator = line.find('=');
        if (separator == std::string::npos) {
            continue;
        }
        const auto key = trim(line.substr(0, separator));
        const auto value = trim(line.substr(separator + 1));
        if (key == "name") {
            descriptor.name = value;
        } else if (key == "type") {
            descriptor.type = value;
        } else if (key == "format") {
            descriptor.format = value;
        } else if (key == "entry") {
            descriptor.entryPoint = value;
        }
    }

    if (descriptor.name.empty() || descriptor.type.empty() || descriptor.format.empty()) {
        throw std::runtime_error("Incomplete plugin descriptor: " + file.string());
    }

    return descriptor;
}

} // namespace muexporter
