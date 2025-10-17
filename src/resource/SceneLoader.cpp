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

	tinygltf::Scene* tfscene = &model.scenes.front();
	if (!tfscene)
		throw std::runtime_error("No default scene found in glTF file.");

	auto root_node = std::make_unique<Node>(0, tfscene->name);
	for (auto node_index : tfscene->nodes)
		traverse_nodes.push({std::ref(*root_node), node_index});

	while (!traverse_nodes.empty()) {
		auto node_it = traverse_nodes.front();
		traverse_nodes.pop();
		if (node_it.second >= nodes.size())
			continue;

		auto& current_node = *nodes.at(node_it.second);
		auto& traverse_root_node = node_it.first;
		current_node.setParent(traverse_root_node);
		traverse_root_node.addChild(current_node);

		for (auto child_node_index : model.nodes.at(node_it.second).children)
			traverse_nodes.push({std::ref(current_node), child_node_index});
	}

	scene->setRoot(*root_node);
	nodes.insert(nodes.begin(), std::move(root_node));
	scene->setNodes(std::move(nodes));

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

	if (const auto& translation = tfnode.translation; !translation.empty())
		transform.setTranslation(glm::vec3(translation[0], translation[1], translation[2]));

	if (const auto& rotation = tfnode.rotation; !rotation.empty())
		transform.setRotation(glm::quat(rotation[0], rotation[1], rotation[2], rotation[3]));

	if (const auto& scale = tfnode.scale; !scale.empty())
		transform.setScale(glm::vec3(scale[0], scale[1], scale[2]));

	if (const auto& matrix = tfnode.matrix; !tfnode.matrix.empty())
		transform.setMatrix(glm::mat4(
		    matrix[0], matrix[1], matrix[2], matrix[3],
		    matrix[4], matrix[5], matrix[6], matrix[7],
		    matrix[8], matrix[9], matrix[10], matrix[11],
		    matrix[12], matrix[13], matrix[14], matrix[15]));

	return node;
}

std::unique_ptr<Mesh> SceneLoader::parseMesh(const tinygltf::Mesh& tfmesh)
{
	auto mesh = std::make_unique<Mesh>(tfmesh.name);

	return mesh;
}

