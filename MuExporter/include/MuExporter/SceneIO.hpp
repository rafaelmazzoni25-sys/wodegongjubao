#pragma once

#include "Scene.hpp"
#include "PluginManager.hpp"

#include <filesystem>
#include <memory>
#include <string>

namespace muexporter {

class SceneImporter {
public:
    virtual ~SceneImporter() = default;
    virtual Scene importScene(const std::filesystem::path &file) = 0;
};

class SceneImporterFactory {
public:
    virtual ~SceneImporterFactory() = default;
    virtual bool supports(const PluginDescriptor &descriptor) const = 0;
    virtual std::unique_ptr<SceneImporter> create(const PluginDescriptor &descriptor) const = 0;
};

const SceneImporterFactory &getSoulSceneImporterFactory();

} // namespace muexporter
