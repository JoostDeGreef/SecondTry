#include "OpenGL.h"

namespace Shaders
{
    const auto GetCompiledShaders()
    {
        std::map<std::string,std::tuple<std::string,std::string,std::vector<std::string>>> shaders;

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
    {}

    Shader::Shader(const std::string & resourceName)
    {
        *this = LoadFromResource(resourceName);
    }

    Shader::Shader(const std::string& vertexShaderSource, 
                   const std::string& fragmentShaderSource, 
                   const std::vector<std::string> & attributes)
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
                //TODO: Log::Error("Error while loading shader: \n{0}", buffer);
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
                //TODO: Log::Error("Error while linking shader: \n{0}", buffer);
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
                        //TODO: Log error binding attribute to shader
                    }
                    m_attributes.emplace_back(attributeLocation);
                }
            }
            if(!linked)
            {
                glDeleteProgram(m_program);
                glDeleteShader(fragmentShaderObject);
                glDeleteShader(vertexShaderObject);
                m_program = 0;
                m_attributes.clear();
            }
        }

    }

    Shader::~Shader()
    {
        glDeleteProgram(m_program);
        m_program = 0;
    }

    const std::vector<GLint> & Shader::Use()
    {
        glUseProgram(m_program);
        return m_attributes;
    }

    Shader Shader::LoadFromResource(const std::string & name)
    {
       static auto compiledShaders = Shaders::GetCompiledShaders();
       auto iter = compiledShaders.find(name);
       if(iter!=compiledShaders.end())
       {
           return Shader(std::get<0>(iter->second),std::get<1>(iter->second),std::get<2>(iter->second));
       }
       // TODO: Log something about shader not being found
       return Shader();
    }
}
