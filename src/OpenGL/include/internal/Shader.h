#pragma once

namespace OpenGL
{
    class Shader
    {
    public:
        unsigned int Program;

        Shader(const std::string & vertex, const std::string & fragment);

        void Use();
    private:
    };
}