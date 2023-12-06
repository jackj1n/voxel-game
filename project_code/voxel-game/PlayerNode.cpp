#include "PlayerNode.hpp"

#include <iostream>

#include <glm/gtx/quaternion.hpp>

#include "gloo/components/CameraComponent.hpp"
#include "gloo/InputManager.hpp"

namespace GLOO {
PlayerNode::PlayerNode(float fov, float aspect, float speed, float distance)
    : SceneNode(), speed_(speed) {
  auto camera = make_unique<CameraComponent>(fov, aspect, 0.1f, 100.f);
  AddComponent(std::move(camera));

  start_position_ = GetTransform().GetPosition();
  start_rotation_ = GetTransform().GetRotation();
  start_distance_ = distance;
  prev_mous_pos_ = glm::dvec2(0.0, 0.0);
}

void PlayerNode::Update(double delta_time) {
  UpdateViewport();

  float delta_dist = speed_ * static_cast<float>(delta_time);
  glm::vec3 old_position = GetTransform().GetPosition();
  glm::vec3 new_position = old_position;
  static bool prev_released = true;
  if (InputManager::GetInstance().IsLeftMousePressed()) {
      if (prev_released) {
          mouse_start_click_ = InputManager::GetInstance().GetCursorPosition();
      }
      PlayerRotation(InputManager::GetInstance().GetCursorPosition());
      prev_released = false;
  }
  else {
      auto scroll = InputManager::GetInstance().FetchAndResetMouseScroll();
      prev_released = true;
      start_position_ = GetTransform().GetPosition();
      start_rotation_ = GetTransform().GetRotation();
      start_distance_ = distance_;
  }
  auto V = make_unique<glm::mat4>(glm::lookAt(
      glm::vec3(0, 0, distance_), glm::vec3(0), glm::vec3(0, 1.f, 0)));
  *V *= glm::toMat4(GetTransform().GetRotation()) *
      glm::translate(glm::mat4(1.f), GetTransform().GetPosition());
  GetComponentPtr<CameraComponent>()->SetViewMatrix(std::move(V));
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

    //const float sensitivity = 0.5f;  // Adjust the sensitivity based on your preference
    //auto delta_pos = pos - prev_mous_pos_;
    //auto rot = GetTransform().GetRotation();
    //float yaw = glm::yaw(rot);
    //float pitch = glm::pitch(rot);

    //// Update yaw (left and right rotation)
    //float yawChange = delta_pos.x * sensitivity;
    //yaw += yawChange;

    //// Update pitch (up and down rotation)
    //float pitchChange = delta_pos.y * sensitivity;
    //pitch += pitchChange;

    //// Ensure pitch stays within a reasonable range to avoid gimbal lock
    //const float maxPitch = 89.0f;  // Adjust based on your needs
    //pitch = glm::clamp(pitch, -maxPitch, maxPitch);
    //GetTransform().SetRotation(glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), 0.0f)));
    //prev_mous_pos_ = pos;

    float sx, sy, sz, ex, ey, ez;
    float scale;
    float sl, el;
    float dotprod;
    glm::ivec2 window_size = InputManager::GetInstance().GetWindowSize();

    // find vectors from center of window
    sx = float(mouse_start_click_.x - (window_size.x / 2.f));
    sy = float(mouse_start_click_.y - (window_size.y / 2.f));
    ex = float(pos.x - (window_size.x / 2.f));
    ey = float(pos.y - (window_size.y / 2.f));

    // invert y coordinates (raster versus device coordinates)
    sy = -sy;
    ey = -ey;

    // scale by inverse of size of window and magical sqrt2 factor
    scale = fmin(float(window_size.x), float(window_size.y));

    scale = 1.f / scale;

    sx *= scale;
    sy *= scale;
    ex *= scale;
    ey *= scale;

    // project points to unit circle
    sl = hypot(sx, sy);
    el = hypot(ex, ey);

    if (sl > 1.f) {
        sx /= sl;
        sy /= sl;
        sl = 1.0;
    }
    if (el > 1.f) {
        ex /= el;
        ey /= el;
        el = 1.f;
    }

    // project up to unit sphere - find Z coordinate
    sz = sqrt(1.0f - sl * sl);
    ez = sqrt(1.0f - el * el);

    // rotate (sx,sy,sz) into (ex,ey,ez)

    // compute angle from dot-product of unit vectors (and double it).
    // compute axis from cross product.
    dotprod = sx * ex + sy * ey + sz * ez;

    if (dotprod != 1 && !std::isnan(dotprod)) {
        glm::vec3 axis(sy * ez - ey * sz, sz * ex - ez * sx, sx * ey - ex * sy);
        axis = glm::normalize(axis);
        if (std::isnan(axis.x) || std::isnan(axis.y) || std::isnan(axis.z)) {
            return;
        }

        float angle = 2.0f * acos(dotprod);

        GetTransform().SetRotation(glm::angleAxis(angle, axis) * start_rotation_);
    }
}
}  // namespace GLOO
