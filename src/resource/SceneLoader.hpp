#pragma once

#include <string_view>

#include <tiny_gltf.h>

#include "scene/Scene.hpp"
#include "scene/Node.hpp"
#include "scene/components/Mesh.hpp"
#include "scene/components/Mesh.hpp"
#include "scene/components/Camera.hpp"
#include "scene/components/Material.hpp"
#include "scene/components/Light.hpp"

class SceneLoader {
private:
	tinygltf::Model    model;
	tinygltf::TinyGLTF loader;

public:
	std::unique_ptr<Scene>   loadScene(std::string_view file_path);
	std::unique_ptr<SubMesh> loadModel(std::string_view file_path, uint32_t index);

	std::unique_ptr<Node>     parseNode(const tinygltf::Node& tfnode, size_t index) const;
	std::unique_ptr<Mesh>     parseMesh(const tinygltf::Mesh& tfmesh) const;
	std::unique_ptr<Camera>   parseCamera(const tinygltf::Camera& tfcamera) const;
	std::unique_ptr<Material> parseMaterial(const tinygltf::Material& tfmaterial) const;
	std::unique_ptr<Light>    parseLight(const tinygltf::Light& tflight) const;
	// std::unique_ptr<Texture>  parseTexture(const tinygltf::Texture& tftexture) const;
	// std::unique_ptr<Image>    parseImage(tinygltf::Image& tfimage) const;
	// std::unique_ptr<Sampler>  parseSampler(const tinygltf::Sampler& tfsampler) const;

	std::unique_ptr<Camera>   createDefaultCamera() const;
	std::unique_ptr<Material> createDefaultMaterial() const;
	// std::unique_ptr<Sampler>            createDefaultSampler(int filter) const;
};
