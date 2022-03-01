#include "OpenGL.h"
#include "Log.h"

namespace Shaders
{
    const auto GetCompiledShaders()
    {
        std::map<std::string,std::tuple<std::string,std::string>> shaders;

#include "Shaders/2d.cpp"
#include "Shaders/3d.cpp"
#include "Shaders/3dphong.cpp"
#include "Shaders/text.cpp"

        return shaders;
    }

}

namespace OpenGL
{
    Shader::Shader()
      : m_state(std::make_shared<State>())
    {}

    Shader::Shader(const std::string& vertexShaderSource, 
                   const std::string& fragmentShaderSource)
      : Shader()
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
                m_state->SetProgram(program);
            }
        }
        glCheck();
    }

    Shader::~Shader()
    {
    }

    bool Shader::IsLoaded() const
    {
        return m_state->GetProgram() != 0;
    }

    Shader::State::~State()
    {
        glDeleteProgram(m_program);
        m_program = 0;
    }

    Shader::State::State()
        : m_program(0)
    {}

    const GLuint Shader::State::GetProgram()
    {
        return m_program;
    }
    void Shader::State::SetProgram(const GLuint program)
    {
        m_program = program;
    }

    // todo: create Uniforms object which caches the locations and can be indexed by name
    std::vector<GLuint> Shader::Use(const std::vector<std::string> & uniforms)
    {
        std::vector<GLuint> res;
        GLCHECK(glUseProgram(GetProgram()));
        for(auto & uniformName:uniforms)
        {
            GLint uniformLocation = glGetUniformLocation(GetProgram(), uniformName.c_str());
       	    if (uniformLocation == -1) 
            {
                LogError("binding uniform \"{}\" to shader failed\n",uniformName);
                throw;
            }
            res.emplace_back(uniformLocation);
        }
        return res;
    }

    const GLuint Shader::GetProgram()
    {
        return m_state->GetProgram();
    }

    Shader Shader::LoadFromResource(const std::string & name)
    {
       static auto compiledShaders = Shaders::GetCompiledShaders();
       auto iter = compiledShaders.find(name);
       if(iter!=compiledShaders.end())
       {
           Shader shader(std::get<0>(iter->second),std::get<1>(iter->second));
           if(shader.GetProgram() == 0)
           {
               LogError("Requested compile-time shader \"{}\" not loaded succesfully\n",name);
           }
           return shader;
       }
       LogError("Requested shader \"{}\" not found\n",name);
       return Shader();
    }
}
