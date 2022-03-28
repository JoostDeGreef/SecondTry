#include "OpenGL.h"
#include "Geometry.h"
#include "Log.h"

using namespace OpenGL;
using namespace Geometry;
using namespace Core;

void GLShape::Render(const OpenGL::Mat4 &model,const OpenGL::Mat4 &view,const OpenGL::Mat4 &projection)
{
    if(m_VAO == 0)
    {
        GLCHECK(glGenVertexArrays(1, &m_VAO));
    }
    if(m_VBO == 0)
    {
        GLCHECK(glGenBuffers(1, &m_VBO));
    }

    if(m_lastRenderId != GetLastChangeID())
    {
        m_lastRenderId = GetLastChangeID();

        // LogWarning("Rebuilding VBO object\n");

        GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
        auto vertices = DrawWithNormals();
        m_vertexCount = vertices.size()/6; // /6 => vertex+normal
        GLCHECK(glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vertices.front()), vertices.data(), GL_STATIC_DRAW));

        GLCHECK(glBindVertexArray(m_VAO)); 

        // vertex
        GLCHECK(glEnableVertexAttribArray(0));
        GLCHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(vertices.front()), (void*)(0 * sizeof(vertices.front()))));

        // normal
        GLCHECK(glEnableVertexAttribArray(1));
        GLCHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(vertices.front()), (void*)(3 * sizeof(vertices.front()))));

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0)); 

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        GLCHECK(glBindVertexArray(0)); 
    }

    RGBColorf color(0xFF0000);
    RGBColorf lightColor(0xFFFFFF);
    RGBColorf ambientColor(0xFF0000);
    RGBColorf reflectionColor(0xFFFFFF);
    Core::Vector3d lightPos(2,2,2);

    m_shader.Activate();
    m_shader.SetModel(model);
    m_shader.SetProjection(projection);
    m_shader.SetView(view);
    m_shader.SetColor(color);
    m_shader.SetLightPos(lightPos);
    m_shader.SetLightColor(lightColor);
    m_shader.SetAmbientStrength(0.1);
    m_shader.SetAmbientColor(ambientColor);
    m_shader.SetReflectionStrength(0.2);
    m_shader.SetReflectionColor(reflectionColor);

    GLCHECK(glBindVertexArray(m_VAO)); 
    GLCHECK(glDrawArrays(GL_TRIANGLES, 0, m_vertexCount)); 
    GLCHECK(glBindVertexArray(0));
}

void GLShape::ReleaseRenderCache()
{
    GLCHECK(glDeleteVertexArrays(1, &m_VAO));
    GLCHECK(glDeleteBuffers(1, &m_VBO));
    m_VAO = 0;
    m_VBO = 0;
    m_shader.Unload();
}
