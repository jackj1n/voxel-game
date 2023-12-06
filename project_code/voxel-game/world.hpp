#ifndef WORLD_H
#define WORLD_H

#include "gloo/SceneNode.hpp"
#include "Voxel.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/PhongShader.hpp"

namespace GLOO
{
	class World
	{
	public:
		World(long seed);
		void Render(SceneNode& root);
		void Update(glm::vec3(pos));
		static const int WORLD_SIZE = 1;
		Voxel voxels[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE];
		std::shared_ptr<ShaderProgram>shader_;
		std::shared_ptr<VertexObject>world_mesh_;
	};
}
#endif // WORLD_H