std::unique_ptr<SubMesh> SceneLoader::parseSubmesh(const tinygltf::Mesh& tfmesh, const tinygltf::Model& model, uint32_t index)
{
	auto submesh = std::make_unique<SubMesh>(std::format("{}_Primitive_{}", tfmesh.name, index));

	const auto& tfprimitive = tfmesh.primitives[index];

	// Load Vertices
	struct AttributeData {
		std::string              name;
		int                      format;
		uint32_t                 size;
		std::span<const uint8_t> data_view;
	};

	std::map<std::string, AttributeData> attributes_map;
	for (const auto& [attr_name, accessor_id] : tfprimitive.attributes) {
		auto upper_name = attr_name;
		std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(), ::toupper);
		if (std::find(attributes_names.begin(), attributes_names.end(), upper_name) == attributes_names.end())
			continue;

		attributes_map[upper_name] = AttributeData{
		    .name = upper_name,
		    .format = getAttributeFormat(&model, accessor_id),
		    .size = getAttributeSize(&model, accessor_id),
		    .data_view = getAttributeDataView(model, accessor_id),
		};
	}

	uint32_t vertex_count = getAttributeCount(&model, tfprimitive.attributes.begin()->second);
	uint32_t vertex_stride = 0;
	for (const auto& attribute_name : attributes_names) {
		if (!attributes_map.contains(attribute_name))
			continue;

		auto&    attribute = attributes_map.at(attribute_name);
		uint32_t offset = vertex_stride;

		submesh->setAttribute(attribute_name, {attribute.format, attribute.size, offset});
		vertex_stride += attribute.size;
	}

	std::vector<uint8_t> vertices_raw_data(vertex_count * vertex_stride);
	for (const auto& attribute_name : attributes_names) {
		if (!attributes_map.contains(attribute_name))
			continue;

		const auto& attribute = attributes_map.at(attribute_name);
		const auto& submesh_attribute = submesh->getAttribute(attribute_name);
		uint32_t    offset = submesh_attribute->offset;

		for (uint32_t vertex_index = 0; vertex_index < vertex_count; vertex_index++) {
			std::memcpy(&vertices_raw_data[vertex_index * vertex_stride + offset],
			            &attribute.data_view[vertex_index * attribute.size],
			            attribute.size);
		}
	}

	std::vector<float> vertices_data(vertices_raw_data.size() / sizeof(float));
	std::memcpy(vertices_data.data(), vertices_raw_data.data(), vertices_raw_data.size());
	submesh->setVertices(std::move(vertices_data), vertex_count);

	// Load Indices
	if (tfprimitive.indices >= 0) {
		auto indices_raw_data = getAttributeDataView(model, tfprimitive.indices);
		auto index_byte_size = getAttributeSize(&model, tfprimitive.indices);

		std::vector<uint32_t> indices_data;
		switch (index_byte_size) {
		case 1:
			indices_data = convertData<uint8_t, uint32_t>(indices_raw_data);
			break;
		case 2:
			indices_data = convertData<uint16_t, uint32_t>(indices_raw_data);
			break;
		case 4:
			indices_data = convertData<uint32_t, uint32_t>(indices_raw_data);
			break;
		default:
			throw std::runtime_error("Unsupported index byte size");
		}

		submesh->setIndices(std::move(indices_data));
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

std::unique_ptr<Texture> SceneLoader::parseTexture(const tinygltf::Texture& tftexture)
{
	return std::make_unique<Texture>(tftexture.name);
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

std::span<const uint8_t> SceneLoader::getAttributeDataView(const tinygltf::Model& model, uint32_t accessor_index)
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

uint32_t SceneLoader::getAttributeCount(const tinygltf::Model* model, uint32_t accessor_id)
{
	assert(accessor_id < model->accessors.size());

	return static_cast<uint32_t>(model->accessors[accessor_id].count);
}

uint32_t SceneLoader::getAttributeSize(const tinygltf::Model* model, uint32_t accessor_id)
{
	assert(accessor_id < model->accessors.size());
	auto& accessor = model->accessors[accessor_id];

	size_t component_size = tinygltf::GetComponentSizeInBytes(accessor.componentType);
	size_t component_num = tinygltf::GetNumComponentsInType(accessor.type);

	return static_cast<uint32_t>(component_size * component_num);
}

uint32_t SceneLoader::getAttributeStride(const tinygltf::Model* model, uint32_t accessor_id)
{
	assert(accessor_id < model->accessors.size());
	auto& accessor = model->accessors[accessor_id];

	assert(accessor.bufferView < model->bufferViews.size());
	auto& buffer_view = model->bufferViews[accessor.bufferView];

	return static_cast<uint32_t>(accessor.ByteStride(buffer_view));
}

int SceneLoader::getAttributeFormat(const tinygltf::Model* model, uint32_t accessor_id)
{
	assert(accessor_id < model->accessors.size());
	auto& accessor = model->accessors[accessor_id];

	int key = accessor.componentType * (accessor.normalized ? -1 : 1);
	if (formats_map.find(key) == formats_map.end())
		return static_cast<int>(vk::Format::eUndefined);
	else if (formats_map.at(key).find(accessor.type) == formats_map.at(key).end())
		return static_cast<int>(vk::Format::eUndefined);

	return static_cast<int>(formats_map.at(key).at(accessor.type));
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

#include <fstream>
#include <iomanip>

void SceneLoader::exportSubmeshToOBJ(const SubMesh& submesh, const std::string& filepath)
{
	std::ofstream file(filepath);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file for writing: " + filepath);
	}

	file << "# OBJ file exported from SubMesh: " << submesh.getName() << "\n";
	file << "# Vertices: " << submesh.getVerticesCount() << "\n";
	file << "# Indices: " << submesh.getIndicesCount() << "\n\n";

	const auto& vertex_data = submesh.getVertices();
	const auto& attributes = submesh.getAttributes();
	uint32_t    vertex_count = submesh.getVerticesCount();
	uint32_t    vertex_stride = 0;
	for (const auto& [name, attr] : attributes) {
		vertex_stride += attr.size;
	}

	bool     has_position = false;
	bool     has_normal = false;
	bool     has_texcoord = false;
	uint32_t pos_offset = 0;
	uint32_t normal_offset = 0;
	uint32_t texcoord_offset = 0;

	for (const auto& [name, attr] : attributes) {
		if (name == "POSITION") {
			pos_offset = attr.offset;
			has_position = true;
		} else if (name == "NORMAL") {
			normal_offset = attr.offset;
			has_normal = true;
		} else if (name == "TEXCOORD_0") {
			texcoord_offset = attr.offset;
			has_texcoord = true;
		}
	}

	if (!has_position) {
		throw std::runtime_error("SubMesh must have POSITION attribute to export to OBJ");
	}

	const uint8_t* data_ptr = reinterpret_cast<const uint8_t*>(vertex_data.data());

	file << "# Vertex positions\n";
	for (uint32_t i = 0; i < vertex_count; ++i) {
		const float* pos = reinterpret_cast<const float*>(data_ptr + i * vertex_stride + pos_offset);
		file << "v " << std::fixed << std::setprecision(6)
		     << pos[0] << " " << pos[1] << " " << pos[2] << "\n";
	}
	file << "\n";

	if (has_texcoord) {
		file << "# Texture coordinates\n";
		for (uint32_t i = 0; i < vertex_count; ++i) {
			const float* uv = reinterpret_cast<const float*>(data_ptr + i * vertex_stride + texcoord_offset);
			file << "vt " << std::fixed << std::setprecision(6)
			     << uv[0] << " " << uv[1] << "\n";
		}
		file << "\n";
	}

	if (has_normal) {
		file << "# Vertex normals\n";
		for (uint32_t i = 0; i < vertex_count; ++i) {
			const float* normal = reinterpret_cast<const float*>(data_ptr + i * vertex_stride + normal_offset);
			file << "vn " << std::fixed << std::setprecision(6)
			     << normal[0] << " " << normal[1] << " " << normal[2] << "\n";
		}
		file << "\n";
	}

	const auto& indices = submesh.getIndices();
	if (!indices.empty()) {
		file << "# Faces\n";
		for (size_t i = 0; i < indices.size(); i += 3) {
			// OBJ索引从1开始
			uint32_t i0 = indices[i] + 1;
			uint32_t i1 = indices[i + 1] + 1;
			uint32_t i2 = indices[i + 2] + 1;

			file << "f ";
			if (has_texcoord && has_normal) {
				// v/vt/vn 格式
				file << i0 << "/" << i0 << "/" << i0 << " "
				     << i1 << "/" << i1 << "/" << i1 << " "
				     << i2 << "/" << i2 << "/" << i2;
			} else if (has_texcoord) {
				// v/vt 格式
				file << i0 << "/" << i0 << " "
				     << i1 << "/" << i1 << " "
				     << i2 << "/" << i2;
			} else if (has_normal) {
				// v//vn 格式
				file << i0 << "//" << i0 << " "
				     << i1 << "//" << i1 << " "
				     << i2 << "//" << i2;
			} else {
				// 仅v格式
				file << i0 << " " << i1 << " " << i2;
			}
			file << "\n";
		}
	}

	file.close();
	std::println("Exported SubMesh to OBJ: {}", filepath);
	std::fflush(stdout);
}

void SceneLoader::printSubmeshInfo(const SubMesh& submesh)
{
	std::println("\n=============== SubMesh Info ===============");
	std::println("Name: {}", submesh.getName());
	std::println("Vertices: {}", submesh.getVerticesCount());
	std::println("Indices: {}", submesh.getIndicesCount());
	std::println("Visible: {}", submesh.isVisible() ? "Yes" : "No");

	const auto& attributes = submesh.getAttributes();
	uint32_t    vertex_stride = 0;
	for (const auto& [name, attr] : attributes) {
		vertex_stride += attr.size;
	}

	std::println("\nVertex stride: {} bytes", vertex_stride);
	std::println("\nAttributes:");
	for (const auto& [name, attr] : attributes) {
		std::println("  {:12} - offset: {:3}, size: {:2} bytes, format: {}",
		             name, attr.offset, attr.size, attr.format);
	}

	const auto&    vertex_data = submesh.getVertices();
	const uint8_t* data_ptr = reinterpret_cast<const uint8_t*>(vertex_data.data());
	uint32_t       print_count = std::min(3u, submesh.getVerticesCount());

	std::println("\nFirst {} vertices:", print_count);
	for (uint32_t i = 0; i < print_count; ++i) {
		std::print("  Vertex {}: ", i);

		for (const auto& [name, attr] : attributes) {
			const float* values = reinterpret_cast<const float*>(
			    data_ptr + i * vertex_stride + attr.offset);
			uint32_t component_count = attr.size / sizeof(float);

			std::print("{:12}(", name);
			for (uint32_t j = 0; j < component_count; ++j) {
				std::print("{:7.3f}{}", values[j], j < component_count - 1 ? ", " : "");
			}
			std::print(") ");
		}
		std::println("");
	}

	const auto& indices = submesh.getIndices();
	if (!indices.empty()) {
		uint32_t face_count = std::min(3u, static_cast<uint32_t>(indices.size() / 3));
		std::println("\nFirst {} faces:", face_count);
		for (uint32_t i = 0; i < face_count * 3; i += 3) {
			std::println("  Face {}: {} {} {}", i / 3, indices[i], indices[i + 1], indices[i + 2]);
		}
	}

	std::println("==========================================\n");
	std::fflush(stdout);
}

void SceneLoader::printSubmeshDetailed(const SubMesh& submesh)
{
	std::println("\n╔════════════════════════════════════════════╗");
	std::println("║          SubMesh Detailed Info             ║");
	std::println("╚════════════════════════════════════════════╝");

	std::println("\n Basic Info:");
	std::println("  Name:     {}", submesh.getName());
	std::println("  Vertices: {}", submesh.getVerticesCount());
	std::println("  Indices:  {}", submesh.getIndicesCount());
	std::println("  Visible:  {}", submesh.isVisible() ? "✓" : "✗");

	const auto& attributes = submesh.getAttributes();
	uint32_t    vertex_stride = 0;
	for (const auto& [name, attr] : attributes) {
		vertex_stride += attr.size;
	}

	std::println("\n📐 Vertex Layout (stride: {} bytes):", vertex_stride);
	std::println("  ┌─────────────┬────────┬────────┬──────────┐");
	std::println("  │ Attribute   │ Offset │ Size   │ Format   │");
	std::println("  ├─────────────┼────────┼────────┼──────────┤");

	for (const auto& [name, attr] : attributes) {
		std::println("  │ {:11} │ {:6} │ {:6} │ {:8} │",
		             name, attr.offset, attr.size, attr.format);
	}
	std::println("  └─────────────┴────────┴────────┴──────────┘");
	std::println("\n️  Memory Layout per Vertex:");
	std::print("  Bytes: ");
	for (uint32_t i = 0; i < vertex_stride; i++) {
		if (i % 4 == 0)
			std::print("│");
		std::print("{:2}", i % 100);
		if (i < vertex_stride - 1)
			std::print(" ");
	}
	std::println("│");

	for (const auto& [name, attr] : attributes) {
		std::print("         ");
		for (uint32_t i = 0; i < vertex_stride; i++) {
			if (i % 4 == 0)
				std::print("│");
			if (i >= attr.offset && i < attr.offset + attr.size) {
				std::print("██");
			} else {
				std::print("  ");
			}
			if (i < vertex_stride - 1)
				std::print(" ");
		}
		std::println("│ {}", name);
	}

	const auto&    vertex_data = submesh.getVertices();
	const uint8_t* data_ptr = reinterpret_cast<const uint8_t*>(vertex_data.data());
	uint32_t       print_count = std::min(3u, submesh.getVerticesCount());

	std::println("\n📊 Vertex Data (first {} vertices):", print_count);
	for (uint32_t v = 0; v < print_count; ++v) {
		std::println("  Vertex {}:", v);
		for (const auto& [name, attr] : attributes) {
			const float* values = reinterpret_cast<const float*>(
			    data_ptr + v * vertex_stride + attr.offset);
			uint32_t component_count = attr.size / sizeof(float);

			std::print("    {:12}: (", name);
			for (uint32_t c = 0; c < component_count; ++c) {
				std::print("{:8.4f}{}", values[c], c < component_count - 1 ? ", " : "");
			}
			std::println(")");
		}
	}

	const auto& indices = submesh.getIndices();
	if (!indices.empty()) {
		uint32_t face_count = std::min(3u, static_cast<uint32_t>(indices.size() / 3));
		std::println("\n🔺 Index Data (first {} triangles):", face_count);
		for (uint32_t i = 0; i < face_count; ++i) {
			uint32_t idx = i * 3;
			std::println("  Triangle {}: [{:3}, {:3}, {:3}]",
			             i, indices[idx], indices[idx + 1], indices[idx + 2]);
		}
	}

	std::println("\n════════════════════════════════════════════\n");
	std::fflush(stdout);
}
