//
// Created by Varun Malladi on 8/23/22.
//

#include "meshes.hpp"

namespace eklib {

Meshes::Meshes(lve::LveDevice& device): device{device} {
    init_triangle_mesh();
}

std::shared_ptr<lve::LveModel> Meshes::get_mesh(const char* mesh_kind) {
    return library[mesh_kind];
}

void Meshes::init_triangle_mesh() {
    std::vector<lve::LveModel::Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                           {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                           {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

    auto lveModel = std::make_shared<lve::LveModel>(device, vertices);

    library.insert({"triangle", lveModel});
}

}
