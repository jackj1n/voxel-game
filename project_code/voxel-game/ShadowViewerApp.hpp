#ifndef SHADOW_VIEWER_APP_H_
#define SHADOW_VIEWER_APP_H_

#include "gloo/Application.hpp"

namespace GLOO {
class ShadowViewerApp : public Application {
 public:
  ShadowViewerApp(const std::string& app_name, glm::ivec2 window_size);
  void SetupScene() override;
  protected:
	void DrawGUI() override;
private:
	int seed_ = 0;
	bool enable_shadows_ = false;
	
};
}  // namespace GLOO

#endif
