//
// Created by Varun Malladi on 8/23/22.
//

#pragma once

#include <map>
#include <vector>
#include "vkbe_device.hpp"
#include "vkbe_model.hpp"

namespace eklib {

class Meshes {
public:
    //Meshes() = default;
    explicit Meshes(vkbe::VkbeDevice& vkbe_device_);
    ~Meshes() = default;

    std::shared_ptr<vkbe::VkbeModel> get_mesh(int mesh_kind);

private:
    vkbe::VkbeDevice& vkbe_device;
    std::unique_ptr<std::map<int, std::shared_ptr<vkbe::VkbeModel>>> library;

    void init_triangle_mesh();
};

}
