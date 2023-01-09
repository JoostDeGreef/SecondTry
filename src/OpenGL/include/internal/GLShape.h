#pragma once

namespace OpenGL
{
    class GLShape : public Geometry::Shape
    {
    public:
        GLShape(Geometry::Shape && shape,const Mat4 model = Mat4::Identity())
            : Geometry::Shape(shape)
            , m_model(model)
            , m_shader()
        {}

        Mat4 & Model()
        {
            return m_model;
        }

        void Render(const OpenGL::Mat4 &model,const OpenGL::Mat4 &view,const OpenGL::Mat4 &projection);
        void ReleaseRenderCache();
    private:
        Mat4 m_model;

        GLuint m_VBO = 0;
        GLuint m_VAO = 0;
        size_t m_lastRenderId = -1;
        OpenGL::Shader_3d_phong m_shader;
        size_t m_vertexCount;
    };
}
