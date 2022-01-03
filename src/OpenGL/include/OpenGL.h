#pragma once

#include <memory>
#include <string>
#include <bitset>
#include <map>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "fmt/core.h"

#include "Core.h"

namespace OpenGL
{
    using TextureCoord = Vector2d;
    using TextureCoordPtr = TextureCoord*;

    using Vertex = Vector3d;
    using VertexPtr = Vertex*;

    using Size = Vector2d;
    using PixelSize = Vector2i;

    using Normal = Vector3d;
    using NormalPtr = Normal*;
}

#include "internal/RGBAColor.h"
#include "internal/RGBColor.h"

namespace OpenGL
{
    using Color = RGBAColorf;
    using ColorPtr = Color*;
}

#include "internal/Shader.h"
#include "internal/Font.h"

namespace OpenGL
{
	void glColor(const RGBAColorf& color);
    void glColor(const RGBAColord& color);
    void glColor(const RGBColorf& color);
    void glColor(const RGBColord& color);
    inline void glColor(const ColorPtr& color) { glColor(*color); }

    void glVertex(const Vector3f& point);
    void glVertex(const Vector3d& point);
    inline void glVertex(const VertexPtr& point) { glVertex(*point); }
    void glVertex(const double& x, const double& y);
    void glVertex(const float& x, const float& y);

    void glNormal(const Vector3f& normal);
    void glNormal(const Vector3d& normal);
    inline void glNormal(const NormalPtr& normal) { glNormal(*normal); }

    void glTextureCoord(const Vector2f& coord);
    void glTextureCoord(const Vector2d& coord);
    inline void glTextureCoord(const TextureCoordPtr& coord) { glTextureCoord(*coord); }

    namespace Mat4
    {
      std::array<float,16> Identity();
      std::array<float,16> Ortho(
          float const & left, 
          float const & right, 
          float const & bottom, 
          float const & top, 
          float const & zNear, 
          float const & zFar);
    }

    void glCheck();    
    void glCheck(const std::string & file, const int line, const std::string func);
};

#ifdef NDEBUG
#define GLCHECK(f) {  \
  f;                  \
}
#else
// Short version of __FILE__ without path requires runtime parsing
#ifdef WIN32
#define __SFILE__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __SFILE__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
#define GLCHECK(f) {  \
  f;                  \
  OpenGL::glCheck(__SFILE__,__LINE__,#f);  \
}
#endif

#include "internal/Window.h"
#include "internal/OSSpecific.h"
