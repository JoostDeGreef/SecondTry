#pragma once

namespace OpenGL
{
    class Shader
    {
    public:
        Shader();
        Shader(const std::string & resourceName);
        Shader(const std::string & vertex, const std::string & fragment, const std::vector<std::string> & attributes = {});

        ~Shader();

        // use the shader. returns a vector with the attributes in order as supplied in the constructor
        const std::vector<GLint> & Use();

        // load a shader from a resource, in this order: disk, data files, compiled. 
        static Shader LoadFromResource(const std::string & name);

    private:
        GLuint m_program;
        std::vector<GLint> m_attributes;
    };

}