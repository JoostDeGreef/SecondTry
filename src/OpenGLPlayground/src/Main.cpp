#include <cstdlib>
#include <thread>

#include "Log.h"
#include "OpenGL.h"
#include "Geometry.h"

using namespace std;
using namespace OpenGL;
using namespace Core;
using namespace Geometry;

class UI : public CallbackHandler
{
public:
    UI()
        : CallbackHandler()
        , mainWindow(*this, "Test window 1", Window::Option::HasCloseButton)
        , m_mouseRotation()
    {
        AddShapes();
    }

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
    void cursor_pos_callback(const std::shared_ptr<OpenGL::Window>& window, double xpos, double ypos) override 
    { 
        window->SetTitle("cursor_pos_callback(...,{0},{1})", xpos, ypos); 
        if(m_mousedown)
        {
            int dx = xpos-m_xpos;
            int dy = ypos-m_ypos;
            Quat qx({1,0,0},-dy*4.1/m_height);
            Quat qy({0,1,0},-dx*4.1/m_width);
            m_mouseRotation *= qx * qy;
        }
        m_xpos = xpos;
        m_ypos = ypos;
    };
    void error_callback(const std::shared_ptr<OpenGL::Window>& window, int error, const char* description) override 
    { 
        LogError("error_callback(...,{0},{1})", error, description); 
    };
    void focus_callback(const std::shared_ptr<OpenGL::Window>& window, int focused) override { window->SetTitle("focus_callback(...,{0})", focused); };
    void framebuffer_size_callback(const std::shared_ptr<OpenGL::Window>& window, int width, int height) override { window->SetTitle("framebuffer_size_callback(...,{0},{1})", width, height); };
    void iconify_callback(const std::shared_ptr<OpenGL::Window>& window, int iconified) override { window->SetTitle("iconify_callback(...,{0})", iconified); };
    void key_callback(const std::shared_ptr<OpenGL::Window>& window, int key, int scancode, int action, int mods) override { window->SetTitle("key_callback(...,{0},{1},{2},{3})", key, scancode, action, mods); };
    void maximize_callback(const std::shared_ptr<OpenGL::Window>& window, int maximized) override { window->SetTitle("maximize_callback(...,{0})", maximized); };
    void mouse_button_callback(const std::shared_ptr<OpenGL::Window>& window, int button, int action, int mods) override 
    { 
        window->SetTitle("mouse_button_callback(...,{0},{1},{2})", button, action, mods); 
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            m_mousedown = (action == GLFW_PRESS);
        } 
        else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        {
            window->GetState3d().RenderWireframe(!window->GetState3d().RenderWireframe());
        }
    };
    void pos_callback(const std::shared_ptr<OpenGL::Window>& window, int xpos, int ypos) override 
    { 
        window->SetTitle("pos_callback(...,{0},{1})", xpos,ypos); 
    };
    void size_callback(const std::shared_ptr<OpenGL::Window>& window, int width, int height) override 
    { 
        window->SetTitle("size_callback(...,{0},{1})", width, height); 
        m_width = width;
        m_height = height;
        window->GetState2d().Size().Set(width, height);
        window->GetState2d().Projection().SetOrtho(0, width, 0, height, -1, 1);
        window->GetState3d().Size().Set(width, height);
        window->GetState3d().Projection().SetPerspective(0.78, width*1.0/height, 0.1, 100);
        window->GetState3d().View().SetLookAt({0,0,1},{0,0,0},{0,1,0});
        window->GetState2d().RenderWireframe(false);
        window->GetState3d().RenderWireframe(false);
    };
    void scroll_callback(const std::shared_ptr<OpenGL::Window>& window, double x, double y) override { window->SetTitle("scroll_callback(...,{0},{1})", x, y); };
    void refresh_callback(const std::shared_ptr<OpenGL::Window>& window) override { window->SetTitle("refresh_callback(...)"); };

    void ContextInit(const std::shared_ptr<OpenGL::Window>& window) override;
    void Draw3D(const std::shared_ptr<OpenGL::Window>& window) override;
    void Draw2D(const std::shared_ptr<OpenGL::Window>& window) override;
    void ContextFree(const std::shared_ptr<OpenGL::Window>& window) override;

    int Run();

    void AddShapes();
    void ClearShapes();
