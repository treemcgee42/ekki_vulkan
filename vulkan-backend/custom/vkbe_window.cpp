//
// Created by Varun Malladi on 8/27/22.
//

#include "vulkan-backend/custom/vkbe_window.hpp"

namespace vkbe {

VkbeWindow::VkbeWindow(int w, int h, const std::string& windowName):
    width{w},
    height{h}
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(window, this);
    // TODO
    // glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

VkbeWindow::~VkbeWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

}
