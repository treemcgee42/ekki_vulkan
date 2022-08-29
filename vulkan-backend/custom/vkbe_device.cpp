//
// Created by Varun Malladi on 8/28/22.
//

#include "vkbe_device.hpp"
#include "vkbe_swap_chain.hpp"
#include <iostream>
#include <set>

namespace vkbe {

// ------------------------------------------------------------------
// ----- Debug helpers ----------------------------------------------
// ------------------------------------------------------------------

/**
 * Checks if the debug validation layers, specified in `vkbe_config.hpp`, are available in
 * the current Vulkan package.
 */
bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : VKBE_CONFIG_VALIDATION_LAYERS) {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData)
{
    std::cerr << "[vkbe::Vulkan] validation layer: " << pCallbackData->pMessage << "\n";

    return VK_FALSE;
}

/**
 * Convenience because we need to make this both during instance creation and when actually
 * setting up the debug messenger. The two configurations must correspond, hence the utility
 * of this function.
 */
void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& debug_create_info) {
    debug_create_info = {};
    debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_create_info.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_create_info.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_create_info.pfnUserCallback = debugCallback;
    debug_create_info.pUserData = nullptr;
}

VkResult create_debug_utils_messenger_ext(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func =
            (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VkbeDevice::setup_debug_messenger() {
    if (!VKBE_CONFIG_ENABLE_VALIDATION_LAYERS) { return; }

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populate_debug_messenger_create_info(createInfo);

    vkbe_check_vk_result_panic(
        create_debug_utils_messenger_ext(instance, &createInfo, nullptr, &debug_messenger),
        "failed to set up debug messenger!"
    );
}

// ------------------------------------------------------------------------
// ----- Helpers for core functions ---------------------------------------
// ------------------------------------------------------------------------

QueueFamilyInfo populate_queue_family_info(VkPhysicalDevice device, VkSurfaceKHR& surface) {
    QueueFamilyInfo indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        device,
        &queueFamilyCount,
        nullptr
    );

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        device,
        &queueFamilyCount,
        queueFamilies.data()
    );

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_queue_family_index = i;
            indices.graphics_index_valid = true;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            device,
            i,
            surface,
            &presentSupport
        );

        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.present_queue_family_index = i;
            indices.present_index_valid = true;
        }

        if (indices.all_queues_valid()) {
            break;
        }

        i++;
    }

    return indices;
}

