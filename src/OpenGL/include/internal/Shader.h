#pragma once

namespace OpenGL
{
    class Shader
    {
    public:
        unsigned int Program;

        Shader(const std::string & vertex, const std::string & fragment);

        unsigned int Compile();
        void Use();
    private:
        const std::string m_vertex;
        const std::string m_fragment;
    };
}