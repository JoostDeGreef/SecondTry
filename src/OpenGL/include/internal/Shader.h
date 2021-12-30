#pragma once

namespace OpenGL
{
    class Shader
    {
    public:
        unsigned int Program;

        Shader();
        Shader(const std::string & vertex, const std::string & fragment);

        void Use();

        // load a shader from a resource, in this order: disk, data files, compiled. 
        static
        Shader LoadFromResource(const std::string & name);
    private:
    };

}