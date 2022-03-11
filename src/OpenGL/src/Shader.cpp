#include "OpenGL.h"
#include "Log.h"

namespace
{
    const auto GetCompiledShaders()
    {
        std::map<std::string,std::tuple<std::string,std::string,std::vector<OpenGL::Shader::Uniforms>>> shaders;

#include "Shaders/2d.cpp"
#include "Shaders/3d.cpp"
#include "Shaders/3dphong.cpp"
#include "Shaders/text.cpp"

        return shaders;
    }

}

    
namespace OpenGL
{
    std::map<Shader::Uniforms,std::string> Shader::m_UniformNames = 
    {
        {Shader::Uniforms::model,"model"},
        {Shader::Uniforms::view,"view"},
        {Shader::Uniforms::projection,"projection"},
        {Shader::Uniforms::color,"color"},
        {Shader::Uniforms::lightPos,"lightPos"},
        {Shader::Uniforms::lightColor,"lightColor"},
        {Shader::Uniforms::ambientStrength,"ambientStrength"},
        {Shader::Uniforms::ambientColor,"ambientColor"},
        {Shader::Uniforms::reflectionStrength,"reflectionStrength"},
        {Shader::Uniforms::reflectionColor,"reflectionColor"},
        {Shader::Uniforms::text,"text"},
    };

    void Shader::SetModel(const OpenGL::Mat4 & model)
    {
        GLuint uniform = m_state.GetUniform(Shader::Uniforms::model);
        GLCHECK(model.ApplyAsUniform(uniform));
    }
    void Shader::SetView(const OpenGL::Mat4 & view)
    {
        GLuint uniform = m_state.GetUniform(Shader::Uniforms::view);
        GLCHECK(view.ApplyAsUniform(uniform));
    }
    void Shader::SetProjection(const OpenGL::Mat4 & projection)
    {
        GLuint uniform = m_state.GetUniform(Shader::Uniforms::projection);
        GLCHECK(projection.ApplyAsUniform(uniform));
    }
    void Shader::SetColor(const RGBColorf & color)
    {
        GLuint uniform = m_state.GetUniform(Shader::Uniforms::color);
        GLCHECK(glUniform3f(uniform, color.R, color.G, color.B)); // todo: use ptr?
    }
    void Shader::SetLightPos(const Core::Vector3d & lightPos)
    {
        GLuint uniform = m_state.GetUniform(Shader::Uniforms::lightPos);
        GLCHECK(glUniform3f(uniform, lightPos[0], lightPos[1], lightPos[2])); // todo: use ptr?
    }
    void Shader::SetLightColor(const RGBColorf & lightColor)
    {
        GLuint uniform = m_state.GetUniform(Shader::Uniforms::lightColor);
        GLCHECK(glUniform3f(uniform, lightColor.R, lightColor.G, lightColor.B)); // todo: use ptr?
    }
    void Shader::SetAmbientStrength(const double & ambientStrength)
    {
        GLuint uniform = m_state.GetUniform(Shader::Uniforms::ambientStrength);
        GLCHECK(glUniform1f(uniform, (float)ambientStrength));
    }
    void Shader::SetAmbientColor(const RGBColorf & ambientColor)
    {
        GLuint uniform = m_state.GetUniform(Shader::Uniforms::ambientColor);
        GLCHECK(glUniform3f(uniform, ambientColor.R, ambientColor.G, ambientColor.B)); // todo: use ptr?
    }
    void Shader::SetReflectionStrength(const double & reflectionStrength)
    {
        GLuint uniform = m_state.GetUniform(Shader::Uniforms::reflectionStrength);
        GLCHECK(glUniform1f(uniform, (float)reflectionStrength));
    }
    void Shader::SetReflectionColor(const RGBColorf & reflectionColor)
    {
        GLuint uniform = m_state.GetUniform(Shader::Uniforms::reflectionColor);
        GLCHECK(glUniform3f(uniform, reflectionColor.R, reflectionColor.G, reflectionColor.B)); // todo: use ptr?
    }
    void Shader::SetText(const GLint text)
    {
        GLuint uniform = m_state.GetUniform(Shader::Uniforms::text);
        GLCHECK(glUniform1i(uniform, text));
    }

    Shader::Shader()
      : m_state()
    {}

    void Shader::Unload()
    {
        m_state = Shader::State();
    }

    void Shader::Activate()
    {
        bool isLoaded = IsLoaded();
        if(!isLoaded)
        {
            isLoaded = Load();
        }
        if(isLoaded)
        {
            GLCHECK(glUseProgram(GetProgram()));
        }
    }
    void Shader::DeActivate()
    {
        GLCHECK(glUseProgram(0));
    }

    bool Shader::IsLoaded() const
    {
        return m_state.GetProgram() != 0;
    }

