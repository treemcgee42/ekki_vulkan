//
// Created by Varun Malladi on 8/27/22.
//

#pragma once

//#define GLFW_INCLUDE_VULKAN
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
private:
    int height;
    int width;
};

}