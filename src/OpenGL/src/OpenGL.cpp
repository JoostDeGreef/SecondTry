#include "OpenGL.h"

#include "glad/glad.h"

void OpenGL::glColor(const RGBAColorf& color)
{
    glColor4fv(color.GetRGBA());
}
void OpenGL::glColor(const RGBAColord& color)
{
    glColor4dv(color.GetRGBA());
}

void OpenGL::glColor(const RGBColorf& color)
{
    glColor3fv(color.GetRGB());
}
void OpenGL::glColor(const RGBColord& color)
{
    glColor3dv(color.GetRGB());
}

void OpenGL::glVertex(const Vector3f& point)
{
    glVertex3fv(point.GetData());
}
void OpenGL::glVertex(const Vector3d& point)
{
    glVertex3dv(point.GetData());
}

void OpenGL::glVertex(const double& x, const double& y)
{
    glVertex2d(x, y);
}
void OpenGL::glVertex(const float& x, const float& y)
{
    glVertex2f(x, y);
}

void OpenGL::glNormal(const Vector3f& normal)
{
    glNormal3fv(normal.GetData());
}

void OpenGL::glNormal(const Vector3d& normal)
{
    glNormal3dv(normal.GetData());
}

void OpenGL::glTextureCoord(const Vector2f& coord)
{
    glTexCoord2fv(coord.GetData());
}

void OpenGL::glTextureCoord(const Vector2d& coord)
{
    glTexCoord2dv(coord.GetData());
}

void OpenGL::glMultMatrix(const Quat& quat)
{
    double r[16];
    quat.GetRotationMatrix4(r);
    glMultMatrixd(r);
}

void OpenGL::glLoadMatrix(const Quat& quat)
{
    double r[16];
    quat.GetRotationMatrix4(r);
    glLoadMatrixd(r);
}

