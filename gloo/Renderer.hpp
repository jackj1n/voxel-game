#ifndef GLOO_RENDERER_H_
#define GLOO_RENDERER_H_

#include "components/LightComponent.hpp"
#include "components/RenderingComponent.hpp"
#include "gl_wrapper/Texture.hpp"
#include "gl_wrapper/Framebuffer.hpp"
#include "shaders/PlainTextureShader.hpp"

#include <unordered_map>

#include "shaders/ShadowShader.hpp"


namespace GLOO {
class Scene;
class Application;
class Renderer {
 public:
  Renderer(Application& application);
  void Render(const Scene& scene) const;

 private:
  using RenderingInfo = std::vector<std::pair<RenderingComponent*, glm::mat4>>;
  void RenderScene(const Scene& scene) const;
  void SetRenderingOptions() const;

  RenderingInfo RetrieveRenderingInfo(const Scene& scene) const;
  static void RecursiveRetrieve(const SceneNode& node,
                                RenderingInfo& info,
                                const glm::mat4& model_matrix);
  std::unique_ptr<VertexObject> quad_;

  std::unique_ptr<Texture> shadow_depth_tex_;
  std::unique_ptr<PlainTextureShader> plain_texture_shader_;
  Application& application_;
  // NEW CODE
  std::unique_ptr<Framebuffer> frame_buffer_;
  std::unique_ptr<ShadowShader> shadow_shader_;
  void RenderShadow(RenderingInfo& rendering_info, LightComponent& light) const;
};
}  // namespace GLOO

#endif
