#include "SceneLoader.hpp"

#include <print>
#include <queue>
#include <stdexcept>

std::unique_ptr<Scene> SceneLoader::loadScene(std::string_view file_path)
{
	std::string error, warn;
	if (!loader.LoadASCIIFromFile(&model, &error, &warn, file_path.data()))
		throw std::runtime_error("Failed to load glTF file.");
	if (!error.empty())
		std::println("Error: {}", error.c_str());
	if (!warn.empty())
		std::println("Warning: {}", warn.c_str());
	std::println("Loaded glTF file: {}", file_path.data());

	auto scene = std::make_unique<Scene>();
	scene->setName("Default Scene");

	// Load Cameras
	std::vector<std::unique_ptr<Camera>> cameras;
	for (const auto& tfcamera : model.cameras)
		cameras.push_back(parseCamera(tfcamera));
	scene->setComponents(std::move(cameras));

	// Load Lights
	std::vector<std::unique_ptr<Light>> lights;
	for (const auto& tflight : model.lights)
		lights.push_back(parseLight(tflight));
	scene->setComponents(std::move(lights));

	// Load Materials
	std::vector<Texture*> textures;
	if (scene->hasComponent<Texture>())
		textures = scene->getComponents<Texture>();
	std::vector<std::unique_ptr<Material>> materials;
	for (const auto& tfmaterial : model.materials) {
		auto material = parseMaterial(tfmaterial);
		// TODO: add texture
		materials.push_back(std::move(material));
	}
	scene->setComponents(std::move(materials));

	// Load Nodes
	std::vector<std::unique_ptr<Node>> nodes;
	for (size_t index = 0; index < model.nodes.size(); ++index) {
		auto tfnode = model.nodes[index];
		auto node = parseNode(tfnode, index);

		if (tfnode.mesh >= 0) {
			auto meshes = scene->getComponents<Mesh>();
			assert(tfnode.mesh < meshes.size());
			auto mesh = meshes[tfnode.mesh];
			node->setComponent(*mesh);
			mesh->addNode(*node);
		}

		if (tfnode.camera >= 0) {
			auto cameras = scene->getComponents<Camera>();
			assert(tfnode.camera < cameras.size());
			auto camera = cameras[tfnode.camera];
			node->setComponent(*camera);
			camera->setNode(*node);
		}

		if (tfnode.light >= 0) {
			auto lights = scene->getComponents<Light>();
			assert(tfnode.light < lights.size());
			auto light = lights[tfnode.light];
			node->setComponent(*light);
			light->setNode(*node);
		}

		nodes.push_back(std::move(node));
	}

	// Load Scenes
	std::queue<std::pair<Node&, int>> traverse_nodes;

	tinygltf::Scene* gltf_scene{};

	for (const auto& tfmesh : model.meshes) {
		auto mesh = parseMesh(tfmesh);
		scene->addComponent(std::move(mesh));
	}

	return scene;
}

std::unique_ptr<SubMesh> SceneLoader::loadModel(std::string_view file_path, uint32_t index)
{
	auto submesh = std::make_unique<SubMesh>();

	// TODO: Load mesh data into submesh
	return submesh;
}

std::unique_ptr<Node> SceneLoader::parseNode(const tinygltf::Node& tfnode, size_t index) const
{
	auto  node = std::make_unique<Node>(index, tfnode.name);
	auto& transform = node->getComponent<Transform>();
	if (!tfnode.translation.empty())
		transform.setTranslation(glm::vec3{
		    static_cast<float>(tfnode.translation[0]),
		    static_cast<float>(tfnode.translation[1]),
		    static_cast<float>(tfnode.translation[2])});

	if (!tfnode.rotation.empty())
		transform.setRotation(glm::quat{
		    static_cast<float>(tfnode.rotation[3]),
		    static_cast<float>(tfnode.rotation[0]),
		    static_cast<float>(tfnode.rotation[1]),
		    static_cast<float>(tfnode.rotation[2])});

	if (!tfnode.scale.empty())
		transform.setScale(glm::vec3{
		    static_cast<float>(tfnode.scale[0]),
		    static_cast<float>(tfnode.scale[1]),
		    static_cast<float>(tfnode.scale[2])});

	if (!tfnode.matrix.empty())
		transform.setMatrix(glm::mat4{
		    static_cast<float>(tfnode.matrix[0]), static_cast<float>(tfnode.matrix[1]), static_cast<float>(tfnode.matrix[2]), static_cast<float>(tfnode.matrix[3]),
		    static_cast<float>(tfnode.matrix[4]), static_cast<float>(tfnode.matrix[5]), static_cast<float>(tfnode.matrix[6]), static_cast<float>(tfnode.matrix[7]),
		    static_cast<float>(tfnode.matrix[8]), static_cast<float>(tfnode.matrix[9]), static_cast<float>(tfnode.matrix[10]), static_cast<float>(tfnode.matrix[11]),
		    static_cast<float>(tfnode.matrix[12]), static_cast<float>(tfnode.matrix[13]), static_cast<float>(tfnode.matrix[14]), static_cast<float>(tfnode.matrix[15])});

	return node;
}

