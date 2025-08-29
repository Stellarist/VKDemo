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
public:
	static std::unique_ptr<Scene>   loadScene(std::string_view file_path);
	static std::unique_ptr<SubMesh> loadModel(std::string_view file_path, uint32_t index);

	static std::unique_ptr<Node>     parseNode(const tinygltf::Node& tfnode, size_t index);
	static std::unique_ptr<Mesh>     parseMesh(const tinygltf::Mesh& tfmesh);
	static std::unique_ptr<Camera>   parseCamera(const tinygltf::Camera& tfcamera);
	static std::unique_ptr<Material> parseMaterial(const tinygltf::Material& tfmaterial);
	static std::unique_ptr<Light>    parseLight(const tinygltf::Light& tflight);
	// static std::unique_ptr<Texture>  parseTexture(const tinygltf::Texture& tftexture) ;
	// static std::unique_ptr<Image>    parseImage(tinygltf::Image& tfimage) ;
	// static std::unique_ptr<Sampler>  parseSampler(const tinygltf::Sampler& tfsampler) ;

	static void printSceneNodes(const Scene& scene);
	static void printSceneComponents(const Scene& scene);
};
