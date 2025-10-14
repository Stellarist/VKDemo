#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "SceneLoader.hpp"

#include <queue>
#include <print>
#include <stdexcept>

std::unique_ptr<Scene> SceneLoader::loadScene(std::string_view file_path)
{
	// Load Scene
	tinygltf::Model    model;
	tinygltf::TinyGLTF loader;
	std::string        error, warn;
	if (!loader.LoadASCIIFromFile(&model, &error, &warn, file_path.data())) {
		if (!error.empty())
			std::println("Error: {}", error);
		if (!warn.empty())
			std::println("Warning: {}", warn);
		throw std::runtime_error("Failed to load glTF file.");
	}
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

	// Load Meshes
	for (const auto& tfmesh : model.meshes) {
		auto mesh = parseMesh(tfmesh);
		for (auto index = 0; index < tfmesh.primitives.size(); index++) {
			auto submesh = parseSubmesh(tfmesh, model, index);
			mesh->addSubmesh(*submesh);
			scene->addComponent(std::move(submesh));
		}
		scene->addComponent(std::move(mesh));
	}

	// Load Nodes
	std::vector<std::unique_ptr<Node>> nodes;
	for (size_t index = 0; index < model.nodes.size(); index++) {
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

	tinygltf::Scene* tfscene = &model.scenes[model.defaultScene];
	if (!tfscene)
		throw std::runtime_error("No default scene found in glTF file.");

	auto root_node = std::make_unique<Node>(0, tfscene->name);
	/////////////////////
	scene->setRoot(*root_node);

	return scene;
}

std::unique_ptr<SubMesh> SceneLoader::loadModel(const tinygltf::Model& tfmodel, uint32_t index)
{
	auto submesh = std::make_unique<SubMesh>();

	auto& tfmesh = tfmodel.meshes[index];
	auto& tfprimitive = tfmesh.primitives[index];

	const float* pos = nullptr;
	const float* normals = nullptr;
	const float* uvs = nullptr;
	const float* colors = nullptr;

	auto& accessor = tfmodel.accessors[tfprimitive.attributes.find("POSITION")->second];
	auto& buffer_view = tfmodel.bufferViews[accessor.bufferView];

	size_t vertex_count = accessor.count;
	pos = reinterpret_cast<const float*>(&tfmodel.buffers[buffer_view.buffer].data[accessor.byteOffset + buffer_view.byteOffset]);

	// submesh->setVerticesCount(static_cast<uint32_t>(vertex_count));

	if (tfprimitive.attributes.find("NORMAL") != tfprimitive.attributes.end()) {
		auto& accessor = tfmodel.accessors[tfprimitive.attributes.find("NORMAL")->second];
		auto& buffer_view = tfmodel.bufferViews[accessor.bufferView];
		normals = reinterpret_cast<const float*>(&tfmodel.buffers[buffer_view.buffer].data[accessor.byteOffset + buffer_view.byteOffset]);
	}

	if (tfprimitive.attributes.find("TEXCOORD_0") != tfprimitive.attributes.end()) {
		auto& accessor = tfmodel.accessors[tfprimitive.attributes.find("TEXCOORD_0")->second];
		auto& buffer_view = tfmodel.bufferViews[accessor.bufferView];
		uvs = reinterpret_cast<const float*>(&tfmodel.buffers[buffer_view.buffer].data[accessor.byteOffset + buffer_view.byteOffset]);
	}

	if (tfprimitive.attributes.find("COLOR_0") != tfprimitive.attributes.end()) {
		auto& accessor = tfmodel.accessors[tfprimitive.attributes.find("COLOR_0")->second];
		auto& buffer_view = tfmodel.bufferViews[accessor.bufferView];
		colors = reinterpret_cast<const float*>(&tfmodel.buffers[buffer_view.buffer].data[accessor.byteOffset + buffer_view.byteOffset]);
	}

	// if (tfprimitive.indices >= 0) {
	// 	auto& accessor = tfmodel.accessors[tfprimitive.indices];
	// 	auto& buffer_view = tfmodel.bufferViews[accessor.bufferView];
	// 	auto* indices = reinterpret_cast<const uint32_t*>(&tfmodel.buffers[buffer_view.buffer].data[accessor.byteOffset + buffer_view.byteOffset]);

	// 	std::vector<uint32_t> index_data(indices, indices + accessor.count);
	// 	submesh->setIndices(std::move(index_data));
	// }

	// TODO: Load mesh data into submesh
	return submesh;
}

std::unique_ptr<Node> SceneLoader::parseNode(const tinygltf::Node& tfnode, size_t index)
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
		    static_cast<float>(tfnode.rotation[0]),
		    static_cast<float>(tfnode.rotation[1]),
		    static_cast<float>(tfnode.rotation[2]),
		    static_cast<float>(tfnode.rotation[3])});

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