std::unique_ptr<Mesh> SceneLoader::parseMesh(const tinygltf::Mesh& tfmesh) const
{
	auto mesh = std::make_unique<Mesh>(tfmesh.name);

	return mesh;
}

std::unique_ptr<Camera> SceneLoader::parseCamera(const tinygltf::Camera& tfcamera) const
{
	auto camera = std::make_unique<PerspectiveCamera>(tfcamera.name);
	camera->setAspectRatio(static_cast<float>(tfcamera.perspective.aspectRatio));
	camera->setFov(static_cast<float>(tfcamera.perspective.yfov));
	camera->setNearPlane(static_cast<float>(tfcamera.perspective.znear));
	camera->setFarPlane(static_cast<float>(tfcamera.perspective.zfar));

	return camera;
}

std::unique_ptr<Material> SceneLoader::parseMaterial(const tinygltf::Material& tfmaterial) const
{
	auto material = std::make_unique<PBRMaterial>(tfmaterial.name);

	for (auto& tfvalue : tfmaterial.values) {
		if (tfvalue.first == "baseColorFactor") {
			const auto& color = tfvalue.second.ColorFactor();
			material->setBaseColorFactor({color[0], color[1], color[2], color[3]});
		} else if (tfvalue.first == "metallicFactor")
			material->setMetallicFactor(static_cast<float>(tfvalue.second.Factor()));
		else if (tfvalue.first == "roughnessFactor")
			material->setRoughnessFactor(static_cast<float>(tfvalue.second.Factor()));
	}

	for (auto& tfvalue : tfmaterial.additionalValues) {
		if (tfvalue.first == "emissiveFactor") {
			const auto& emissive = tfvalue.second.number_array;
			material->setEmissive({emissive[0], emissive[1], emissive[2]});
		} else if (tfvalue.first == "alphaMode") {
			if (tfvalue.second.string_value == "BLEND")
				material->setAlphaMode(AlphaMode::Blend);
			else if (tfvalue.second.string_value == "OPAQUE")
				material->setAlphaMode(AlphaMode::Opaque);
			else if (tfvalue.second.string_value == "MASK")
				material->setAlphaMode(AlphaMode::Mask);
		} else if (tfvalue.first == "alphaCutoff")
			material->setAlphaCutoff(static_cast<float>(tfvalue.second.number_value));
		else if (tfvalue.first == "doubleSided")
			material->setDoubleSided(tfvalue.second.bool_value);
	}

	return material;
}

std::unique_ptr<Light> SceneLoader::parseLight(const tinygltf::Light& tflight) const
{
	std::unique_ptr<Light> light{};
	if (tflight.type == "directional") {
		light = std::make_unique<DirectionalLight>(tflight.name);
	} else if (tflight.type == "point") {
		light = std::make_unique<PointLight>(tflight.name);
		const auto& range = tflight.range;
		dynamic_cast<PointLight*>(light.get())->setRange(range);
	} else if (tflight.type == "spot") {
		light = std::make_unique<SpotLight>(tflight.name);
		const auto& range = tflight.range;
		dynamic_cast<SpotLight*>(light.get())->setRange(range);
		const auto& spot = tflight.spot;
		dynamic_cast<SpotLight*>(light.get())->setInnerConeAngle(spot.innerConeAngle);
		dynamic_cast<SpotLight*>(light.get())->setOuterConeAngle(spot.outerConeAngle);
	} else
		throw std::runtime_error("Unknown light type");

	const auto& color = tflight.color;
	light->setColor({color[0], color[1], color[2]});

	const auto& intensity = tflight.intensity;
	light->setIntensity(intensity);

	return light;
}

std::unique_ptr<Camera> SceneLoader::createDefaultCamera() const
{
	return std::make_unique<PerspectiveCamera>("default_camera");
}

std::unique_ptr<Material> SceneLoader::createDefaultMaterial() const
{
	return std::make_unique<PBRMaterial>("default_material");
}
