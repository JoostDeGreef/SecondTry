#pragma once

namespace OpenGL
{
    class Shader_2d : public Shader
    {
    public:
        using Shader::SetModel;
        using Shader::SetProjection;
        using Shader::SetColor;

        bool Load() override
        {
            m_state = ShaderCache::Get("2d");
            return m_state.GetProgram() != 0;
        }
    };
}