std::unique_ptr<Mesh> SceneLoader::parseMesh(const tinygltf::Mesh& tfmesh)
{
	auto mesh = std::make_unique<Mesh>(tfmesh.name);

	return mesh;
}

std::unique_ptr<SubMesh> SceneLoader::parseSubmesh(const tinygltf::Mesh& tfmesh, const tinygltf::Model& model, uint32_t index)
{
	const auto& tfprimitive = tfmesh.primitives[index];

	auto submesh_name = std::format("{}_Primitive_{}", tfmesh.name, index);
	auto submesh = std::make_unique<SubMesh>(std::move(submesh_name));

	uint32_t vertex_count = 0, vertex_offset = 0;
	if (!tfprimitive.attributes.empty())
		vertex_count = static_cast<uint32_t>(getAttributeSize(&model, tfprimitive.attributes.begin()->second));
	uint32_t vertex_stride = std::accumulate(tfprimitive.attributes.begin(), tfprimitive.attributes.end(), 0,
	                                         [&](uint32_t sum, const auto& attribute) {
		                                         return sum + static_cast<uint32_t>(getAttributeStride(&model, attribute.second));
	                                         });
	size_t   total_bytes = static_cast<size_t>(vertex_count) * vertex_stride;

	std::vector<float> vertices_data(total_bytes / sizeof(float));

	for (const auto& attribute : tfprimitive.attributes) {
		auto attribute_name = attribute.first;
		auto accessor_id = attribute.second;
		std::transform(attribute_name.begin(), attribute_name.end(), attribute_name.begin(), ::toupper);

		auto format = getAttributeFormat(&model, accessor_id);
		auto stride = static_cast<uint32_t>(getAttributeStride(&model, accessor_id));
		auto data = getAttributeData(model, accessor_id);
		submesh->setAttribute(attribute_name, {format, stride});

		for (uint32_t v = 0; v < vertex_count; v++) {
			size_t dst_offset = v * vertex_stride + vertex_offset;
			size_t src_offset = v * stride;
			std::memcpy(reinterpret_cast<uint8_t*>(vertices_data.data()) + dst_offset,
			            data.data() + src_offset, stride);
		}

		vertex_offset += stride;
	}
	submesh->setVertices(std::move(vertices_data), vertex_count);

	if (tfprimitive.indices >= 0) {
		auto format = getAttributeFormat(&model, tfprimitive.indices);
		auto index_data = getAttributeData(model, tfprimitive.indices);

		std::vector<uint32_t> indices;
		switch (format) {
		case static_cast<int>(vk::Format::eR8Uint):
			indices = convertData<uint32_t>(index_data, 1);
			break;
		case static_cast<int>(vk::Format::eR16Uint):
			indices = convertData<uint32_t>(index_data, 2);
			break;
		case static_cast<int>(vk::Format::eR32Uint):
			indices = convertData<uint32_t>(index_data, 4);
			break;
		default:
			throw std::runtime_error("Unsupported index format");
		}

		submesh->setIndices(std::move(indices));
	}

	return submesh;
}

std::unique_ptr<Camera> SceneLoader::parseCamera(const tinygltf::Camera& tfcamera)
{
	auto camera = std::make_unique<PerspectiveCamera>(tfcamera.name);
	camera->setAspectRatio(static_cast<float>(tfcamera.perspective.aspectRatio));
	camera->setFov(static_cast<float>(tfcamera.perspective.yfov));
	camera->setNearPlane(static_cast<float>(tfcamera.perspective.znear));
	camera->setFarPlane(static_cast<float>(tfcamera.perspective.zfar));

	return camera;
}

std::unique_ptr<Material> SceneLoader::parseMaterial(const tinygltf::Material& tfmaterial)
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

std::unique_ptr<Light> SceneLoader::parseLight(const tinygltf::Light& tflight)
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
		const auto& spot = tflight.spot;
		dynamic_cast<SpotLight*>(light.get())->setRange(range);
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

std::vector<uint8_t> SceneLoader::getAttributeData(const tinygltf::Model& model, uint32_t accessor_index)
{
	assert(accessor_index < model.accessors.size());
	const auto& accessor = model.accessors[accessor_index];

	assert(accessor.bufferView < model.bufferViews.size());
	const auto& buffer_view = model.bufferViews[accessor.bufferView];

	assert(buffer_view.buffer < model.buffers.size());
	const auto& buffer = model.buffers[buffer_view.buffer];

	auto stride = accessor.ByteStride(buffer_view);
	auto start_byte = accessor.byteOffset + buffer_view.byteOffset;
	auto end_byte = start_byte + accessor.count * stride;

	return {buffer.data.begin() + start_byte, buffer.data.begin() + end_byte};
}

