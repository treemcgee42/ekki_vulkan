//
// Created by Varun Malladi on 8/27/22.
//

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace vkbe {

/**
 * Encapsulates GLFW window creation, and holds the resulting data. Also captures any
 * callbacks from GLFW, such as when the window is resized.
 */
class VkbeWindow {
public:
    GLFWwindow *window;

    VkbeWindow(int w, int h, const std::string& name);
    ~VkbeWindow();

    VkbeWindow(const VkbeWindow &) = delete;
    VkbeWindow &operator=(const VkbeWindow &) = delete;

    VkExtent2D get_extent() const { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
    bool was_resized() { return resized; }
    void reset_resized_flag() { resized = false; }
    bool should_close() { return glfwWindowShouldClose(window); }
private:
    int height;
    int width;
    bool resized = false;

    static void resize_callback(GLFWwindow *window, int width, int height);
};

}