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
    void close_callback(const std::shared_ptr<OpenGL::Window>& window) { window->SetTitle("close_callback(...)"); };
    void content_scale_callback(const std::shared_ptr<OpenGL::Window>& window, float xscale, float yscale) { window->SetTitle("content_scale_callback(...,{0},{1})", xscale, yscale); };
    void cursor_pos_callback(const std::shared_ptr<OpenGL::Window>& window, double xpos, double ypos) { window->SetTitle("cursor_pos_callback(...,{0},{1})", xpos, ypos); };
    void error_callback(const std::shared_ptr<OpenGL::Window>& window, int error, const char* description) { window->SetTitle("error_callback(...,{0},{1})", error, description); };
    void focus_callback(const std::shared_ptr<OpenGL::Window>& window, int focused) { window->SetTitle("focus_callback(...,{0})", focused); };
    void framebuffer_size_callback(const std::shared_ptr<OpenGL::Window>& window, int width, int height) { window->SetTitle("framebuffer_size_callback(...,{0},{1})", width, height); };
    void iconify_callback(const std::shared_ptr<OpenGL::Window>& window, int iconified) { window->SetTitle("iconify_callback(...,{0})", iconified); };
    void key_callback(const std::shared_ptr<OpenGL::Window>& window, int key, int scancode, int action, int mods) { window->SetTitle("key_callback(...,{0},{1},{2},{3})", key, scancode, action, mods); };
    void maximize_callback(const std::shared_ptr<OpenGL::Window>& window, int maximized) { window->SetTitle("maximize_callback(...,{0})", maximized); };
    void mouse_button_callback(const std::shared_ptr<OpenGL::Window>& window, int button, int action, int mods) { window->SetTitle("mouse_button_callback(...,{0},{1},{2})", button, action, mods); };
    void pos_callback(const std::shared_ptr<OpenGL::Window>& window, int xpos, int ypos) { window->SetTitle("pos_callback(...,{0},{1})", xpos,ypos); };
    void size_callback(const std::shared_ptr<OpenGL::Window>& window, int width, int height) { window->SetTitle("size_callback(...,{0},{1})", width, height); };
    void scroll_callback(const std::shared_ptr<OpenGL::Window>& window, double x, double y) { window->SetTitle("scroll_callback(...,{0},{1})", x, y); };
    void refresh_callback(const std::shared_ptr<OpenGL::Window>& window) { window->SetTitle("refresh_callback(...)"); };

    void ContextInit(const std::shared_ptr<OpenGL::Window>& window);
    void Draw3D(const std::shared_ptr<OpenGL::Window>& window);
    void Draw2D(const std::shared_ptr<OpenGL::Window>& window, const int width, const int height);
    void ContextFree(const std::shared_ptr<OpenGL::Window>& window);

    int Run();

    void AddShaders();

private:
    enum class ShaderId
    {
        text = 1,
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
    m_shaders.emplace(ShaderId::text, OpenGL::Shader::LoadFromResource("2d"));
}

void UI::ContextInit(const std::shared_ptr<OpenGL::Window>& window)
{
    AddShaders();
    GLfloat light_position[] = { 4.0, 4.0, 4.0, 0.0 };
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glShadeModel(GL_SMOOTH);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);
    glFrontFace(GL_CCW);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glDisable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilMask(0x00);
    glDepthFunc(GL_LESS);
    //glDepthFunc(GL_GREATER);
}

void UI::ContextFree(const std::shared_ptr<OpenGL::Window>& window)
{
    m_shaders.clear();
}

void UI::Draw2D(const std::shared_ptr<OpenGL::Window>& window, const int width, const int height)
{
    m_shaders[ShaderId::text].Use();
}

void UI::Draw3D(const std::shared_ptr<OpenGL::Window>& window)
{
}

int main(int argc, char* argv[])
{
    Logger::AddSync<Logger::SyncCOut>(Logger::Level::Debug);
    Logger::AddSync<Logger::SyncCOut>(Logger::Level::Info);
    Logger::AddSync<Logger::SyncCErr>(Logger::Level::Warning);
    Logger::AddSync<Logger::SyncCErr>(Logger::Level::Error);

    UI ui;
    return ui.Run();
}