size_t SceneLoader::getAttributeSize(const tinygltf::Model* model, uint32_t accessor_id)
{
	assert(accessor_id < model->accessors.size());

	return model->accessors[accessor_id].count;
}

size_t SceneLoader::getAttributeStride(const tinygltf::Model* model, uint32_t accessor_id)
{
	assert(accessor_id < model->accessors.size());
	auto& accessor = model->accessors[accessor_id];

	assert(accessor.bufferView < model->bufferViews.size());
	auto& buffer_view = model->bufferViews[accessor.bufferView];

	return accessor.ByteStride(buffer_view);
}

int SceneLoader::getAttributeFormat(const tinygltf::Model* model, uint32_t accessor_id)
{
	assert(accessor_id < model->accessors.size());
	auto& accessor = model->accessors[accessor_id];

	int key = accessor.componentType * (accessor.normalized ? -1 : 1);
	if (formats.find(key) == formats.end())
		return static_cast<int>(vk::Format::eUndefined);
	else if (formats.at(key).find(accessor.type) == formats.at(key).end())
		return static_cast<int>(vk::Format::eUndefined);

	return static_cast<int>(formats.at(key).at(accessor.type));
}

void SceneLoader::printSceneNodes(const Scene& scene)
{
	std::println("\n==================== Scene Nodes Tree ====================");
	std::println("Scene: {}", scene.getName());

	auto print_node_tree = [&](
	                           this auto&&        print_node_tree,
	                           Node*              node,
	                           const std::string& prefix = "",
	                           bool               is_last = true) {
		if (!node)
			return;

		std::string node_icon = is_last ? "└── " : "├── ";
		std::string child_prefix = prefix + (is_last ? "    " : "│   ");
		std::println("{}{}[{}] {} (ID: {})", prefix, node_icon, node->getType().name(), node->getName(), node->getId());

		if (node->hasComponent<Transform>()) {
			const auto& transform = node->getComponent<Transform>();
			const auto& pos = transform.getTranslation();
			const auto& scale = transform.getScale();
			std::println("{}├── Transform: pos({:.2f}, {:.2f}, {:.2f}) scale({:.2f}, {:.2f}, {:.2f})",
			             child_prefix, pos.x, pos.y, pos.z, scale.x, scale.y, scale.z);
		}

		if (node->hasComponent<Camera>()) {
			const auto& camera = node->getComponent<Camera>();
			std::println("{}├── Camera: {}", child_prefix, camera.getName());
		}

		if (node->hasComponent<Light>()) {
			const auto& light = node->getComponent<Light>();
			const auto& color = light.getColor();
			std::println("{}├── Light: {} - color({:.2f}, {:.2f}, {:.2f}) intensity({:.2f})",
			             child_prefix, light.getName(), color.r, color.g, color.b, light.getIntensity());
		}

		if (node->hasComponent<Mesh>()) {
			const auto& mesh = node->getComponent<Mesh>();
			std::println("{}├── SubMesh: {} - submeshes({})",
			             child_prefix, mesh.getName(), mesh.getSubmeshes().size());
		}

		const auto& children = node->getChildren();
		for (size_t i = 0; i < children.size(); ++i) {
			bool is_last_child = (i == children.size() - 1);
			print_node_tree(children[i], child_prefix, is_last_child);
		}
	};

	const Node& root = const_cast<Scene&>(scene).getRoot();
	if (root.getChildren().empty()) {
		std::println("└── (No child nodes)");
		return;
	}

	const auto& children = root.getChildren();
	for (size_t i = 0; i < children.size(); ++i) {
		bool is_last = (i == children.size() - 1);
		print_node_tree(children[i], "", is_last);
	}

	std::println("=========================================================\n");
	std::fflush(stdout);
}

