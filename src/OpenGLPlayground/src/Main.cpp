#include <cstdlib>
#include <thread>

#include "Log.h"
#include "OpenGL.h"

using namespace std;
using namespace OpenGL;

class UI : public CallbackHandler
{
public:
    UI()
        : CallbackHandler()
        , mainWindow(*this, "Test window 1", Window::Option::HasCloseButton)
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
    void close_callback(const std::shared_ptr<OpenGL::Window>& window) override { window->SetTitle("close_callback(...)"); };
    void content_scale_callback(const std::shared_ptr<OpenGL::Window>& window, float xscale, float yscale) override { window->SetTitle("content_scale_callback(...,{0},{1})", xscale, yscale); };
    void cursor_pos_callback(const std::shared_ptr<OpenGL::Window>& window, double xpos, double ypos) override { window->SetTitle("cursor_pos_callback(...,{0},{1})", xpos, ypos); };
    void error_callback(const std::shared_ptr<OpenGL::Window>& window, int error, const char* description) override 
    { 
        LogError("error_callback(...,{0},{1})", error, description); 
    };
    void focus_callback(const std::shared_ptr<OpenGL::Window>& window, int focused) override { window->SetTitle("focus_callback(...,{0})", focused); };
    void framebuffer_size_callback(const std::shared_ptr<OpenGL::Window>& window, int width, int height) override { window->SetTitle("framebuffer_size_callback(...,{0},{1})", width, height); };
    void iconify_callback(const std::shared_ptr<OpenGL::Window>& window, int iconified) override { window->SetTitle("iconify_callback(...,{0})", iconified); };
    void key_callback(const std::shared_ptr<OpenGL::Window>& window, int key, int scancode, int action, int mods) override { window->SetTitle("key_callback(...,{0},{1},{2},{3})", key, scancode, action, mods); };
    void maximize_callback(const std::shared_ptr<OpenGL::Window>& window, int maximized) override { window->SetTitle("maximize_callback(...,{0})", maximized); };
    void mouse_button_callback(const std::shared_ptr<OpenGL::Window>& window, int button, int action, int mods) override { window->SetTitle("mouse_button_callback(...,{0},{1},{2})", button, action, mods); };
    void pos_callback(const std::shared_ptr<OpenGL::Window>& window, int xpos, int ypos) override { window->SetTitle("pos_callback(...,{0},{1})", xpos,ypos); };
    void size_callback(const std::shared_ptr<OpenGL::Window>& window, int width, int height) override 
    { 
        window->SetTitle("size_callback(...,{0},{1})", width, height); 
        window->GetState2d().Size().Set(width, height);
        window->GetState2d().Projection().SetOrtho(0, width, 0, height, -1, 1);
    };
    void scroll_callback(const std::shared_ptr<OpenGL::Window>& window, double x, double y) override { window->SetTitle("scroll_callback(...,{0},{1})", x, y); };
    void refresh_callback(const std::shared_ptr<OpenGL::Window>& window) override { window->SetTitle("refresh_callback(...)"); };

    void ContextInit(const std::shared_ptr<OpenGL::Window>& window) override;
    void Draw3D(const std::shared_ptr<OpenGL::Window>& window) override;
    void Draw2D(const std::shared_ptr<OpenGL::Window>& window) override;
    void ContextFree(const std::shared_ptr<OpenGL::Window>& window) override;

    int Run();

    void AddShaders();

private:
    enum class ShaderId
    {
        two_d = 1,
    };

    OpenGL::Window mainWindow;
    std::map<ShaderId,OpenGL::Shader> m_shaders;
};

int UI::Run()
{
    if (!mainWindow.Initialize())
    {
        return EXIT_FAILURE;
    }

    OpenGL::Window::EnterMessageLoop();

    return EXIT_SUCCESS;
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
    auto & state2d = window->GetState2d();
    state2d.Model().ApplyAsUniform(uniforms.at(0));
    state2d.Projection().ApplyAsUniform(uniforms.at(1));
    GLCHECK(glUniform3f(uniforms.at(2), color.R, color.G, color.B)); // color

    GLCHECK(glBindVertexArray(VAO)); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    GLCHECK(glDrawArrays(GL_TRIANGLES, 0, 6));

    GLCHECK(glDeleteVertexArrays(1, &VAO));
    GLCHECK(glDeleteBuffers(1, &VBO));

    // draw the actual text
    GLCHECK(glDisable(GL_DEPTH_TEST));
    font.RenderText(text, pos[0], pos[1], 1.0f, OpenGL::RGBColorf(0.5, 0.8f, 0.2f),window->GetState2d());
    GLCHECK(glEnable(GL_DEPTH_TEST));

    // draw the text scaled 
    GLCHECK(glDisable(GL_DEPTH_TEST));
    font.RenderText(text, pos[0], box[1]+10, 0.75f, OpenGL::RGBColorf(0.9, 0.2f, 0.2f),window->GetState2d());
    GLCHECK(glEnable(GL_DEPTH_TEST));

}

void UI::Draw3D(const std::shared_ptr<OpenGL::Window>& window)
{
}

void InitializeLogger()
{
    Logger::AddSync<Logger::SyncCOut>(Logger::Level::Debug);
    Logger::AddSync<Logger::SyncCOut>(Logger::Level::Info);
    Logger::AddSync<Logger::SyncCErr>(Logger::Level::Warning);
    Logger::AddSync<Logger::SyncCErr>(Logger::Level::Error);
    LogInfo("Logger Initialized\n");
}

int RunUILoop()
{
    UI ui;
    return ui.Run();
}

int main(int argc, char* argv[])
{
    InitializeLogger();
    return RunUILoop();
}
