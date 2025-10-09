#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace muexporter {

struct PluginDescriptor {
    std::string name;
    std::string type;
    std::string format;
    std::string entryPoint;
    std::filesystem::path sourcePath;
};

class PluginManager {
public:
    void loadDirectory(const std::filesystem::path &directory);
    std::optional<PluginDescriptor> findByFormat(const std::string &format) const;

private:
    PluginDescriptor parseDescriptor(const std::filesystem::path &file) const;

    std::vector<PluginDescriptor> m_descriptors;
};

} // namespace muexporter
