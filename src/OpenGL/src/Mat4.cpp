#include "OpenGL.h"

using namespace OpenGL;

Mat4 & Mat4::SetOrtho(
    element_type const & left, 
    element_type const & right, 
    element_type const & bottom, 
    element_type const & top, 
    element_type const & zNear, 
    element_type const & zFar)
{
    *this = Ortho(left,right,bottom,top,zNear,zFar);
    return *this;
}

Mat4 Mat4::Ortho(
    element_type const & left, 
    element_type const & right, 
    element_type const & bottom, 
    element_type const & top, 
    element_type const & zNear, 
    element_type const & zFar)
{
    const element_type rl = 1.0 / (right - left);
    const element_type tb = 1.0 / (top - bottom);
    const element_type nf = 1.0 / (zNear - zFar);
    Mat4 res(
         2.0 * rl,              0.0,                   0.0,                   0.0,
         0.0,                   2.0 *tb,               0.0,                   0.0,
         0.0,                   0.0,                   2.0 * nf,              0.0,
         -(right + left) * rl,  -(top + bottom) * tb,  -(zFar + zNear) * nf,  1.0);
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
  const element_type & verticalFOV, 
  const element_type & aspect, 
  const element_type & near, 
  const element_type & far) 
{
    element_type f = 1.0 / tan(verticalFOV / 2);
    element_type nf = 1.0 / (near - far);
    Mat4 res(
        f / aspect, 0, 0,                     0,
        0,          f, 0,                     0,
        0,          0, (far + near) * nf,     -1,
        0,          0, (2 * far * near) * nf, 0);
    return res;
};
OpenGL::Mat4 & OpenGL::Mat4::SetPerspective(
  const element_type & verticalFOV, 
  const element_type & aspect, 
  const element_type & near, 
  const element_type & far) 
{
  *this = Perspective(verticalFOV,aspect,near,far);
  return *this;  
}

OpenGL::Mat4 & OpenGL::Mat4::SetLookAt(
    const vector_type & eye,
    const vector_type & center,
    const vector_type & up)
{
  *this = LookAt(eye,center,up);
  return *this;
}
OpenGL::Mat4 OpenGL::Mat4::LookAt(
    const vector_type & eye,
    const vector_type & center,
    const vector_type & up)
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
        x[0],                  y[0],                  z[0],                  0,
        x[1],                  y[1],                  z[1],                  0,
        x[2],                  y[2],                  z[2],                  0,
        -x.InnerProduct(eye),  -y.InnerProduct(eye),  -z.InnerProduct(eye),  1);

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

OpenGL::Mat4 & OpenGL::Mat4::Translate(
    const vector_type & translation)
{
    auto & M = *this;
    M[12] += M[0]*translation[0] + M[4]*translation[1] + M[ 8]*translation[2];
    M[13] += M[1]*translation[0] + M[5]*translation[1] + M[ 9]*translation[2];
    M[14] += M[2]*translation[0] + M[6]*translation[1] + M[10]*translation[2];
    M[15] += M[3]*translation[0] + M[7]*translation[1] + M[11]*translation[2];
    return M;
}

Mat4 Mat4::Translation(
    const vector_type & translation)
{
    auto M = Mat4::Identity();
    M[12] += translation[0];
    M[13] += translation[1];
    M[14] += translation[2];
    return M;
}

const float * OpenGL::Mat4::glData() const
{
    return Raw();
}

namespace Helper
{
    void ApplyAsUniform(GLuint uniform, const float * data)
    {
        GLCHECK(glUniformMatrix4fv(uniform, 1, false, data)); 
    }
    void ApplyAsUniform(GLuint uniform, const double * data)
    {
        GLCHECK(glUniformMatrix4dv(uniform, 1, false, data)); 
    }
}

void OpenGL::Mat4::ApplyAsUniform(GLuint uniform) const
{
    Helper::ApplyAsUniform(uniform,glData());
}

OpenGL::Mat4::vector_type OpenGL::Mat4::GetTranslation() const
{
    auto & M = *this;
    return vector_type(M[12],M[13],M[14]);
}

OpenGL::Mat4 OpenGL::Mat4::Rotated(const Core::Quat & rotation) const
{
    Core::Quat::value_type R[9];
    rotation.GetRotationMatrix3(R);
    auto & M = *this;
    auto res = *this;
    res[ 0] = M[ 0]*R[0] + M[ 1]*R[3] + M[ 2]*R[6];
    res[ 1] = M[ 0]*R[1] + M[ 1]*R[4] + M[ 2]*R[7];
    res[ 2] = M[ 0]*R[2] + M[ 1]*R[5] + M[ 2]*R[8];
    res[ 4] = M[ 4]*R[0] + M[ 5]*R[3] + M[ 6]*R[6];
    res[ 5] = M[ 4]*R[1] + M[ 5]*R[4] + M[ 6]*R[7];
    res[ 6] = M[ 4]*R[2] + M[ 5]*R[5] + M[ 6]*R[8];
    res[ 8] = M[ 8]*R[0] + M[ 9]*R[3] + M[10]*R[6];
    res[ 9] = M[ 8]*R[1] + M[ 9]*R[4] + M[10]*R[7];
    res[10] = M[ 8]*R[2] + M[ 9]*R[5] + M[10]*R[8];
    return res;
}
