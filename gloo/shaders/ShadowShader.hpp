#ifndef SHADOW_SHADER_H_
#define SHADOW_SHADER_H_

#include "gloo/shaders/ShaderProgram.hpp"

namespace GLOO {
    class ShadowShader : public ShaderProgram {
    public:
        ShadowShader();
        void SetTargetNode(const SceneNode& node,
            const glm::mat4& model_matrix) const override;
        void SetLightSource(const LightComponent& light) const override;

    private:
        void AssociateVertexArray(VertexArray& vertex_array) const;
    };
}  // namespace GLOO

#endif
