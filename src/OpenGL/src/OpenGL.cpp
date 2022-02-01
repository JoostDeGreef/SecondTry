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

/**
 * Generates a perspective projection matrix with the given bounds
 *
 * @param {mat4} out mat4 frustum matrix will be written into
 * @param {number} verticalFOV Vertical field of view in radians
 * @param {number} aspect Aspect ratio. typically viewport width/height
 * @param {number} near Near bound of the frustum
 * @param {number} far Far bound of the frustum
 * @returns {mat4} out
 */
OpenGL::Mat4 OpenGL::Mat4::Perspective(
  const double verticalFOV, 
  const double aspect, 
  const double near, 
  const double far) 
{
    double f = 1.0 / tan(verticalFOV / 2);
    double nf = 1.0 / (near - far);
    Mat4 res(
        f / aspect, 0, 0,                   0,
        0,          f, 0,                   0,
        0,          0, (far + near) * nf,   (2 * far * near) * nf,
        0,          0, -1,                  0);
    return res;
};
OpenGL::Mat4 & OpenGL::Mat4::SetPerspective(
  const double verticalFOV, 
  const double aspect, 
  const double near, 
  const double far) 
{
  *this = Perspective(verticalFOV,aspect,near,far);
  return *this;  
}

OpenGL::Mat4 & OpenGL::Mat4::SetLookAt(
    const Core::Vector3d & eye,
    const Core::Vector3d & center,
    const Core::Vector3d & up)
{
  *this = LookAt(eye,center,up);
  return *this;
}
OpenGL::Mat4 OpenGL::Mat4::LookAt(
    const Core::Vector3d & eye,
    const Core::Vector3d & center,
    const Core::Vector3d & up)
{
    auto z = eye - center;

    if (abs(z[0]) < 0.000001 &&
        abs(z[1]) < 0.000001 &&
        abs(z[2]) < 0.000001) 
    {
        return Identity();
    }

    z.Normalize();
    auto x = up.Cross(z).Normalized(); // todo: what is normalization can't be done? default should be 0,0,0
    auto y = z.Cross(x).Normalized(); // todo: what is normalization can't be done? default should be 0,0,0

    Mat4 res(
        x[0], x[1], x[2], -x.InnerProduct(eye),
        y[0], y[1], y[2], -y.InnerProduct(eye),
        z[0], z[1], z[2], -z.InnerProduct(eye),
        0,    0,    0,    1);

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
