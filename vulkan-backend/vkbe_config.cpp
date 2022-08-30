//
// Created by Varun Malladi on 8/29/22.
//

#include "vulkan-backend/include/vkbe_config.hpp"

namespace vkbe {

void vkbe_check_vk_result_panic(VkResult result, const char *msg) {
    if (result == VK_SUCCESS) { return; }

    std::ostringstream error_msg;
    error_msg << "[vkbe::Vulkan] error: VkResult = " << result << ". " << msg << "\n";

    throw std::runtime_error(error_msg.str());
}

}