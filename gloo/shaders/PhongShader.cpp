#include "PhongShader.hpp"

#include <stdexcept>

#include <glm/gtc/quaternion.hpp>
#include <glm/matrix.hpp>

#include "gloo/components/CameraComponent.hpp"
#include "gloo/components/LightComponent.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/SceneNode.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/lights/PointLight.hpp"
#include "gloo/lights/DirectionalLight.hpp"

namespace GLOO {
PhongShader::PhongShader()
    : ShaderProgram(std::unordered_map<GLenum, std::string>{
          {GL_VERTEX_SHADER, "phong.vert"},
          {GL_FRAGMENT_SHADER, "phong.frag"}}) {
}

void PhongShader::AssociateVertexArray(VertexArray& vertex_array) const {
  if (!vertex_array.HasPositionBuffer()) {
    throw std::runtime_error("Phong shader requires vertex positions!");
  }
  if (!vertex_array.HasNormalBuffer()) {
    throw std::runtime_error("Phong shader requires vertex normals!");
  }
  vertex_array.LinkPositionBuffer(GetAttributeLocation("vertex_position"));
  vertex_array.LinkNormalBuffer(GetAttributeLocation("vertex_normal"));
  if (vertex_array.HasTexCoordBuffer()) {
    GLint loc = GetAttributeLocation("vertex_tex_coord");
    if (loc != -1) {
      vertex_array.LinkTexCoordBuffer(loc);
    }
  }
}

void PhongShader::SetTargetNode(const SceneNode& node,
                                const glm::mat4& model_matrix) const {
  // Associate the right VAO before rendering.
  AssociateVertexArray(node.GetComponentPtr<RenderingComponent>()
                           ->GetVertexObjectPtr()
                           ->GetVertexArray());

  // Set transform.
  glm::mat3 normal_matrix =
      glm::transpose(glm::inverse(glm::mat3(model_matrix)));
  SetUniform("model_matrix", model_matrix);
  SetUniform("normal_matrix", normal_matrix);

  // Set material.
  MaterialComponent* material_component_ptr =
      node.GetComponentPtr<MaterialComponent>();
  const Material* material_ptr;
  if (material_component_ptr == nullptr) {
    material_ptr = &Material::GetDefault();
  } else {
    material_ptr = &material_component_ptr->GetMaterial();
  }
  SetUniform("material.ambient", material_ptr->GetAmbientColor());
  SetUniform("material.diffuse", material_ptr->GetDiffuseColor());
  SetUniform("material.specular", material_ptr->GetSpecularColor());
  SetUniform("material.shininess", material_ptr->GetShininess());

  // Bind the ambient, diffuse, and specular textures from the material
  // (if there's any) to separate texture units (e.g. 0, 1, 2) and then set the
  // shader properly to use these texture units.
  SetUniform("ambient_texture", 0);
  SetUniform("diffuse_texture", 1);
  SetUniform("specular_texture", 2);
  if (material_ptr->GetAmbientTexture()) {
      SetUniform("ambient_enabled", true);
      material_ptr->GetAmbientTexture()->BindToUnit(0);
  }
  else {
      SetUniform("ambient_enabled", false);
  }
  if (material_ptr->GetDiffuseTexture()) {
      material_ptr->GetDiffuseTexture()->BindToUnit(1);
      SetUniform("diffuse_enabled", true);
  }
  else {
      SetUniform("diffuse_enabled", false);
  }
  if (material_ptr->GetSpecularTexture()) {
      SetUniform("specular_enabled", true);
      material_ptr->GetSpecularTexture()->BindToUnit(2);
  }
  else {
      SetUniform("specular_enabled", false);
  }
}

void PhongShader::SetCamera(const CameraComponent& camera) const {
  SetUniform("view_matrix", camera.GetViewMatrix());
  SetUniform("projection_matrix", camera.GetProjectionMatrix());
  SetUniform("camera_position",
             camera.GetNodePtr()->GetTransform().GetWorldPosition());
}

void PhongShader::SetLightSource(const LightComponent& component) const {
  auto light_ptr = component.GetLightPtr();
  if (light_ptr == nullptr) {
    throw std::runtime_error("Light component has no light attached!");
  }

  // First disable all lights.
  // In a single rendering pass, only one light of one type is enabled.
  SetUniform("ambient_light.enabled", false);
  SetUniform("point_light.enabled", false);
  SetUniform("directional_light.enabled", false);

  if (light_ptr->GetType() == LightType::Ambient) {
    auto ambient_light_ptr = static_cast<AmbientLight*>(light_ptr);
    SetUniform("ambient_light.enabled", true);
    SetUniform("ambient_light.ambient", ambient_light_ptr->GetAmbientColor());
  } else if (light_ptr->GetType() == LightType::Point) {
    auto point_light_ptr = static_cast<PointLight*>(light_ptr);
    SetUniform("point_light.enabled", true);
    SetUniform("point_light.position",
               component.GetNodePtr()->GetTransform().GetPosition());
    SetUniform("point_light.diffuse", point_light_ptr->GetDiffuseColor());
    SetUniform("point_light.specular", point_light_ptr->GetSpecularColor());
    SetUniform("point_light.attenuation", point_light_ptr->GetAttenuation());
  } else if (light_ptr->GetType() == LightType::Directional) {
    auto directional_light_ptr = static_cast<DirectionalLight*>(light_ptr);
    SetUniform("directional_light.enabled", true);
    SetUniform("directional_light.direction",
               directional_light_ptr->GetDirection());
    SetUniform("directional_light.diffuse",
               directional_light_ptr->GetDiffuseColor());
    SetUniform("directional_light.specular",
               directional_light_ptr->GetSpecularColor());
  } else {
    throw std::runtime_error(
        "Encountered light type unrecognized by the shader!");
  }
}

void PhongShader::SetShadowMapping(
    const Texture& shadow_texture,
    const glm::mat4& world_to_light_ndc_matrix) const {
  // Set necessary uniforms for the shader and bind the texture to the
  // corresponding texture unit.
    SetUniform("shadow_texture", 3);
    SetUniform("world_to_light_ndc_matrix", world_to_light_ndc_matrix);
    shadow_texture.BindToUnit(3);
}
}  // namespace GLOO
