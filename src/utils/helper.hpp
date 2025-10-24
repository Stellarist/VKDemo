#pragma once

#include <print>
#include <fstream>
#include <iomanip>

#include "scene/base/Scene.hpp"
#include "scene/base/Node.hpp"
#include "scene/components/Mesh.hpp"
#include "scene/components/Camera.hpp"
#include "scene/components/Material.hpp"
#include "scene/components/Light.hpp"

inline void printSceneNodes(const Scene& scene)
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

inline void printSceneComponents(const Scene& scene)
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

inline void exportSubmeshToOBJ(const SubMesh& submesh, const std::string& filepath)
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

inline void printSubmeshInfo(const SubMesh& submesh)
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

inline void printSubmeshDetailed(const SubMesh& submesh)
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
