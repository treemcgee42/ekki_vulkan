//
// Created by Varun Malladi on 8/23/22.
//

#include "meshes.hpp"

namespace eklib {

Meshes::Meshes(vkbe::VkbeDevice& vkbe_device_): vkbe_device{vkbe_device_}
{
    library = std::make_unique<std::map<int, std::shared_ptr<vkbe::VkbeModel>>>();

    init_triangle_mesh();
}

std::shared_ptr<vkbe::VkbeModel> Meshes::get_mesh(int mesh_kind) {
    return (*library)[mesh_kind];
}

void Meshes::init_triangle_mesh() {
    std::vector<eklib::Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                           {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                           {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

    auto lveModel = std::make_shared<vkbe::VkbeModel>(vkbe_device, vertices);

    library->insert({0, lveModel});
}

}
