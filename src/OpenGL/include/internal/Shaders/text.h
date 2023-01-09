#pragma once

namespace OpenGL
{
    class Shader_text : public Shader
    {
    public:
        using Shader::SetModel;
        using Shader::SetProjection;
        using Shader::SetColor;
        using Shader::SetText;

        bool Load() override
        {
            m_state = ShaderCache::Get("text");
            return m_state.GetProgram() != 0;
        }
    };
}
