#pragma once

#include <memory>
#include <string>
#include <bitset>
#include <map>
#include <utility>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "fmt/core.h"

#include "Core.h"
#include "Geometry.h"

namespace OpenGL
{
    using TextureCoord = Core::Vector2d;
    using TextureCoordPtr = TextureCoord*;

    using Vertex = Core::Vector3d;
    using VertexPtr = Vertex*;

    using Size = Core::Vector2d;
    using PixelSize = Core::Vector2i;

    using Normal = Core::Vector3d;
    using NormalPtr = Normal*;
}

#include "internal/RGBAColor.h"
#include "internal/RGBColor.h"

namespace OpenGL
{
    using Color = RGBAColorf;
    using ColorPtr = Color*;
}

#include "internal/Mat4.h"
#include "internal/Shader.h"
#include "internal/State.h"

namespace OpenGL
{
    void glCheck();    
    void glCheck(const std::string & file, const int line, const std::string func);
}

#ifdef NDEBUG
#define GLCHECK(f) f
#else
#define GLCHECK(f)    \
  f;                  \
  OpenGL::glCheck(__FILE__,__LINE__,#f); 
#endif

#include "internal/GLShape.h"
#include "internal/Font.h"
#include "internal/Window.h"
#include "internal/Menu.h"
#include "internal/OSSpecific.h"
