#include "OpenGL.h"
#include "Log.h"

namespace Shaders
{
    const auto GetCompiledShaders()
    {
        std::map<std::string,std::tuple<std::string,std::string,std::vector<std::string>,std::vector<std::string>>> shaders;

#include "Shaders/2d.cpp"
#include "Shaders/text.cpp"

        return shaders;
    }

}

namespace OpenGL
{
    Shader::Shader()
      : m_program(0)
      , m_attributes()
      , m_uniforms()
    {}

    Shader::Shader(const std::string & resourceName)
    {
        *this = LoadFromResource(resourceName);
    }

    Shader::Shader(const std::string& vertexShaderSource, 
                   const std::string& fragmentShaderSource, 
                   const std::vector<std::string> & attributes,
                   const std::vector<std::string> & uniforms)
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
                LogError("Error while loading shader: \n{0}\n", buffer);
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
            m_program = glCreateProgram();

            glAttachShader(m_program, vertexShaderObject);
            glAttachShader(m_program, fragmentShaderObject);

            glLinkProgram(m_program);

            GLint linked;
            glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
            if (!linked)
            {
                GLint length = 0;
                glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);
                char* buffer = new char[length + 1];
                glGetProgramInfoLog(m_program, length, &length, buffer);
                LogError("Error while linking shader: \n{0}\n", buffer);
                delete[] buffer;
            }
            else            
            {
                m_attributes.reserve(attributes.size());
                for(const std::string & attributeName : attributes)
                {
                    GLint attributeLocation = glGetAttribLocation(m_program, attributeName.c_str());
       	            if (attributeLocation == -1) 
                    {
                        linked = false;
                        LogError("binding attribute \"{}\" to shader failed\n",attributeName);
                    }
                    m_attributes.emplace_back(attributeLocation);
                }
                m_uniforms.reserve(uniforms.size());
                for(const std::string & uniformName : uniforms)
                {
                    GLint uniformLocation = glGetUniformLocation(m_program, uniformName.c_str());
       	            if (uniformLocation == -1) 
                    {
                        linked = false;
                        LogError("binding uniform \"{}\" to shader failed\n",uniformName);
                    }
                    m_uniforms.emplace_back(uniformLocation);
                }
            }
            if(!linked)
            {
                glDeleteProgram(m_program);
                glDeleteShader(fragmentShaderObject);
                glDeleteShader(vertexShaderObject);
                m_program = 0;
                m_attributes.clear();
                m_uniforms.clear();
            }
        }

    }

    Shader::~Shader()
    {
        glDeleteProgram(m_program);
        m_program = 0;
    }

    const std::tuple<std::vector<GLint>,std::vector<GLint>> Shader::Use()
    {
        glUseProgram(m_program);
        return std::make_tuple(m_attributes,m_uniforms);
    }

    Shader Shader::LoadFromResource(const std::string & name)
    {
       static auto compiledShaders = Shaders::GetCompiledShaders();
       auto iter = compiledShaders.find(name);
       if(iter!=compiledShaders.end())
       {
           Shader shader(std::get<0>(iter->second),std::get<1>(iter->second),std::get<2>(iter->second));
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
