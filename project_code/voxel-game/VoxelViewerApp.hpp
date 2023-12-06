#ifndef VOXEL_VIEWER_APP_H_
#define VOXEL_VIEWER_APP_H_

#include "gloo/Application.hpp"
namespace GLOO {
	class VoxelViewerApp : public Application {
public:
		VoxelViewerApp();
		void SetupScene() override;
	protected:
		void DrawGUI() override;
private:
	int bounces_;
	int size_;
	bool shadows_;
	};
} // namespace GLOO
#endif