void SceneLoader::printSceneComponents(const Scene& scene)
{
	std::println("\n================== Scene Components ==================");
	std::println("Scene: {}", scene.getName());

	auto get_component_type_name = [](const std::type_index& type) {
		std::string type_name = type.name();
		if (type_name.find("Transform") != std::string::npos)
			return "Transform";
		if (type_name.find("PerspectiveCamera") != std::string::npos)
			return "PerspectiveCamera";
		if (type_name.find("OrthoCamera") != std::string::npos)
			return "OrthoCamera";
		if (type_name.find("Camera") != std::string::npos)
			return "Camera";
		if (type_name.find("DirectionalLight") != std::string::npos)
			return "DirectionalLight";
		if (type_name.find("PointLight") != std::string::npos)
			return "PointLight";
		if (type_name.find("SpotLight") != std::string::npos)
			return "SpotLight";
		if (type_name.find("Light") != std::string::npos)
			return "Light";
		if (type_name.find("SubMesh") != std::string::npos)
			return "SubMesh";
		if (type_name.find("Mesh") != std::string::npos)
			return "Mesh";
		if (type_name.find("PBRMaterial") != std::string::npos)
			return "PBRMaterial";
		if (type_name.find("Material") != std::string::npos)
			return "Material";
		if (type_name.find("Texture") != std::string::npos)
			return "Texture";
		if (type_name.find("Image") != std::string::npos)
			return "Image";
		if (type_name.find("AABB") != std::string::npos)
			return "AABB";
		return "";
	};

	std::vector<std::type_index> component_types = {
	    typeid(Transform),
	    typeid(PerspectiveCamera),
	    typeid(OrthoCamera),
	    typeid(DirectionalLight),
	    typeid(PointLight),
	    typeid(SpotLight),
	    typeid(Mesh),
	    typeid(SubMesh),
	    typeid(PBRMaterial),
	    typeid(Material),
	    typeid(Texture),
	    typeid(AABB),
	};

	bool has_any_components = false;
	for (const auto& type : component_types) {
		if (scene.hasComponent(type)) {
			has_any_components = true;
			const auto& components = scene.getComponents(type);
			std::string type_name = get_component_type_name(type);

			std::println("├── {} ({})", type_name, components.size());

			for (size_t i = 0; i < components.size(); ++i) {
				const auto& component = components[i];
				bool        is_last = (i == components.size() - 1);
				std::string item_icon = is_last ? "│   └── " : "│   ├── ";

				std::println("{}[{}] {} (UID: {})", item_icon,
				             get_component_type_name(component->getType()),
				             component->getName(),
				             component->getUid());

				if (auto* camera = dynamic_cast<PerspectiveCamera*>(component.get())) {
					std::println("{}    FOV: {:.1f}°, Aspect: {:.2f}, Near: {:.2f}, Far: {:.2f}",
					             is_last ? "        " : "│       ",
					             glm::degrees(camera->getFov()), camera->getAspectRatio(),
					             camera->getNearPlane(), camera->getFarPlane());
				} else if (auto* light = dynamic_cast<DirectionalLight*>(component.get())) {
					const auto& color = light->getColor();
					const auto& dir = light->getDirection();
					std::println("{}    Color: ({:.2f}, {:.2f}, {:.2f}), Intensity: {:.2f}",
					             is_last ? "        " : "│       ",
					             color.r, color.g, color.b, light->getIntensity());
					std::println("{}    Direction: ({:.2f}, {:.2f}, {:.2f})",
					             is_last ? "        " : "│       ",
					             dir.x, dir.y, dir.z);
				} else if (auto* mesh = dynamic_cast<Mesh*>(component.get())) {
					std::println("{}    Submeshes: {}",
					             is_last ? "        " : "│       ",
					             mesh->getSubmeshes().size());
				} else if (auto* submesh = dynamic_cast<SubMesh*>(component.get())) {
					std::println("{}    Vertices: {}, Indices: {}, Visible: {}",
					             is_last ? "        " : "│       ",
					             submesh->getVerticesCount(), submesh->getIndicesCount(),
					             submesh->isVisible() ? "Yes" : "No");
				} else if (auto* material = dynamic_cast<PBRMaterial*>(component.get())) {
					const auto& base_color = material->getBaseColorFactor();
					std::println("{}    BaseColor: ({:.2f}, {:.2f}, {:.2f}, {:.2f})",
					             is_last ? "        " : "│       ",
					             base_color.r, base_color.g, base_color.b, base_color.a);
					std::println("{}    Metallic: {:.2f}, Roughness: {:.2f}",
					             is_last ? "        " : "│       ",
					             material->getMetallicFactor(), material->getRoughnessFactor());
				} else if (auto* texture = dynamic_cast<Texture*>(component.get())) {
					std::println("{}    Size: {}x{}, Format: {}, Data: {} bytes",
					             is_last ? "        " : "│       ",
					             texture->getWidth(), texture->getHeight(), texture->getFormat(),
					             texture->getData().size());
				}
			}
		}
	}

	if (!has_any_components) {
		std::println("└── (No components found)");
	}

	std::println("=====================================================\n");
	std::fflush(stdout);
}