bool check_device_extension_support(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &extensionCount,
        nullptr
    );

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &extensionCount,
        availableExtensions.data()
    );

    std::set<std::string> requiredExtensions(VKBE_CONFIG_DEVICE_EXTENSIONS.begin(), VKBE_CONFIG_DEVICE_EXTENSIONS.end());

    for (const auto &extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

bool VkbeDevice::is_device_suitable(VkPhysicalDevice device) {
    queue_family_info = populate_queue_family_info(device, surface);

    bool extensionsSupported = check_device_extension_support(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        auto swapChainSupport = DeviceSwapChainSupportDetails::query(device, surface);
        swapChainAdequate = !swapChainSupport.surface_formats.empty() && !swapChainSupport.present_modes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return queue_family_info.all_queues_valid() && extensionsSupported && swapChainAdequate &&
           supportedFeatures.samplerAnisotropy;
}

/**
 * Puts in the required GLFW extensions and takes the rest from `vkbe_config.hpp`.
 */
std::vector<const char*> VkbeDevice::get_required_instance_extensions() {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    for (auto extension : VKBE_CONFIG_INSTANCE_EXTENSIONS) {
        extensions.push_back(extension);
    }

    return std::move(extensions);
}

// -------------------------------------------------------------------------
// ----- Core implementations ----------------------------------------------
// -------------------------------------------------------------------------

/**
 * Creating the Vulkan instance means specifying validation layers, instance extensions
 * (including those from GLFW) and tell it about the debugging interface we will set up
 * later.
 */
void VkbeDevice::create_vulkan_instance() {
    if (VKBE_CONFIG_ENABLE_VALIDATION_LAYERS && !checkValidationLayerSupport()) {
        throw std::runtime_error("[vkbe] validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = VKBE_CONFIG_APP_NAME.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(
            VKBE_CONFIG_APP_VERSION[0],
            VKBE_CONFIG_APP_VERSION[1],
            VKBE_CONFIG_APP_VERSION[2]
    );
    appInfo.pEngineName = VKBE_CONFIG_APP_ENGINE_NAME.c_str();
    appInfo.engineVersion = VK_MAKE_VERSION(
            VKBE_CONFIG_APP_ENGINE_VERSION[0],
            VKBE_CONFIG_APP_ENGINE_VERSION[1],
            VKBE_CONFIG_APP_ENGINE_VERSION[2]
    );
    appInfo.apiVersion = VKBE_CONFIG_API_VERSION;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    // TODO: this might only be necessary for macOS
    createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    auto extensions = get_required_instance_extensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    if (VKBE_CONFIG_ENABLE_VALIDATION_LAYERS) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VKBE_CONFIG_VALIDATION_LAYERS.size());
        createInfo.ppEnabledLayerNames = VKBE_CONFIG_VALIDATION_LAYERS.data();

        populate_debug_messenger_create_info(debug_create_info);

        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debug_create_info;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    vkbe_check_vk_result_panic(
        vkCreateInstance(&createInfo, nullptr, &instance),
        "failed to create Vulkan instance!"
    );
}

void VkbeDevice::create_vulkan_surface(GLFWwindow *window) {
    vkbe_check_vk_result_panic(
        glfwCreateWindowSurface(instance, window, nullptr, &surface),
        "failed to create window surface!"
    );
}

/**
 * There are a few things we require of a physical device. We need it to support the
 * queues laid out in the `QueueFamilyInfo` struct, that it supports the device
 * extensions specificed in `vkbe_config.hpp`, and is able of creating a basic swap
 * chain.
 *
 * TODO: create logic to select the "best" device, rather than just the first supported one
 */
void VkbeDevice::pick_physical_device() {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
    if (device_count == 0) {
        throw std::runtime_error("[vkbe::Vulkan] failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    for (const auto &device : devices) {
        if (is_device_suitable(device)) {
            physical_device = device;
            break;
        }
    }

    if (physical_device == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
}

void VkbeDevice::create_logical_device() {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {queue_family_info.graphics_queue_family_index, queue_family_info.present_queue_family_index};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(VKBE_CONFIG_DEVICE_EXTENSIONS.size());
    createInfo.ppEnabledExtensionNames = VKBE_CONFIG_DEVICE_EXTENSIONS.data();

    // might not really be necessary anymore because device specific validation layers
    // have been deprecated
    if (VKBE_CONFIG_ENABLE_VALIDATION_LAYERS) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VKBE_CONFIG_VALIDATION_LAYERS.size());
        createInfo.ppEnabledLayerNames = VKBE_CONFIG_VALIDATION_LAYERS.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physical_device, &createInfo, nullptr, &logical_device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkbe_check_vk_result_panic(
        vkCreateDevice(physical_device, &createInfo, nullptr, &logical_device),
        "failed to create logical device!"
    );
}

void VkbeDevice::get_queues_from_logical_device() {
    vkGetDeviceQueue(logical_device, queue_family_info.graphics_queue_family_index, 0, &graphics_queue);
    vkGetDeviceQueue(logical_device, queue_family_info.present_queue_family_index, 0, &present_queue);
}

void VkbeDevice::create_command_pool() {
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queue_family_info.graphics_queue_family_index;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    vkbe_check_vk_result_panic(
        vkCreateCommandPool(logical_device, &poolInfo, nullptr, &command_pool),
        "failed to create command pool!"
    );
}

VkbeDevice::VkbeDevice(VkbeWindow& vkbe_window) {
    create_vulkan_instance();
    setup_debug_messenger();
    create_vulkan_surface(vkbe_window.window);
    pick_physical_device();
    create_logical_device();
    get_queues_from_logical_device();
    // TODO: should this be happening here?
    create_command_pool();
}

// --------------------------------------------------------------------------------
// ----- Helpers for public functions ---------------------------------------------
// --------------------------------------------------------------------------------

uint32_t find_memory_type(VkPhysicalDevice physical_device, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

// --------------------------------------------------------------------------------
// ----- Public functions ---------------------------------------------------------
// --------------------------------------------------------------------------------

/**
 * Checks if the device supports any of the provided features. This will not pick the most
 * optimized format, merely the first that is supported. One could work around this by placing
 * the ideal formats at the front of the list of candidates.
 */
VkFormat VkbeDevice::find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physical_device, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
                   (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

void VkbeDevice::create_image(
    const VkImageCreateInfo &imageInfo,
    VkMemoryPropertyFlags properties,
    VkImage &image,
    VkDeviceMemory &imageMemory) const
{
    vkbe_check_vk_result_panic(
        vkCreateImage(logical_device, &imageInfo, nullptr, &image),
        "failed to create image!"
    );

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(logical_device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = find_memory_type(physical_device, memRequirements.memoryTypeBits, properties);

    vkbe_check_vk_result_panic(
        vkAllocateMemory(logical_device, &allocInfo, nullptr, &imageMemory),
        "failed to allocate image memory!"
    );

    vkbe_check_vk_result_panic(
            vkBindImageMemory(logical_device, image, imageMemory, 0),
            "failed to bind image memory!"
    );
}

}