#pragma once

namespace OpenGL
{
    class Shader_3d_phong : public Shader
    {
    public:
        using Shader::SetModel;
        using Shader::SetProjection;
        using Shader::SetView;
        using Shader::SetColor;
        using Shader::SetLightPos;
        using Shader::SetLightColor;
        using Shader::SetAmbientStrength;
        using Shader::SetAmbientColor;
        using Shader::SetReflectionStrength;
        using Shader::SetReflectionColor;

        bool Load() override
        {
            m_state = ShaderCache::Get("3d_phong");
            return m_state.GetProgram() != 0;
        }
    };
}
