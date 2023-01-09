#pragma once

namespace OpenGL
{
    class Mat4 : public Core::TMatrix<float,4,4>
    {
    public:
        using TMatrix::TMatrix;

        typedef Core::TVector3<element_type> vector_type;

        const element_type * glData() const;

        Mat4 & SetIdentity();
        static Mat4 Identity();

        Mat4 & SetOrtho(
            element_type const & left, 
            element_type const & right, 
            element_type const & bottom, 
            element_type const & top, 
            element_type const & zNear, 
            element_type const & zFar);
        static Mat4 Ortho(
            element_type const & left, 
            element_type const & right, 
            element_type const & bottom, 
            element_type const & top, 
            element_type const & zNear, 
            element_type const & zFar);

        Mat4 & SetPerspective(
            const element_type & verticalFOV, 
            const element_type & aspect, 
            const element_type & near, 
            const element_type & far);
        static Mat4 Perspective(
            const element_type & verticalFOV, 
            const element_type & aspect, 
            const element_type & near, 
            const element_type & far);

        Mat4 & SetLookAt(
            const vector_type & eye,
            const vector_type & center,
            const vector_type & up);
        static Mat4 LookAt(
            const vector_type & eye,
            const vector_type & center,
            const vector_type & up);

        Mat4 & Translate(
            const vector_type & translation);
        static Mat4 Translation(
            const vector_type & translation);
        
        vector_type GetTranslation() const;

        Mat4 Rotated(const Core::Quat & rotation) const;

        void ApplyAsUniform(GLuint uniform) const;
    };
}
