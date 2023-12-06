#include "Voxel.hpp"

namespace GLOO {
    Voxel::Voxel() : type(VoxelType::Air) {}

    Voxel::Voxel(VoxelType type) : type(type) {}

    VoxelType Voxel::getType() const {
        return type;
    }

    void Voxel::setType(VoxelType type) {
        this->type = type;
    }
}  // namespace GLOO
