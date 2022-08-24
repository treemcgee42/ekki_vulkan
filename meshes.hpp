//
// Created by Varun Malladi on 8/23/22.
//

#pragma once

#include <map>
#include <vector>
#include "vulkan-backend/lve_model.hpp"
#include "vulkan-backend/lve_device.hpp"


namespace eklib {

class Meshes {
public:
    Meshes(lve::LveDevice& device);
    ~Meshes() = default;

    std::shared_ptr<lve::LveModel> get_mesh(const char* mesh_kind);

private:
    lve::LveDevice& device;
    std::map<std::string, std::shared_ptr<lve::LveModel>> library;

    void init_triangle_mesh();
};

}
