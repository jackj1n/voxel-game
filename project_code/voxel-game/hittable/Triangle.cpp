#include "Triangle.hpp"

#include <iostream>
#include <stdexcept>

#include <glm/common.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Plane.hpp"

namespace GLOO {
Triangle::Triangle(const glm::vec3& p0,
                   const glm::vec3& p1,
                   const glm::vec3& p2,
                   const glm::vec3& n0,
                   const glm::vec3& n1,
                   const glm::vec3& n2) {
    positions_ = { p0, p1, p2 };
    normals_ = { n0, n1, n2 };
}

Triangle::Triangle(const std::vector<glm::vec3>& positions,
                   const std::vector<glm::vec3>& normals) {
    positions_ = positions;
    normals_ = normals;
}

bool Triangle::Intersect(const Ray& ray, float t_min, HitRecord& record) const {
  // TODO: Implement ray-triangle intersection.
    glm::vec3 a = positions_[0];
    glm::vec3 b = positions_[1];
    glm::vec3 c = positions_[2];
    auto rD = ray.GetDirection();
    auto rO = ray.GetOrigin();
    // Uses Cramer's Rule to find alpha, beta, gamma, and t
    glm::mat3 betaNum = { float(a[0] - rO[0]), float(a[0] - c[0]), rD[0],
                          float(a[1] - rO[1]), float(a[1] - c[1]), rD[1],
                          float(a[2] - rO[2]), float(a[2] - c[2]), rD[2],
    };
    glm::mat3 gammaNum = { float(a[0] - b[0]), float(a[0] - rO[0]), rD[0],
                           float(a[1] - b[1]), float(a[1] - rO[1]), rD[1],
                           float(a[2] - b[2]), float(a[2] - rO[2]), rD[2],
    };
    glm::mat3 tNum = { float(a[0] - b[0]), float(a[0] - c[0]), float(a[0] - rO[0]),
                       float(a[1] - b[1]), float(a[1] - c[1]), float(a[1] - rO[1]),
                       float(a[2] - b[2]), float(a[2] - c[2]), float(a[2] - rO[2]),
    };
    glm::mat3 A_mat = { float(a[0] - b[0]), float(a[0] - c[0]), rD[0],
                        float(a[1] - b[1]), float(a[1] - c[1]), rD[1],
                        float(a[2] - b[2]), float(a[2] - c[2]), rD[2],
    };
    float A_det = glm::determinant(A_mat);
    float beta = glm::determinant(betaNum) / A_det;
    float gamma = glm::determinant(gammaNum) / A_det;
    float alpha = 1.f - beta - gamma;
    float t = glm::determinant(tNum) / A_det;

    // Condition for intersection
    if (gamma + beta <= 1.f && gamma >= 0.f && beta >= 0.f && t > t_min) {
        if (t < record.time) {
            record.time = t;
            // Interpolates the normal
            glm::vec3 interp_norm = alpha * normals_[0] + beta * normals_[1] + gamma * normals_[2];
            record.normal = glm::normalize(interp_norm);
            return true;
        }
        return false;
    }
    return false;
}
}  // namespace GLOO