private:
    OpenGL::Window mainWindow;
    std::vector<OpenGL::GLShape> m_shapes;

    bool m_mousedown = false;
    int m_xpos = 0;
    int m_ypos = 0;
    Quat m_mouseRotation;
    int m_width;
    int m_height;
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

void UI::ContextInit(const std::shared_ptr<OpenGL::Window>& window)
{
    GLCHECK(glClearColor(0.1, 0.1, 0.1, 1.0));

    GLCHECK(glFrontFace(GL_CCW));
    GLCHECK(glEnable(GL_CULL_FACE));
    GLCHECK(glEnable(GL_DEPTH_TEST));
    GLCHECK(glActiveTexture(GL_TEXTURE0));
    GLCHECK(glEnable(GL_BLEND));
    GLCHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCHECK(glBlendEquation(GL_FUNC_ADD));
    // glDisable(GL_STENCIL_TEST);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    // glStencilMask(0x00);
    GLCHECK(glDepthFunc(GL_LESS));
}

void UI::ContextFree(const std::shared_ptr<OpenGL::Window>& window)
{
}

void UI::Draw2D(const std::shared_ptr<OpenGL::Window>& window)
{
    // this function should be converted into a 2d rendering class
    static OpenGL::Shader_2d shader;
    static OpenGL::Font font("C:/Src/SecondTry/Data/fonts/CHILLER.TTF" /*"/home/joost/src/SecondTry/Data/fonts/CHILLER.TTF"*/);
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

    auto & state2d = window->GetState2d();
    if(state2d.RenderWireframe())
    {
        // draw the shapes as wireframes
        GLCHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
        GLCHECK(glDisable(GL_DEPTH_TEST));
        GLCHECK(glDisable(GL_CULL_FACE));
    }
    else
    {
        GLCHECK(glEnable(GL_CULL_FACE));
        GLCHECK(glEnable(GL_DEPTH_TEST));
        GLCHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
    }

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

    RGBColorf color(0xFF0000);

    shader.Activate();
    shader.SetModel(state2d.Model());
    shader.SetProjection(state2d.Projection());
    shader.SetColor(color);

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
    auto & state3d = window->GetState3d();
    if(state3d.RenderWireframe())
    {
        GLCHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
        GLCHECK(glDisable(GL_DEPTH_TEST));
        GLCHECK(glDisable(GL_CULL_FACE));
    }
    else
    {
        GLCHECK(glEnable(GL_CULL_FACE));
        GLCHECK(glEnable(GL_DEPTH_TEST));
        GLCHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
    }

    for(auto & shape:m_shapes)
    {
        shape.Render(shape.Model().Rotated(m_mouseRotation), state3d.View(), state3d.Projection());
    }
}

void UI::AddShapes()
{
    m_shapes.emplace_back(Geometry::Shape::Construct::Cube(0.3),Mat4::Translation({-.4,0,-1}));
    m_shapes.back().Translate({-.15,-.15,-.15});
    m_shapes.emplace_back(Geometry::Shape::Construct::Sphere(0.15,-0.8),Mat4::Translation({-0.1,0,-1}));
    m_shapes.back().Translate({0,0,0});
    m_shapes.emplace_back(Geometry::Shape::Construct::Sphere(0.15,-.98),Mat4::Translation({0.2,0,-1}));
    m_shapes.back().Translate({0,0,0});
    m_shapes.emplace_back(Geometry::Shape::Construct::Cylinder(0.3,0.15,-0.98),Mat4::Translation({0.5,0,-1}));
    m_shapes.back().Translate({0,0,-.15});
}

void UI::ClearShapes()
{
    for(auto shape:m_shapes)
    {
        shape.ReleaseRenderCache();
    }
    m_shapes.clear();
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
