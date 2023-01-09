#pragma once

namespace OpenGL
{
    class Shader
    {
        friend class ShaderCache;
    public:
        // use the shader
        void Activate();
        void DeActivate();

        bool IsLoaded() const;

        // get the program identifier
        GLuint GetProgram() const;

        Shader();
        void Unload();

        enum class Uniforms
        {
            model = 0,
            view = 1,
            projection = 2,
            color = 3,
            lightPos = 4,
            lightColor = 5,
            ambientStrength = 6,
            ambientColor = 7,
            reflectionStrength = 8,
            reflectionColor = 9,
            text = 10,
        };
    protected:
        virtual bool Load() = 0;

        static std::map<Uniforms,std::string> m_UniformNames;
       
        void SetModel(const OpenGL::Mat4 & model);
        void SetView(const OpenGL::Mat4 & view);
        void SetProjection(const OpenGL::Mat4 & projection);
        void SetColor(const RGBColorf & color);
        void SetLightPos(const Core::Vector3d & lightPos);
        void SetLightColor(const RGBColorf & lightColor);
        void SetAmbientStrength(const double & ambientStrength);
        void SetAmbientColor(const RGBColorf & ambientColor);
        void SetReflectionStrength(const double & reflectionStrength);
        void SetReflectionColor(const RGBColorf & reflectionColor);
        void SetText(const GLint text);

        class State
        {
        public:
            State();
            ~State();

            GLuint GetProgram() const;
            GLuint GetUniform(Shader::Uniforms id) const;

            // load a shader state from a resource, in this order: disk, data files, compiled. 
            static State LoadFromResource(const std::string & name);

        private:
            GLuint m_program;
            std::vector<GLuint> m_uniforms;

            void SetProgram(const GLuint program);
        };
        Shader::State m_state;

    public:
    };
};

namespace OpenGL
{
    class ShaderCache
    {
    private:
        ShaderCache()
        {}
        ~ShaderCache()
        {}
        
    public:
        static void LoadAll();
        static void UnloadAll();
        static Shader::State & Get(const std::string & name);
    private:
        static ShaderCache & Instance();
        std::map<std::string,Shader::State> m_shaders;
    };
}

#include "Shaders/2d.h"
#include "Shaders/3d.h"
#include "Shaders/3dphong.h"
#include "Shaders/text.h"