    Shader::State::~State()
    {}

    Shader::State::State()
        : m_program(0)
    {}

    GLuint Shader::State::GetProgram() const
    {
        return m_program;
    }
    void Shader::State::SetProgram(const GLuint program)
    {
        m_program = program;
    }

    GLuint Shader::GetProgram() const
    {
        return m_state.GetProgram();
    }

    GLuint Shader::State::GetUniform(Shader::Uniforms id) const
    {
        return m_uniforms[static_cast<std::underlying_type_t<decltype(id)>>(id)];
    }

    Shader::State Shader::State::LoadFromResource(const std::string & name)
    {
        State res;
        static auto compiledShaders = GetCompiledShaders();
        auto iter = compiledShaders.find(name);
        if(iter!=compiledShaders.end())
        {
            auto loadShader = [](unsigned int shaderType, const std::string& source)
            {
                GLuint shader = glCreateShader(shaderType);
                const char* sourcePtr = source.c_str();
                GLint length = (GLint)source.size();
                GLCHECK(glShaderSource(shader, 1, &sourcePtr, &length));
                GLCHECK(glCompileShader(shader));
                GLint compiled;
                GLCHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled));
                if (!compiled)
                {
                    GLCHECK(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
                    char* buffer = new char[length + 1];
                    GLCHECK(glGetShaderInfoLog(shader, length, &length, buffer));
                    GLCHECK(glDeleteShader(shader));
                    shader = 0;
                    LogError("Error while loading shader: \n{0}\n", buffer);
                    delete[] buffer;
                }
                return shader;
            };

            std::string & vertexShaderSource = std::get<0>(iter->second);
            std::string & fragmentShaderSource = std::get<1>(iter->second);
            auto & uniforms = std::get<2>(iter->second);

            auto vertexShaderObject = loadShader(GL_VERTEX_SHADER, vertexShaderSource);
            auto fragmentShaderObject = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
            
            if (0 == vertexShaderObject || 0 == fragmentShaderObject)
            {
                GLCHECK(glDeleteShader(fragmentShaderObject));
                GLCHECK(glDeleteShader(vertexShaderObject));
            }
            else
            {
                GLuint program = glCreateProgram();

                GLCHECK(glAttachShader(program, vertexShaderObject));
                GLCHECK(glAttachShader(program, fragmentShaderObject));

                GLCHECK(glLinkProgram(program));

                GLint linked;
                GLCHECK(glGetProgramiv(program, GL_LINK_STATUS, &linked));
                if (!linked)
                {
                    GLint length = 0;
                    GLCHECK(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
                    char* buffer = new char[length + 1];
                    GLCHECK(glGetProgramInfoLog(program, length, &length, buffer));
                    LogError("Error while linking shader: \n{0}\n", buffer);
                    delete[] buffer;
                    glDeleteProgram(program);
                    glDeleteShader(fragmentShaderObject);
                    glDeleteShader(vertexShaderObject);
                    program = 0;
                }
                else
                {
                    res.SetProgram(program);
                }
            }
            glCheck();

            if(res.GetProgram() == 0)
            {
                LogError("Requested compile-time shader \"{}\" not loaded succesfully\n",name);
            }
            else
            {
                res.m_uniforms.resize(m_UniformNames.size(),-1);
                for(const auto &uniform:uniforms)
                {
                    GLint uniformLocation = glGetUniformLocation(res.GetProgram(), m_UniformNames[uniform].c_str());
                    if (uniformLocation == -1) 
                    {
                        LogError("binding uniform \"{}\" to shader failed\n",m_UniformNames[uniform]);
                        throw;
                    }
                    res.m_uniforms[static_cast<std::underlying_type_t<Shader::Uniforms>>(uniform)] = uniformLocation;
                }
            }
            return res;
        }
        LogError("Requested shader \"{}\" not found\n",name);
        return Shader::State();
    }

    ShaderCache & ShaderCache::Instance()
    {
        static ShaderCache cache;
        return cache;
    }

    Shader::State & ShaderCache::Get(const std::string & name)
    {
        return Instance().m_shaders.at(name);
    }

    void ShaderCache::LoadAll()
    {
        auto & shaders = Instance().m_shaders;
        if(!shaders.empty())
        {
            UnloadAll();
        }
        auto compiledShaders = GetCompiledShaders();
        for(const auto key:compiledShaders)
        {
            shaders.emplace(key.first,Shader::State::LoadFromResource(key.first));
        }
    }

    void ShaderCache::UnloadAll()
    {
        auto & shaders = Instance().m_shaders;
        for(const auto & key:shaders)
        {
            glDeleteProgram(key.second.GetProgram());
        }
        shaders.clear();
    }
}
