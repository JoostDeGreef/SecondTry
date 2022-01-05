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

namespace OpenGL
{
    void glCheck();    
    void glCheck(const std::string & file, const int line, const std::string func);

    class Mat4 : public TMatrix<float,4,4>
    {
    public:
        using TMatrix::TMatrix;

        const float * glData() const;

        Mat4 & SetIdentity();
        static Mat4 Identity();

        Mat4 & SetOrtho(
            float const & left, 
            float const & right, 
            float const & bottom, 
            float const & top, 
            float const & zNear, 
            float const & zFar);
        static Mat4 Ortho(
            float const & left, 
            float const & right, 
            float const & bottom, 
            float const & top, 
            float const & zNear, 
            float const & zFar);

        void ApplyAsUniform(GLuint uniform) const;
    };

    class State
    {
    public:
        const Mat4 & Model() const { return m_model; }
        const Mat4 & View() const { return m_view; }
        const Mat4 & Projection() const { return m_projection; }
        const Vector2f & Size() const { return m_size; }

        Mat4 & Model() { return m_model; }
        Mat4 & View() { return m_view; }
        Mat4 & Projection() { return m_projection; }
        Vector2f & Size() { return m_size; }

    private:
        Mat4 m_model;
        Mat4 m_view;
        Mat4 m_projection;
        Vector2f m_size;
    };
};

#ifdef NDEBUG
#define GLCHECK(f) f
#else
#define GLCHECK(f)    \
  f;                  \
  OpenGL::glCheck(__FILE__,__LINE__,#f); 
#endif

#include "internal/Font.h"
#include "internal/Window.h"
#include "internal/OSSpecific.h"
