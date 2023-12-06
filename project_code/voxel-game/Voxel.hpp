// Voxel.hpp

#ifndef VOXEL_H
#define VOXEL_H

namespace GLOO{
    // VoxelType can be an enum representing different types of voxels (e.g., air, dirt, stone)
    enum class VoxelType {
        Air,
        Dirt,
        Stone,
        // ... other types
    };

    class Voxel {
    public:
        Voxel();
        explicit Voxel(VoxelType type);

        VoxelType getType() const;
        void setType(VoxelType type);

        // Additional functions for voxel behavior can be added here

    private:
        VoxelType type;
    };
}   // namespace GLOO

#endif // VOXEL_H