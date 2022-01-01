#pragma once

namespace OpenGL
{
    class Shader
    {
    public:
        Shader();
        Shader(const std::string & resourceName);
        Shader(const std::string & vertex,
               const std::string & fragment,
               const std::vector<std::string> & attributes = {},
               const std::vector<std::string> & uniforms = {});

        ~Shader();

        // use the shader. returns a vector with the attributes and a vector with the uniforms, in order as supplied in the constructor
        const std::tuple<std::vector<GLint>,std::vector<GLint>> Use();

        // get the program identifier
        const GLuint GetProgram();

        // get the attribute identifiers
        const std::vector<GLint> GetAttributes();

        // get the uniform identifiers
        const std::vector<GLint> GetUniforms();

        // load a shader from a resource, in this order: disk, data files, compiled. 
        static Shader LoadFromResource(const std::string & name);

    private:
        GLuint m_program;
        std::vector<GLint> m_attributes;
        std::vector<GLint> m_uniforms;
    };

}