#include <cstdlib>
#include <thread>

#include "OpenGL.h"

using namespace std;

static const GLchar* vertex_shader_source = R"SRC(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
out vec3 ourColor;
uniform mat4 transform;
void main() 
{
    gl_Position = transform * vec4(position, 1.0f);
    ourColor = color;
}
)SRC";
static const GLchar* fragment_shader_source = R"SRC(
#version 330 core
in vec3 ourColor;
out vec4 color;
void main() 
{
    color = vec4(ourColor, 1.0f);
}
)SRC";

class UI : public CallbackHandler
{
public:
    UI()
        : CallbackHandler()
        , mainWindow(*this, "Test window 1", Window::Option::HasCloseButton)
        , m_shader(vertex_shader_source, fragment_shader_source)
    {}

    void char_callback(const std::shared_ptr<Window>& window, unsigned int c) 
    { 
        window->SetTitle("char_callback(...,{0})",c); 
        switch (c)
        {
        case 'f':
            window->ToggleOption(Window::Option::FullScreen);
            break;
        case 's':
            window->ToggleOption(Window::Option::StayOnTop);
            break;
        case 't':
            window->ToggleOption(Window::Option::ToolWindow);
            break;
        case 'x':
            window->ToggleOption(Window::Option::HasCloseButton);
            break;
        case 'q':
            window->Close();
            break;
        }
    };
    void close_callback(const std::shared_ptr<Window>& window) { window->SetTitle("close_callback(...)"); };
    void content_scale_callback(const std::shared_ptr<Window>& window, float xscale, float yscale) { window->SetTitle("content_scale_callback(...,{0},{1})", xscale, yscale); };
    void cursor_pos_callback(const std::shared_ptr<Window>& window, double xpos, double ypos) { window->SetTitle("cursor_pos_callback(...,{0},{1})", xpos, ypos); };
    void error_callback(const std::shared_ptr<Window>& window, int error, const char* description) { window->SetTitle("error_callback(...,{0},{1})", error, description); };
    void focus_callback(const std::shared_ptr<Window>& window, int focused) { window->SetTitle("focus_callback(...,{0})", focused); };
    void framebuffer_size_callback(const std::shared_ptr<Window>& window, int width, int height) { window->SetTitle("framebuffer_size_callback(...,{0},{1})", width, height); };
    void iconify_callback(const std::shared_ptr<Window>& window, int iconified) { window->SetTitle("iconify_callback(...,{0})", iconified); };
    void key_callback(const std::shared_ptr<Window>& window, int key, int scancode, int action, int mods) { window->SetTitle("key_callback(...,{0},{1},{2},{3})", key, scancode, action, mods); };
    void maximize_callback(const std::shared_ptr<Window>& window, int maximized) { window->SetTitle("maximize_callback(...,{0})", maximized); };
    void mouse_button_callback(const std::shared_ptr<Window>& window, int button, int action, int mods) { window->SetTitle("mouse_button_callback(...,{0},{1},{2})", button, action, mods); };
    void pos_callback(const std::shared_ptr<Window>& window, int xpos, int ypos) { window->SetTitle("pos_callback(...,{0},{1})", xpos,ypos); };
    void size_callback(const std::shared_ptr<Window>& window, int width, int height) { window->SetTitle("size_callback(...,{0},{1})", width, height); };
    void scroll_callback(const std::shared_ptr<Window>& window, double x, double y) { window->SetTitle("scroll_callback(...,{0},{1})", x, y); };
    void refresh_callback(const std::shared_ptr<Window>& window) { window->SetTitle("refresh_callback(...)"); };

    void setup(const std::shared_ptr<Window>& window) override;
    void draw(const std::shared_ptr<Window>& window) override;
    void finish(const std::shared_ptr<Window>& window) override;

    int Run();

private:
    Window mainWindow;
    OpenGL::Shader m_shader;
};

int UI::Run()
{
    if (!mainWindow.Initialize())
    {
        return EXIT_FAILURE;
    }

    Window::EnterMessageLoop();

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    UI ui;
    return ui.Run();
}

void UI::setup(const std::shared_ptr<Window>& window)
{
    //GLfloat light_position[] = { 4.0, 4.0, 4.0, 0.0 };
    //glClearColor(0.0, 0.0, 0.0, 1.0);
    //glShadeModel(GL_SMOOTH);
    //glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    //glEnable(GL_LIGHT0);
    //glFrontFace(GL_CCW);
    //glEnable(GL_COLOR_MATERIAL);
    //glEnable(GL_LIGHTING);
    ////glEnable(GL_CULL_FACE);
    ////glEnable(GL_DEPTH_TEST);
    //glActiveTexture(GL_TEXTURE0);
    //glEnable(GL_TEXTURE_2D);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendEquation(GL_FUNC_ADD);
    //glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    //glDisable(GL_STENCIL_TEST);
    ////glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    ////glStencilMask(0x00);
    ////glDepthFunc(GL_LESS);
    ////glDepthFunc(GL_GREATER);

    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void UI::draw(const std::shared_ptr<Window>& window)
{
//    //float ratio = m_width / (float)m_height;
//    //glViewport(0, 0, m_width, m_height);
//    //glStencilMask(0xFF);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//    //glStencilMask(0x00);
//    //glMatrixMode(GL_PROJECTION);
//    //glLoadIdentity();
//    //glOrtho(-ratio, ratio, -1.0, 1.0, -1.1, 1.1);
//    //glMatrixMode(GL_MODELVIEW);
//    //glLoadIdentity();
//
////    DrawShapes();
//
//    glDisable(GL_DEPTH_TEST);
//    glDisable(GL_LIGHTING);
//    glDepthMask(GL_FALSE);
//
//    //    DrawFPS();
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
////    glOrtho(0.0f, m_width, 0.0f, m_height, 0.0f, 1.0f);
//    static OpenGL::Font font("C:/Src/SecondTry/Data/CHILLER.TTF");
//    font.RenderText("This is sample text", 25.0f, 25.0f, 1.0f, OpenGL::RGBColorf(0.5, 0.8f, 0.2f));
//
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_LIGHTING);
//    glDepthMask(GL_TRUE);

    static GLfloat vertices[] = {
        /*   Positions              Colors */
             0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
             0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f
    };

    GLuint vbo;
    GLuint vao;

    glClearColor(0, 0, 1, 1);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    /* Position attribute */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    /* Color attribute */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

        glClear(GL_COLOR_BUFFER_BIT);

        m_shader.Use();
        int transform_location = glGetUniformLocation(m_shader.Program, "transform");
        /* THIS is just a dummy transform. */
        GLfloat transform[] = {
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
        };
        auto time = glfwGetTime();
        transform[0] = 2.0f * sin(time);
        transform[5] = 2.0f * cos(time);
        glUniformMatrix4fv(transform_location, 1, GL_FALSE, transform);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    static OpenGL::Font font("C:/Src/SecondTry/Data/CHILLER.TTF");
    float sx = 2.0f / 800;
    float sy = 2.0f / 600;
    font.RenderText("This is sample text", 0.0f, 0.0f, sx, sy, OpenGL::RGBAColorf(0.5, 0.8f, 0.2f, 1.0f));

}

void UI::finish(const std::shared_ptr<Window>& window)
{
}
