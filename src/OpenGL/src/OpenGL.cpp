#include "OpenGL.h"
#include "Log.h"

void OpenGL::glColor(const RGBAColorf& color)
{
    glColor4fv(color.RGBA);
}
void OpenGL::glColor(const RGBAColord& color)
{
    glColor4dv(color.RGBA);
}

void OpenGL::glColor(const RGBColorf& color)
{
    glColor3fv(color.RGB);
}
void OpenGL::glColor(const RGBColord& color)
{
    glColor3dv(color.RGB);
}

void OpenGL::glVertex(const Vector3f& point)
{
    glVertex3fv(point.Raw());
}
void OpenGL::glVertex(const Vector3d& point)
{
    glVertex3dv(point.Raw());
}

void OpenGL::glVertex(const double& x, const double& y)
{
    glVertex2d(x, y);
}
void OpenGL::glVertex(const float& x, const float& y)
{
    glVertex2f(x, y);
}

void OpenGL::glNormal(const Vector3f& normal)
{
    glNormal3fv(normal.Raw());
}

void OpenGL::glNormal(const Vector3d& normal)
{
    glNormal3dv(normal.Raw());
}

void OpenGL::glTextureCoord(const Vector2f& coord)
{
    glTexCoord2fv(coord.Raw());
}

void OpenGL::glTextureCoord(const Vector2d& coord)
{
    glTexCoord2dv(coord.Raw());
}

std::array<float,16> OpenGL::Mat4::Ortho(
    float const & left, 
    float const & right, 
    float const & bottom, 
    float const & top, 
    float const & zNear, 
    float const & zFar)
{
    std::array<float,16> res = {
         2.0f / (right - left), 0.0f,                  0.0f,                   -(right + left) / (right - left),
         0.0f,                  2.0f / (top - bottom), 0.0f,                   -(top + bottom) / (top - bottom),
         0.0f,                  0.0f,                  2.0f / (zNear - zFar),  -(zFar + zNear) / (zFar - zNear),
         0.0f,                  0.0f,                  0.0f,                   1.0f};
    return res;
}

std::array<float,16> OpenGL::Mat4::Identity()
{
    static std::array<float,16> res = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    return res;
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
     msg = fmt::format("{}:{} \"{}\"\n{}", file, line, func, msg);
#ifdef NDEBUG
     LogWarning("{}",msg);
#else  
     throw std::runtime_error(msg);
#endif
   }
}
