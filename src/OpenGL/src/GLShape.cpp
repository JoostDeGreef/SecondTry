#include "OpenGL.h"
#include "Geometry.h"

using namespace OpenGL;
using namespace Geometry;

void GLShape::Render(const OpenGL::Mat4 &model,const OpenGL::Mat4 &view,const OpenGL::Mat4 &projection)
{
    if(!m_shader.IsLoaded())
    {
        m_shader = OpenGL::Shader::LoadFromResource("3d_phong");
    }
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

        m_lastRenderId = GetLastChangeID();
    }

    auto uniforms = m_shader.Use("model","view","projection","color","lightPos","lightColor","ambientStrength","ambientColor","reflectionStrength","reflectionColor");
    RGBColorf color(0xFF0000);
    RGBColorf lightColor(0xFFFFFF);
    RGBColorf ambientColor(0xFF0000);
    RGBColorf reflectionColor(0xFFFFFF);
    Core::Vector3d lightPos(-2,0,-1);
    model.ApplyAsUniform(uniforms.at(0));
    view.ApplyAsUniform(uniforms.at(1));
    projection.ApplyAsUniform(uniforms.at(2));
    GLCHECK(glUniform3f(uniforms.at(3), color.R, color.G, color.B)); // color
    GLCHECK(glUniform3f(uniforms.at(4), lightPos[0], lightPos[1], lightPos[2])); // light pos
    GLCHECK(glUniform3f(uniforms.at(5), lightColor.R, lightColor.G, lightColor.B)); // light color
    GLCHECK(glUniform1f(uniforms.at(6), 0.1)); // ambient strength
    GLCHECK(glUniform3f(uniforms.at(7), ambientColor.R, ambientColor.G, ambientColor.B)); // ambient color
    GLCHECK(glUniform1f(uniforms.at(8), 0.2)); // reflection strength
    GLCHECK(glUniform3f(uniforms.at(9), reflectionColor.R, reflectionColor.G, reflectionColor.B)); // reflection color

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
    m_shader = OpenGL::Shader();
}
