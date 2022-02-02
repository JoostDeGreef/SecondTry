#pragma once

namespace OpenGL
{
    class GLShape : public Geometry::Shape
    {
    public:
        GLShape(Geometry::Shape && shape,const Mat4 model = Mat4::Identity())
            : Geometry::Shape(shape)
            , m_model(model)
        {}

        Mat4 & Model()
        {
            return m_model;
        }

    private:
        Mat4 m_model;
    };
}
