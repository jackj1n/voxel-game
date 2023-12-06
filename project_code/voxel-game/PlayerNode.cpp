#include "PlayerNode.hpp"

#include <iostream>

#include <glm/gtx/quaternion.hpp>

#include "gloo/components/CameraComponent.hpp"
#include "gloo/InputManager.hpp"

namespace GLOO {
PlayerNode::PlayerNode(float fov, float aspect, float speed, float distance)
    : SceneNode(), fov_(fov), speed_(speed), distance_(distance) {
  auto camera = make_unique<CameraComponent>(fov, aspect, 0.1f, 100.f);
  AddComponent(std::move(camera));
  start_position_ = GetTransform().GetPosition();
  start_rotation_ = GetTransform().GetRotation();
  start_distance_ = distance;
  mouse_start_click_ = glm::dvec2(0.0f);
}
void PlayerNode::Calibrate() {
    start_position_ = GetTransform().GetPosition();
    start_rotation_ = GetTransform().GetRotation();
}
void PlayerNode::Update(double delta_time) {
  UpdateViewport();

  static bool prev_released = true;
  if (InputManager::GetInstance().IsLeftMousePressed()) {
      if (prev_released) {
          mouse_start_click_ = InputManager::GetInstance().GetCursorPosition();
      }
      PlayerRotation(InputManager::GetInstance().GetCursorPosition());
      prev_released = false;
  }   else {
          prev_released = true;
          start_position_ = GetTransform().GetPosition();
          start_rotation_ = GetTransform().GetRotation();
          start_distance_ = distance_;
  }
  float delta_dist = speed_ * static_cast<float>(delta_time);
  glm::vec3 old_position = GetTransform().GetPosition();
  glm::vec3 new_position = old_position;
  if (InputManager::GetInstance().IsKeyPressed('W')) {
    new_position += delta_dist * GetTransform().GetForwardDirection();
  }
  if (InputManager::GetInstance().IsKeyPressed('S')) {
    new_position -= delta_dist * GetTransform().GetForwardDirection();
  }
  if (InputManager::GetInstance().IsKeyPressed('A')) {
    new_position -= delta_dist * GetTransform().GetRightDirection();
  }
  if (InputManager::GetInstance().IsKeyPressed('D')) {
    new_position += delta_dist * GetTransform().GetRightDirection();
  }
  GetTransform().SetPosition(new_position);
}

void PlayerNode::UpdateViewport() {
  glm::ivec2 window_size = InputManager::GetInstance().GetWindowSize();
  float aspect_ratio =
      static_cast<float>(window_size.x) / static_cast<float>(window_size.y);
  GetComponentPtr<CameraComponent>()->SetAspectRatio(aspect_ratio);
}
void PlayerNode::PlayerRotation(glm::dvec2 pos) {
    const float sensitivity = 0.25f;  // Adjust the sensitivity based on your preference
    auto delta_pos = pos - mouse_start_click_;
    float yaw = glm::yaw(start_rotation_);
    float pitch = glm::pitch(start_rotation_);

    // Update yaw (left and right rotation)
    float yawChange = delta_pos.x * sensitivity;
    yaw += yawChange;

    // Update pitch (up and down rotation)
    float pitchChange = delta_pos.y * sensitivity;
    pitch += pitchChange;

    // Ensure pitch stays within a reasonable range to avoid gimbal lock
    const float maxPitch = 89.0f;  // Adjust based on your needs
    pitch = glm::clamp(pitch, -maxPitch, maxPitch);
    auto new_rotation = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), 0.0f));
    GetTransform().SetRotation(new_rotation);
    //std::cout << new_rotation.x << " " << new_rotation.y << " " << new_rotation.z << " " << new_rotation.w << std::endl;
}
}  // namespace GLOO
