#pragma once

namespace OpenGL
{
    class Shader
    {
    public:
        ~Shader();

        // use the shader
        std::vector<GLuint> Use(const std::vector<std::string> & uniforms = {});
        template<typename ...ARGS>
        std::vector<GLuint> Use(const std::string & arg0, ARGS... args)
        {
            return Use({arg0, args...});
        }

        // get the program identifier
        const GLuint GetProgram();

        // load a shader from a resource, in this order: disk, data files, compiled. 
        static Shader LoadFromResource(const std::string & name);

        Shader();
        Shader(const std::string & vertex,
               const std::string & fragment);

        bool IsLoaded() const;
    private:
        class State
        {
        public:
            State();
            ~State();

            const GLuint GetProgram();
            void SetProgram(const GLuint program);

        private:
            GLuint m_program;
        };
        std::shared_ptr<Shader::State> m_state;
    };

    // class ShaderCache
    // {
    // private:
    //     ShaderCache()
    //     {}

    // public:
    //     Shader & Use(const std::string & name, );
    // };
}