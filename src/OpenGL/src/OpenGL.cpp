#include "OpenGL.h"
#include "Log.h"

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
