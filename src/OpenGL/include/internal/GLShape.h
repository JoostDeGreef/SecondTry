#pragma once

namespace OpenGL
{
    class GLShape : public Geometry::Shape
    {
    public:
        GLShape(Geometry::Shape && shape)
            : Geometry::Shape(shape)
            , m_model(Mat4::Identity())
        {}

        Mat4 & Model()
        {
            return m_model;
        }

    private:
        Mat4 m_model;
    };
}
