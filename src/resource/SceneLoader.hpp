#pragma once

#include <unordered_map>
#include <string_view>

#include <vulkan/vulkan.hpp>
#include <tiny_gltf.h>

#include "scene/Scene.hpp"
#include "scene/Node.hpp"
#include "scene/components/Mesh.hpp"
#include "scene/components/Camera.hpp"
#include "scene/components/Material.hpp"
#include "scene/components/Light.hpp"

class SceneLoader {
	template <typename T>
	static std::vector<T> convertData(std::span<const uint8_t> data, size_t size);

	static std::vector<uint8_t> getAttributeData(const tinygltf::Model& model, uint32_t accessor_index);
	static uint32_t             getAttributeCount(const tinygltf::Model* model, uint32_t accessor_id);
	static uint32_t             getAttributeSize(const tinygltf::Model* model, uint32_t accessor_id);
	static uint32_t             getAttributeStride(const tinygltf::Model* model, uint32_t accessor_id);
	static int                  getAttributeFormat(const tinygltf::Model* model, uint32_t accessor_id);

public:
	static std::unique_ptr<Scene>   loadScene(std::string_view file_path);
	static std::unique_ptr<SubMesh> loadModel(const tinygltf::Model& tfmodel, uint32_t index);

	static std::unique_ptr<Node>     parseNode(const tinygltf::Node& tfnode, size_t index);
	static std::unique_ptr<Mesh>     parseMesh(const tinygltf::Mesh& tfmesh);
	static std::unique_ptr<SubMesh>  parseSubmesh(const tinygltf::Mesh& tfmesh, const tinygltf::Model& model, uint32_t index);
	static std::unique_ptr<Camera>   parseCamera(const tinygltf::Camera& tfcamera);
	static std::unique_ptr<Material> parseMaterial(const tinygltf::Material& tfmaterial);
	static std::unique_ptr<Light>    parseLight(const tinygltf::Light& tflight);
	// static std::unique_ptr<Texture>  parseTexture(const tinygltf::Texture& tftexture) ;
	// static std::unique_ptr<Image>    parseImage(tinygltf::Image& tfimage) ;
	// static std::unique_ptr<Sampler>  parseSampler(const tinygltf::Sampler& tfsampler) ;

	static void printSceneNodes(const Scene& scene);
	static void printSceneComponents(const Scene& scene);
};

template <typename T>
std::vector<T> SceneLoader::convertData(std::span<const uint8_t> data, size_t size)
{
	std::vector<T> result(data.size() / size);
	for (size_t i = 0; i < result.size(); i++)
		std::memcpy(&result[i], &data[i * size], sizeof(T));

	return result;
}

static const std::unordered_map<int, std::unordered_map<int, vk::Format>> formats = {
    {
        TINYGLTF_COMPONENT_TYPE_BYTE,
        {
            {TINYGLTF_TYPE_SCALAR, vk::Format::eR8Sint},
            {TINYGLTF_TYPE_VEC2, vk::Format::eR8G8Sint},
            {TINYGLTF_TYPE_VEC3, vk::Format::eR8G8B8Sint},
            {TINYGLTF_TYPE_VEC4, vk::Format::eR8G8B8A8Sint},
        },
    },
    {
        TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
        {
            {TINYGLTF_TYPE_SCALAR, vk::Format::eR8Uint},
            {TINYGLTF_TYPE_VEC2, vk::Format::eR8G8Uint},
            {TINYGLTF_TYPE_VEC3, vk::Format::eR8G8B8Uint},
            {TINYGLTF_TYPE_VEC4, vk::Format::eR8G8B8A8Uint},
        },
    },
    {
        -TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
        {
            {TINYGLTF_TYPE_SCALAR, vk::Format::eR8Unorm},
            {TINYGLTF_TYPE_VEC2, vk::Format::eR8G8Unorm},
            {TINYGLTF_TYPE_VEC3, vk::Format::eR8G8B8Unorm},
            {TINYGLTF_TYPE_VEC4, vk::Format::eR8G8B8A8Unorm},
        },
    },
    {
        TINYGLTF_COMPONENT_TYPE_SHORT,
        {
            {TINYGLTF_TYPE_SCALAR, vk::Format::eR16Sint},
            {TINYGLTF_TYPE_VEC2, vk::Format::eR16G16Sint},
            {TINYGLTF_TYPE_VEC3, vk::Format::eR16G16B16Sint},
            {TINYGLTF_TYPE_VEC4, vk::Format::eR16G16B16A16Sint},
        },
    },
    {
        TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT,
        {
            {TINYGLTF_TYPE_SCALAR, vk::Format::eR16Uint},
            {TINYGLTF_TYPE_VEC2, vk::Format::eR16G16Uint},
            {TINYGLTF_TYPE_VEC3, vk::Format::eR16G16B16Uint},
            {TINYGLTF_TYPE_VEC4, vk::Format::eR16G16B16A16Uint},
        },
    },
    {
        -TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT,
        {
            {TINYGLTF_TYPE_SCALAR, vk::Format::eR16Unorm},
            {TINYGLTF_TYPE_VEC2, vk::Format::eR16G16Unorm},
            {TINYGLTF_TYPE_VEC3, vk::Format::eR16G16B16Unorm},
            {TINYGLTF_TYPE_VEC4, vk::Format::eR16G16B16A16Unorm},
        },
    },
    {
        TINYGLTF_COMPONENT_TYPE_INT,
        {
            {TINYGLTF_TYPE_SCALAR, vk::Format::eR32Sint},
            {TINYGLTF_TYPE_VEC2, vk::Format::eR32G32Sint},
            {TINYGLTF_TYPE_VEC3, vk::Format::eR32G32B32Sint},
            {TINYGLTF_TYPE_VEC4, vk::Format::eR32G32B32A32Sint},
        },
    },
    {
        TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT,
        {
            {TINYGLTF_TYPE_SCALAR, vk::Format::eR32Uint},
            {TINYGLTF_TYPE_VEC2, vk::Format::eR32G32Uint},
            {TINYGLTF_TYPE_VEC3, vk::Format::eR32G32B32Uint},
            {TINYGLTF_TYPE_VEC4, vk::Format::eR32G32B32A32Uint},
        },
    },
    {
        TINYGLTF_COMPONENT_TYPE_FLOAT,
        {
            {TINYGLTF_TYPE_SCALAR, vk::Format::eR32Sfloat},
            {TINYGLTF_TYPE_VEC2, vk::Format::eR32G32Sfloat},
            {TINYGLTF_TYPE_VEC3, vk::Format::eR32G32B32Sfloat},
            {TINYGLTF_TYPE_VEC4, vk::Format::eR32G32B32A32Sfloat},
        },
    },
    {TINYGLTF_COMPONENT_TYPE_DOUBLE,
     {
         {TINYGLTF_TYPE_SCALAR, vk::Format::eR64Sfloat},
         {TINYGLTF_TYPE_VEC2, vk::Format::eR64G64Sfloat},
         {TINYGLTF_TYPE_VEC3, vk::Format::eR64G64B64Sfloat},
         {TINYGLTF_TYPE_VEC4, vk::Format::eR64G64B64A64Sfloat},
     }}};
