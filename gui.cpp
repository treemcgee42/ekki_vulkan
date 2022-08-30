



#include "gui.hpp"

#include <vector>
#include <stdexcept>
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>

void Gui::init_imgui(eklib::Engine& engine) {
    //1: create descriptor pool for IMGUI
    // the size of the pool is very oversize, but it's copied from imgui demo itself.
    VkDescriptorPoolSize pool_sizes[] =
            {
                    { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
            };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VkDescriptorPool imguiPool;
    if (vkCreateDescriptorPool(engine.vkbe_device.get_logical_device(), &pool_info, nullptr, &imguiPool) != VK_SUCCESS) {
        throw std::runtime_error("couldn't create imgui descriptor pool");
    }


    // 2: initialize imgui library

    //this initializes the core structures of imgui
    ImGui::CreateContext();

    auto io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsLight();

    //this initializes imgui for SDL
    //ImGui_ImplSDL2_InitForVulkan(_window);
    ImGui_ImplGlfw_InitForVulkan(engine.vkbe_window.window, true);

    //this initializes imgui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = engine.vkbe_device.get_instance();
    init_info.PhysicalDevice = engine.vkbe_device.get_physical_device();
    init_info.Device = engine.vkbe_device.get_logical_device();
    init_info.Queue = engine.vkbe_device.get_graphics_queue();
    init_info.DescriptorPool = imguiPool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, engine.vkbe_renderer.get_render_pass());

    std::cout << "before immediate submit\n";

    //execute a gpu command to upload imgui font textures
//    engine.immediate_submit([&](VkCommandBuffer cmd) {
//        ImGui_ImplVulkan_CreateFontsTexture(cmd);
//    });

    std::cout << "after immediate submit\n";

    //clear font textures from cpu data
    ImGui_ImplVulkan_DestroyFontUploadObjects();

    imgui_descriptor_pool = imguiPool;

    //add the destroy the imgui created structures
//    _mainDeletionQueue.push_function([=]() {
//
//        vkDestroyDescriptorPool(_device, imguiPool, nullptr);
//        ImGui_ImplVulkan_Shutdown();
//    });
}

