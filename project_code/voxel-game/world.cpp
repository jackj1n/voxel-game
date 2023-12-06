#include "world.hpp"

#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <stdlib.h>

namespace GLOO
{
	World::World(long seed)
	{
		voxels[0][0][0] = Voxel(VoxelType::Dirt);
		shader_ = std::make_shared<PhongShader>();
		world_mesh_ = std::make_shared<VertexObject>();
	}

	void World::Render(SceneNode& root)
	{
		auto positions = make_unique<PositionArray>();
		auto normals = make_unique<NormalArray>();
		auto indices = make_unique<IndexArray>();

		for (int i = 0; i < WORLD_SIZE; i++)
		{
			for (int j = 0; j < WORLD_SIZE; j++)
			{
				for (int k = 0; k < WORLD_SIZE; k++)
				{
					
					// only for one voxel for now

					positions->push_back(glm::vec3(i, j, k));
					positions->push_back(glm::vec3(i+1, j, k));
					positions->push_back(glm::vec3(i, j+1, k));
					positions->push_back(glm::vec3(i+1, j+1, k));
					positions->push_back(glm::vec3(i, j+1, k+1));
					positions->push_back(glm::vec3(i+1, j+1, k+1));
					positions->push_back(glm::vec3(i, j, k+1));
					positions->push_back(glm::vec3(i+1, j, k+1));

					normals->push_back(glm::vec3(0, 0, -1));
					normals->push_back(glm::vec3(0, 0, -1));
					normals->push_back(glm::vec3(0, 1, 0));
					normals->push_back(glm::vec3(0, 1, 0));
					normals->push_back(glm::vec3(0, 0, 1));
					normals->push_back(glm::vec3(0, 0, 1));
					normals->push_back(glm::vec3(0, -1, 0));
					normals->push_back(glm::vec3(0, -1, 0));
					normals->push_back(glm::vec3(-1, 0, 0));
					normals->push_back(glm::vec3(-1, 0, 0));
					normals->push_back(glm::vec3(1, 0, 0));
					normals->push_back(glm::vec3(1, 0, 0));

					auto index_nums = {	0, 1, 2, 1, 2, 3, 
										2, 3, 4, 3, 4, 5, 
										4, 5, 6, 5, 6, 7,
										6, 7, 0, 7, 0, 1, 
										0, 2, 6, 2, 6, 4, 
										1, 3, 7, 3, 7, 5 };
					indices->insert(indices->end(), index_nums.begin(), index_nums.end());

					world_mesh_->UpdatePositions(std::move(positions));
					world_mesh_->UpdateNormals(std::move(normals));
					world_mesh_->UpdateIndices(std::move(indices));
				}
			}
		}
		

		auto mesh_node = make_unique<SceneNode>();
		mesh_node->CreateComponent<ShadingComponent>(shader_);
		mesh_node->CreateComponent<RenderingComponent>(world_mesh_);
		mesh_node->GetComponentPtr<RenderingComponent>()->SetDrawMode(DrawMode::Triangles);
		mesh_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(glm::vec3(0.5f), glm::vec3(0.5f), glm::vec3(0.5f), 1.0f));

		root.AddChild(std::move(mesh_node));
	}

	void World::Update(glm::vec3(pos))
	{

	}
}  // namespace GLOO
