#include "OpenGL.h"

namespace Shaders
{
    const auto GetCompiledShaders()
    {
        std::map<std::string,std::tuple<std::string,std::string>> shaders;

#include "Shaders/2d.cpp"

        return shaders;
    }

}

namespace OpenGL
{
    Shader::Shader()
      : Program(0)
    {}

    Shader::Shader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
      : Shader()
    {
        auto loadShader = [](unsigned int shaderType, const std::string& source)
        {
            GLuint shader = glCreateShader(shaderType);
            const char* sourcePtr = source.c_str();
            GLint length = (GLint)source.size();
            glShaderSource(shader, 1, &sourcePtr, &length);
            glCompileShader(shader);
            GLint compiled;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
            if (!compiled)
            {
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
                char* buffer = new char[length + 1];
                glGetShaderInfoLog(shader, length, &length, buffer);
                glDeleteShader(shader);
                shader = 0;
                //Log::Error("Error while loading shader: \n{0}", buffer);
                delete[] buffer;
            }
            return shader;
        };

        auto vertexShaderObject = loadShader(GL_VERTEX_SHADER, vertexShaderSource);
        auto fragmentShaderObject = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        if (0 == vertexShaderObject || 0 == fragmentShaderObject)
        {
            glDeleteShader(fragmentShaderObject);
            glDeleteShader(vertexShaderObject);
        }
        else
        {
            Program = glCreateProgram();

            glAttachShader(Program, vertexShaderObject);
            glAttachShader(Program, fragmentShaderObject);

            glLinkProgram(Program);

            GLint linked;
            glGetProgramiv(Program, GL_LINK_STATUS, &linked);
            if (!linked)
            {
                GLint length = 0;
                glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &length);
                char* buffer = new char[length + 1];
                glGetProgramInfoLog(Program, length, &length, buffer);
                //Log::Error("Error while linking shader: \n{0}", buffer);
                delete[] buffer;

                glDeleteProgram(Program);
                glDeleteShader(fragmentShaderObject);
                glDeleteShader(vertexShaderObject);
                Program = 0;
            }
        }

    }

    void Shader::Use()
    {
        glUseProgram(Program);
    }

    Shader Shader::LoadFromResource(const std::string & name)
    {
       static auto compiledShaders = Shaders::GetCompiledShaders();
       auto iter = compiledShaders.find(name);
       if(iter!=compiledShaders.end())
       {
           return Shader(std::get<0>(iter->second),std::get<1>(iter->second));
       }
       // TODO: Log something about shader not being found
       return Shader();
    }
}
