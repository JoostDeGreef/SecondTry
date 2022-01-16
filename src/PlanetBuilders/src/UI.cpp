#include <cstdlib>
#include <thread>

#include "Log.h"
#include "UI.h"

using namespace std;
using namespace OpenGL;
using namespace Core;

UI::UI()
    : CallbackHandler()
    , m_mainWindow(*this, "PlanetBuilder", Window::Option::HasCloseButton)
    , m_state2d(m_mainWindow.GetState2d())
    , m_state3d(m_mainWindow.GetState3d())
{}

int UI::Run()
{
    if (!m_mainWindow.Initialize())
    {
        return EXIT_FAILURE;
    }
    else
    {
        OpenGL::Window::EnterMessageLoop();
        return EXIT_SUCCESS;
    }
}

void UI::AddShaders()
{
    m_shaders.emplace(ShaderId::two_d, OpenGL::Shader::LoadFromResource("2d"));
}

void UI::ContextInit(const std::shared_ptr<OpenGL::Window>& window)
{
    AddShaders();
    GLfloat light_position[] = { 4.0, 4.0, 4.0, 0.0 };
    GLCHECK(glClearColor(0.1, 0.1, 0.1, 1.0));

    GLCHECK(glFrontFace(GL_CCW));
    GLCHECK(glEnable(GL_CULL_FACE));
    GLCHECK(glEnable(GL_DEPTH_TEST));
    GLCHECK(glActiveTexture(GL_TEXTURE0));
    GLCHECK(glEnable(GL_BLEND));
    GLCHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCHECK(glBlendEquation(GL_FUNC_ADD));
 // GLCHECK(glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE));
    // glDisable(GL_STENCIL_TEST);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    // glStencilMask(0x00);
    GLCHECK(glDepthFunc(GL_LESS));
}

void UI::ContextFree(const std::shared_ptr<OpenGL::Window>& window)
{
    m_shaders.clear();
}

void UI::Draw2D(const std::shared_ptr<OpenGL::Window>& window)
{
    static OpenGL::Font font(/*"C:/Src/SecondTry/Data/fonts/CHILLER.TTF"*/ "/home/joost/src/SecondTry/Data/fonts/CHILLER.TTF");
    std::string text = "This is sample text";

    // measure the text
    auto sizes = font.CalcTextSize(text,1.0f);
    Vector2f pos(25,25);

    auto box = sizes.GetSize() + pos;
    float vertices[] = {
        pos[0], pos[1], 0.0,
        box[0], pos[1], 0.0,
        pos[0], box[1], 0.0,

        pos[0], box[1], 0.0,
        box[0], pos[1], 0.0,
        box[0], box[1], 0.0,
    }; 

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    GLCHECK(glBindVertexArray(VAO));
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GLCHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    GLCHECK(glEnableVertexAttribArray(0));

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0)); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    GLCHECK(glBindVertexArray(0)); 

    auto uniforms = m_shaders[ShaderId::two_d].Use("model","projection","color");
    RGBColorf color(0xFF0000);
    m_state2d.Model().ApplyAsUniform(uniforms.at(0));
    m_state2d.Projection().ApplyAsUniform(uniforms.at(1));
    GLCHECK(glUniform3f(uniforms.at(2), color.R, color.G, color.B)); // color

    GLCHECK(glBindVertexArray(VAO)); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    GLCHECK(glDrawArrays(GL_TRIANGLES, 0, 6));

    GLCHECK(glDeleteVertexArrays(1, &VAO));
    GLCHECK(glDeleteBuffers(1, &VBO));

    // draw the actual text
    GLCHECK(glDisable(GL_DEPTH_TEST));
    font.RenderText(text, pos[0], pos[1], 1.0f, OpenGL::RGBColorf(0.5, 0.8f, 0.2f),m_state2d);
    GLCHECK(glEnable(GL_DEPTH_TEST));

    // draw the text scaled 
    GLCHECK(glDisable(GL_DEPTH_TEST));
    font.RenderText(text, pos[0], box[1]+10, 0.75f, OpenGL::RGBColorf(0.9, 0.2f, 0.2f),m_state2d);
    GLCHECK(glEnable(GL_DEPTH_TEST));

}

void UI::Draw3D(const std::shared_ptr<OpenGL::Window>& window)
{
}


