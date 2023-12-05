#include "Plane.hpp"
#include <iostream>

namespace GLOO {
Plane::Plane(const glm::vec3& normal, float d) {
	normal_ = normal;
	d_ = d;
}

bool Plane::Intersect(const Ray& ray, float t_min, HitRecord& record) const {
  // TODO: Implement ray-plane intersection.
	float denom = glm::dot(normal_, ray.GetDirection());
	// Makes sures no divide by 0 error
	if (denom != 0.f) {
		glm::vec3 center = d_ * normal_;
		float numer = glm::dot(center - ray.GetOrigin(), normal_);
		//float numer = -1.f*(d_ + glm::dot(normal_, ray.GetOrigin())); // I don't know why this doesnt work, this was the equation given on the lecture slides
		float t = numer / denom;
		if (t < record.time && t > t_min) {
			record.time = t;
			record.normal = normal_;
			return true;
		}
	}
	return false;
}
}  // namespace GLOO
