cmake_minimum_required(VERSION 4.0)

project(VKEngine)

file(GLOB_RECURSE SRC_LIST src/*.cpp)
file(GLOB_RECURSE INC_LIST src/*.hpp)
file(GLOB_RECURSE SHADER_LIST shaders/*.slang)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SRC_LIST})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files" FILES ${INC_LIST})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/shaders PREFIX "Shader Files" FILES ${SHADER_LIST})

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_executable(VKEngine
	${SRC_LIST}
	${INC_LIST}
)

target_include_directories(VKEngine
	PRIVATE src
)

target_compile_definitions(VKEngine
    PRIVATE
        SOURCE_DIR="${CMAKE_SOURCE_DIR}"
        SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders"
        ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets"
        SCRIPTS_DIR="${CMAKE_SOURCE_DIR}/scripts"
        GLM_ENABLE_EXPERIMENTAL
)

target_link_libraries(VKEngine
	Vulkan::Vulkan
	SDL3::SDL3
	glm::glm
	imgui::imgui
)

add_custom_target(compile_shaders
	COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/compile_shaders.py
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS ${SHADER_LIST}
)

add_dependencies(VKEngine 
	compile_shaders
)
