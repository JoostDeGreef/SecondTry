#include "OpenGL.h"
#include "Log.h"

OpenGL::Mat4 & OpenGL::Mat4::SetOrtho(
    float const & left, 
    float const & right, 
    float const & bottom, 
    float const & top, 
    float const & zNear, 
    float const & zFar)
{
    *this = Ortho(left,right,bottom,top,zNear,zFar);
    return *this;
}

OpenGL::Mat4 OpenGL::Mat4::Ortho(
    float const & left, 
    float const & right, 
    float const & bottom, 
    float const & top, 
    float const & zNear, 
    float const & zFar)
{
    Mat4 res(
         2.0f / (right - left), 0.0f,                  0.0f,                   -(right + left) / (right - left),
         0.0f,                  2.0f / (top - bottom), 0.0f,                   -(top + bottom) / (top - bottom),
         0.0f,                  0.0f,                  2.0f / (zNear - zFar),  -(zFar + zNear) / (zFar - zNear),
         0.0f,                  0.0f,                  0.0f,                   1.0f);
    return res;
}

OpenGL::Mat4 & OpenGL::Mat4::SetIdentity()
{
    *this = Identity();
    return *this;
}

OpenGL::Mat4 OpenGL::Mat4::Identity()
{
    static Mat4 res(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
    return res;
}

const float * OpenGL::Mat4::glData() const
{
    return Raw();
}

void OpenGL::Mat4::ApplyAsUniform(GLuint uniform) const
{
    GLCHECK(glUniformMatrix4fv(uniform, 1, true, glData())); 
}

namespace
{
  std::string glGetErrorMessage()
  {
    auto value = glGetError();
    std::string msg;
    switch(value)
    {
        case GL_NO_ERROR:
          break;
        case GL_INVALID_ENUM:
          msg = "glGetError returned GL_INVALID_ENUM\n";
          break;
        case GL_INVALID_VALUE:
          msg = "glGetError returned GL_INVALID_VALUE\n";
          break;
        case GL_INVALID_OPERATION:
          msg = "glGetError returned GL_INVALID_OPERATION\n";
          break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
          msg = "glGetError returned GL_INVALID_FRAMEBUFFER_OPERATION\n";
          break;
        case GL_OUT_OF_MEMORY:
          msg = "glGetError returned GL_OUT_OF_MEMORY\n";
          break;
        case GL_STACK_UNDERFLOW:
          msg = "glGetError returned GL_STACK_UNDERFLOW\n";
          break;
        case GL_STACK_OVERFLOW:
          msg = "glGetError returned GL_STACK_OVERFLOW\n";
          break;
        default:
          msg = fmt::format("glGetError returned {}\n",value);
          break;
    }
    return msg;
  }  
}

void OpenGL::glCheck()
{
   auto msg = glGetErrorMessage();
   if(!msg.empty())
   {
#ifdef NDEBUG
     LogWarning("{}",msg);
#else  
     throw std::runtime_error(msg);
#endif
   }
}
void OpenGL::glCheck(const std::string & file, const int line, const std::string func)
{
   auto msg = glGetErrorMessage();
   if(!msg.empty())
   {
     // Short version of __FILE__ without path requires runtime parsing
     auto sfile = [file]()
     {
#ifdef WIN32
        return (strrchr(file.c_str(), '\\') ? strrchr(file.c_str(), '\\') + 1 : file.c_str());
#else
        return (strrchr(file.c_str(), '/') ? strrchr(file.c_str(), '/') + 1 : file.c_str());
#endif
     };

     msg = fmt::format("{}:{} \"{}\"\n{}", sfile(), line, func, msg);
#ifdef NDEBUG
     LogWarning("{}",msg);
#else  
     throw std::runtime_error(msg);
#endif
   }
}
