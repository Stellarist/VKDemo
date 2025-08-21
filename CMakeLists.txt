cmake_minimum_required(VERSION 4.0)

project(VKDemo)

add_compile_definitions(SOURCE_DIR="${CMAKE_SOURCE_DIR}")
add_compile_definitions(SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders")
add_compile_definitions(SCRIPTS_DIR="${CMAKE_SOURCE_DIR}/scripts")

include_directories(src)

file(GLOB INC_LIST src/*.hpp)
file(GLOB SRC_LIST src/*.cpp)
file(GLOB SHADER_LIST shaders/*.slang)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SRC_LIST})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files" FILES ${INC_LIST})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/shaders PREFIX "Shader Files" FILES ${SHADER_LIST})

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_executable(VKDemo
	${INC_LIST} 
	${SRC_LIST}
)

target_link_libraries(VKDemo
	Vulkan::Vulkan
	SDL3::SDL3
)

add_custom_target(compile_shaders
	COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/compile_shaders.py
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS ${SHADER_LIST}
)

add_dependencies(VKDemo compile_shaders)
