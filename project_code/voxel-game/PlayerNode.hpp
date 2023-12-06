#ifndef PLAYER_NODE_H_
#define PLAYER_NODE_H_

#include "gloo/SceneNode.hpp"

namespace GLOO {
class CameraComponent;

class PlayerNode : public SceneNode {
 public:
  PlayerNode(float fov = 45.f, float aspect = 0.75f, float speed = 2.0f, float distance = 5.0f);
  void Update(double delta_time) override;
  void Calibrate();

 private:
  void UpdateViewport();
  void PlayerRotation(glm::dvec2 pos);
  float fov_;
  float speed_;
  float distance_;
  glm::vec3 start_position_;
  glm::quat start_rotation_;
  float start_distance_;
  glm::dvec2 mouse_start_click_;
};
}  // namespace GLOO

#endif
