cmake_minimum_required(VERSION 3.22)
project(ekki)

set(CMAKE_CXX_STANDARD 17)

SET(VULKAN_SDK_PATH /Users/ogmalladii/VulkanSDK/1.3.216.0/macOS)

SET(CMAKE_MACOSX_RPATH true)
SET(CMAKE_INSTALL_RPATH ${VULKAN_SDK_PATH})
SET(CMAKE_BUILD_WITH_INSTALL_RPATH true)

include_directories(${VULKAN_SDK_PATH}/include /usr/local/include)
include_directories(${CMAKE_SOURCE_DIR})
include_directories(libs/imgui/include)

file(GLOB SOURCES
    main.cpp
        libs/imgui/imgui.cpp
        libs/imgui/imgui_demo.cpp
        libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_impl_glfw.cpp
        libs/imgui/imgui_impl_vulkan.cpp
        libs/imgui/imgui_tables.cpp
        libs/imgui/imgui_widgets.cpp
        libs/imgui/include/imconfig.h
        libs/imgui/include/imgui.h
        libs/imgui/include/imgui_impl_glfw.h
        libs/imgui/include/imgui_impl_vulkan.h
        libs/imgui/include/imgui_internal.h
        libs/imgui/include/imstb_rectpack.h
        libs/imgui/include/imstb_textedit.h
        libs/imgui/include/imstb_truetype.h
        vulkan-backend/first_app.hpp
        vulkan-backend/first_app.cpp
        vulkan-backend/lve_device.hpp
        vulkan-backend/lve_device.cpp
        vulkan-backend/lve_game_object.hpp
        vulkan-backend/lve_model.hpp
        vulkan-backend/lve_model.cpp
        vulkan-backend/lve_pipeline.hpp
        vulkan-backend/lve_pipeline.cpp
        vulkan-backend/lve_renderer.hpp
        vulkan-backend/lve_renderer.cpp
        vulkan-backend/lve_swap_chain.hpp
        vulkan-backend/lve_swap_chain.cpp
        vulkan-backend/lve_window.hpp
        vulkan-backend/lve_window.cpp
        vulkan-backend/simple_render_system.hpp
        vulkan-backend/simple_render_system.cpp
        animation.hpp
        animation.cpp
        constants.hpp
        triangle.hpp
        triangle.cpp
        scene.hpp
        scene.cpp
        engine.cpp
        engine.hpp
        meshes.cpp
        meshes.hpp
        gui.hpp
        gui.cpp
)
add_executable(ekki ${SOURCES})
set_target_properties(
        ekki
        PROPERTIES
            LINK_FLAGS "-Wl,-rpath,${VULKAN_SDK_PATH}/lib -L${VULKAN_SDK_PATH}/lib -L/usr/local/lib"
)
target_link_libraries(ekki glfw.3.3 vulkan.1 vulkan.1.3.216)