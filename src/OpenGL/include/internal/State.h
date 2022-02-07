#pragma once

namespace OpenGL
{
    class State
    {
    public:
        const OpenGL::Mat4 & Model() const { return m_model; }
        const OpenGL::Mat4 & View() const { return m_view; }
        const OpenGL::Mat4 & Projection() const { return m_projection; }
        const Core::Vector2f & Size() const { return m_size; }

        OpenGL::Mat4 & Model() { return m_model; } // used when shape has no model (2d only?)
        OpenGL::Mat4 & View() { return m_view; }
        OpenGL::Mat4 & Projection() { return m_projection; }
        Core::Vector2f & Size() { return m_size; }

        bool & RenderWireframe(bool wireframe) { m_wireframe = wireframe; return m_wireframe; };
        bool & RenderWireframe() { return m_wireframe; };
    private:
        OpenGL::Mat4 m_model;
        OpenGL::Mat4 m_view;
        OpenGL::Mat4 m_projection;
        Core::Vector2f m_size;
        bool m_wireframe = false;
    };
}
