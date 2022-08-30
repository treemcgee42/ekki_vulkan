//
// Created by Varun Malladi on 8/27/22.
//

#include "vulkan-backend/include/vkbe_window.hpp"

namespace vkbe {

void VkbeWindow::resize_callback(GLFWwindow *window, int width, int height) {
    auto vkbe_window = reinterpret_cast<VkbeWindow *>(glfwGetWindowUserPointer(window));
    vkbe_window->resized = true;
    vkbe_window->width = width;
    vkbe_window->height = height;
}

VkbeWindow::VkbeWindow(int w, int h, const std::string& windowName):
    width{w},
    height{h}
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, resize_callback);
}

VkbeWindow::~VkbeWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

}
