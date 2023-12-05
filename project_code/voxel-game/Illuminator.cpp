#include "Illuminator.hpp"

#include <limits>
#include <stdexcept>

#include <glm/geometric.hpp>

#include "gloo/lights/DirectionalLight.hpp"
#include "gloo/lights/PointLight.hpp"
#include "gloo/SceneNode.hpp"

namespace GLOO {
void Illuminator::GetIllumination(const LightComponent& light_component,
                                  const glm::vec3& hit_pos,
                                  glm::vec3& dir_to_light,
                                  glm::vec3& intensity,
                                  float& dist_to_light) {
  // Calculation will be done in world space.

  auto light_ptr = light_component.GetLightPtr();
  if (light_ptr->GetType() == LightType::Directional) {
    auto directional_light_ptr = static_cast<DirectionalLight*>(light_ptr);
    dir_to_light = -directional_light_ptr->GetDirection();
    intensity = directional_light_ptr->GetDiffuseColor();
    dist_to_light = std::numeric_limits<float>::max();
  } else if (light_ptr->GetType() == LightType::Point) {
      auto point_light_ptr = static_cast<PointLight*>(light_ptr);
      auto pos = light_component.GetNodePtr()->GetTransform().GetWorldPosition();
      dir_to_light = glm::normalize(pos - hit_pos);
      // Attenuation is a vector of 3 equal attenuation values
      glm::vec3 attenuation = point_light_ptr->GetAttenuation();
      dist_to_light = glm::distance(pos, hit_pos);
      auto color = point_light_ptr->GetDiffuseColor();
      intensity = color * float (1.f / (float(attenuation[0]) * std::pow(dist_to_light,2)));
  }
}
}  // namespace GLOO
