#include "OpenGL.h"

void OpenGL::glColor(const RGBAColorf& color)
{
    glColor4fv(color.RGBA);
}
void OpenGL::glColor(const RGBAColord& color)
{
    glColor4dv(color.RGBA);
}

void OpenGL::glColor(const RGBColorf& color)
{
    glColor3fv(color.RGB);
}
void OpenGL::glColor(const RGBColord& color)
{
    glColor3dv(color.RGB);
}

void OpenGL::glVertex(const Vector3f& point)
{
    glVertex3fv(point.Raw());
}
void OpenGL::glVertex(const Vector3d& point)
{
    glVertex3dv(point.Raw());
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
    glNormal3fv(normal.Raw());
}

void OpenGL::glNormal(const Vector3d& normal)
{
    glNormal3dv(normal.Raw());
}

void OpenGL::glTextureCoord(const Vector2f& coord)
{
    glTexCoord2fv(coord.Raw());
}

void OpenGL::glTextureCoord(const Vector2d& coord)
{
    glTexCoord2dv(coord.Raw());
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

