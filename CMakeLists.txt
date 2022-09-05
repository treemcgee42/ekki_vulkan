cmake_minimum_required(VERSION 3.22)
project(ekki)

set(CMAKE_CXX_STANDARD 17)

SET(VULKAN_SDK_PATH /Users/ogmalladii/VulkanSDK/1.3.216.0/macOS)

SET(CMAKE_MACOSX_RPATH true)
SET(CMAKE_INSTALL_RPATH ${VULKAN_SDK_PATH})
SET(CMAKE_BUILD_WITH_INSTALL_RPATH true)

include_directories(${VULKAN_SDK_PATH}/include /usr/local/include)
include_directories(${CMAKE_SOURCE_DIR})
include_directories(src/vulkan-backend/include)
include_directories(libs/imgui/include)
include_directories(libs/vma/include)

file(GLOB SOURCES
        src/main.cpp
        libs/imgui/include/*.h
        libs/imgui/*.cpp
        libs/vma/vma_implementation.cpp
        src/vulkan-backend/include/*.hpp
        src/vulkan-backend/*.cpp
        src/include/animation.hpp
        src/animation.cpp
        src/include/constants.hpp
        src/include/triangle.hpp
        src/triangle.cpp
        src/include/scene.hpp
        src/scene.cpp
        src/engine.cpp
        src/include/engine.hpp
        src/include/types.hpp
        src/types.cpp
        src/meshes.cpp
        src/include/meshes.hpp
        src/include/gui.hpp
        src/gui.cpp
)
add_executable(ekki ${SOURCES})
set_target_properties(
        ekki
        PROPERTIES
            LINK_FLAGS "-Wl,-rpath,${VULKAN_SDK_PATH}/lib -L${VULKAN_SDK_PATH}/lib -L/usr/local/lib"
)
target_link_libraries(ekki glfw.3.3 vulkan.1 vulkan.1.3.216)