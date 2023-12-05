#include "Tracer.hpp"

#include <glm/gtx/string_cast.hpp>
#include <stdexcept>
#include <algorithm>

#include "gloo/Transform.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/lights/AmbientLight.hpp"

#include "gloo/Image.hpp"
#include "Illuminator.hpp"

#include "gloo/lights/LightBase.hpp"

namespace GLOO {
void Tracer::Render(const Scene& scene, const std::string& output_file) {
  scene_ptr_ = &scene;

  auto& root = scene_ptr_->GetRootNode();
  tracing_components_ = root.GetComponentPtrsInChildren<TracingComponent>();
  light_components_ = root.GetComponentPtrsInChildren<LightComponent>();

  Image image(image_size_.x, image_size_.y);
  for (size_t y = 0; y < image_size_.y; y++) {
    for (size_t x = 0; x < image_size_.x; x++) {
      // TODO: For each pixel, cast a ray, and update its value in the image.
        // Generates new ray that is scaled down to x: [-1,1], y:[-1,1]
        Ray new_ray = camera_.GenerateRay(glm::vec2(float(2.f * (float(x) / image_size_.x)) - 1.f, float(2.f * (float(y) / image_size_.y)) - 1.f));
        // Creates new record for each ray
        HitRecord new_rec = HitRecord();
        glm::vec3 color = TraceRay(new_ray, size_t(0), new_rec);
        image.SetPixel(x, y, color);
    }
  }

  if (output_file.size())
    image.SavePNG(output_file);
}

// 0.0001f will be epsilon value
glm::vec3 Tracer::TraceRay(const Ray& ray,
                           size_t bounces,
                           HitRecord& record) const {
  // TODO: Compute the color for the cast ray.
    // BASE CASE FOR SPECULAR
    if (bounces > max_bounces_) {
        return glm::vec3(0.f);
    }
    Material mat;
    // Checks if ever hits an object
    bool ever_inter = false;
    for (int i = 0; i < tracing_components_.size(); i++) {
        auto t_comp = tracing_components_[i];
        auto trans_mat = t_comp->GetNodePtr()->GetTransform().GetLocalToWorldMatrix();
        // Creates copy of ray and transforms it to object space
        auto obj_ray = ray;
        obj_ray.ApplyTransform(glm::inverse(trans_mat));
        // Since record is updated as this runs, will automatically find lowest time
        auto intersect = t_comp->GetHittable().Intersect(obj_ray, 0.0001f, record);
        if (intersect) {
            // If intersects, get material that intersected with
            auto trans_norm = glm::inverse(glm::transpose(trans_mat)) * glm::vec4(record.normal, 0.f);
            record.normal = glm::normalize(glm::vec3(trans_norm.x, trans_norm.y, trans_norm.z));
            mat = t_comp->GetNodePtr()->GetComponentPtr<MaterialComponent>()->GetMaterial();
            ever_inter = true;
        }
    }
    if (ever_inter) {
        // Illuminator
        glm::vec3 overall_intensity = glm::vec3(0.f);
        glm::vec3 ref;
        for (int j = 0; j < light_components_.size(); j++) {
            auto l_comp = light_components_[j];
            // If ambient light, just add I_ambient
            if (l_comp->GetLightPtr()->GetType() == LightType::Ambient) {
                overall_intensity += (l_comp->GetLightPtr()->GetDiffuseColor() * mat.GetDiffuseColor());
                continue;
            }
            // Otherwise add diffuse and specular
            glm::vec3 dir_to_light = glm::vec3();
            glm::vec3 intensity = glm::vec3();
            float dist_to_light = 0.f;
            Illuminator::GetIllumination(*l_comp, ray.At(record.time), dir_to_light, intensity, dist_to_light);
            float d_clamp = Clamp(dir_to_light, record.normal);
            glm::vec3 diffuse_I = d_clamp * (intensity * mat.GetDiffuseColor());
            ref = glm::normalize(ray.GetDirection() - (2.f * (glm::dot(ray.GetDirection(), record.normal)) * record.normal));
            float spec_clamp = Clamp(dir_to_light, ref);
            glm::vec3 spec_I = std::pow(spec_clamp, mat.GetShininess()) * intensity * mat.GetSpecularColor();
            overall_intensity += (diffuse_I + spec_I);
            // If shadows are enabled, have new ray towards light source
            if (shadows_enabled_) {
                auto shadow_ray = Ray(ray.At(record.time) + (0.0001f * dir_to_light), dir_to_light);
                HitRecord shadow_rec = HitRecord();
                auto color = TraceRay(shadow_ray, bounces, shadow_rec);
                // If non-background color then intersection occured and ifintersection is closer than distnace to light, remove direct lighting.
                if (color != GetBackgroundColor(shadow_ray.GetDirection()) && glm::distance(ray.At(record.time), shadow_ray.At(shadow_rec.time)) < dist_to_light) {
                    if (l_comp->GetLightPtr()->GetType() == LightType::Ambient) {
                        overall_intensity -= (l_comp->GetLightPtr()->GetDiffuseColor() * mat.GetDiffuseColor());
                    }
                    else {
                        overall_intensity -= (diffuse_I + spec_I);
                    }
                }
            }
        }
        // Checks if specular material
        if (mat.GetSpecularColor() != glm::vec3(0.0f) && bounces < max_bounces_) {
            // Create new ray starting from intersection point (with some epsilon error) in the direction of perfect reflectance
            auto reflected_ray = Ray(ray.At(record.time) + (0.0001f * ref), ref);
            bounces++;
            HitRecord reflected_rec = HitRecord();
            overall_intensity += (mat.GetSpecularColor() * TraceRay(reflected_ray, bounces, reflected_rec));
        }
        return overall_intensity;
    }
    // Return background if no intersections
    return GetBackgroundColor(ray.GetDirection());
}


glm::vec3 Tracer::GetBackgroundColor(const glm::vec3& direction) const {
  if (cube_map_ != nullptr) {
    return cube_map_->GetTexel(direction);
  } else
    return background_color_;
}

float Tracer::Clamp(glm::vec3 vec1, glm::vec3 vec2) const {
    float dot = glm::dot(vec1, vec2);
    if (dot > 0.f) {
        return dot;
    }
    return 0.f;
}
}  // namespace GLOO
