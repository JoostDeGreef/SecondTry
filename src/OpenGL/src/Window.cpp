#include <cassert>
#include <algorithm>
#include <chrono>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>
#include <iostream>
#include <deque>
#include <set>

#include "OpenGL.h"

using namespace std;
using namespace std::chrono_literals;

class Window::WindowImp
{
private:
    class OpenGLState
    {
    public:
        OpenGLState();
        ~OpenGLState();
        bool InitialisationSucceeded() const;
        void AddWindow(Window::WindowImp* window);
        void RemoveWindow(Window::WindowImp* window);
        void EnterMessageLoop();
    private:
        bool m_initialisationSucceeded;
        std::set<Window::WindowImp*> m_windows;
        static void error_callback(int error, const char* description);
    };
    static OpenGLState state;

public:
    WindowImp(const shared_ptr<Window> & owner, CallbackHandler & callbackHandler, const std::string & title, const Options & options);
    ~WindowImp();

    bool Initialize();
    static void EnterMessageLoop();

    std::string GetTitle() const;
    void SetTitle(const std::string& title);

    void HandleOption(const Option& option, const bool& value = true);
    bool GetOption(const Option& option);
    bool SetOption(const Option& option, const bool & value);
    bool ToggleOption(const Option& option);

    void Minimize();
    void Maximize();
    void Restore();
    void Close();

private:
    void SetCallbacks();
    void PurgeCallbacks();

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void char_callback(GLFWwindow* window, unsigned int c);
    static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void size_callback(GLFWwindow* window, int width, int height);
    static void scroll_callback(GLFWwindow* window, double x, double y);
    static void iconify_callback(GLFWwindow* window, int iconified);
    static void maximize_callback(GLFWwindow* window, int maximized);
    static void focus_callback(GLFWwindow* window, int focused);
    static void pos_callback(GLFWwindow* window, int xpos, int ypos);
    static void close_callback(GLFWwindow* window);
    static void refresh_callback(GLFWwindow* window);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void content_scale_callback(GLFWwindow* window, float xscale, float yscale);
    static void error_callback(Window::WindowImp* window, int error, const char* description);

    void size_callback(int width, int height);
    void close_callback();

    void MainLoop();
    void Draw();
private:
    CallbackHandler & m_callbackHandler;
    std::string m_title;
    Options m_options;
    std::mutex m_mutex;   // mutex for state sanity during callbacks
    GLFWwindow* m_window;
    shared_ptr<Window> m_owner;
    std::thread m_renderThread;
    int m_width;
    int m_height;
};

// 
// Window
//

Window::Window(CallbackHandler& callbackHandler, const std::string& title, const Options& options)
    : m_imp(new WindowImp(shared_ptr<Window>(this, [](Window * window){}), callbackHandler, title, options))
{}

Window::~Window() = default;

bool Window::Initialize() { return m_imp->Initialize(); }
void Window::EnterMessageLoop() { Window::WindowImp::EnterMessageLoop(); }

void Window::SetTitle(const std::string & title) { return m_imp->SetTitle(title); }
std::string Window::GetTitle() const { return m_imp->GetTitle(); }

bool Window::GetOption(const Option& option) { return m_imp->GetOption(option); }
bool Window::SetOption(const Option& option, const bool& value) { return m_imp->SetOption(option, value); }
bool Window::ToggleOption(const Option& option) { return m_imp->ToggleOption(option); }

void Window::Minimize() { m_imp->Minimize(); }
void Window::Maximize() { m_imp->Maximize(); }
void Window::Restore() { m_imp->Restore(); }
void Window::Close() { m_imp->Close(); }

//
// Global OpenGL state
//

Window::WindowImp::OpenGLState::OpenGLState()
    : m_initialisationSucceeded(false)
{
    // setup glfw
    glfwSetErrorCallback(error_callback);
    m_initialisationSucceeded = glfwInit();
}
Window::WindowImp::OpenGLState::~OpenGLState()
{
    // todo: close all open windows

    if (InitialisationSucceeded())
    {
        glfwTerminate();
    }
}

bool Window::WindowImp::OpenGLState::InitialisationSucceeded() const
{
    return m_initialisationSucceeded;
}

void Window::WindowImp::OpenGLState::AddWindow(Window::WindowImp* window)
{
    m_windows.emplace(window);
}

void Window::WindowImp::OpenGLState::RemoveWindow(Window::WindowImp* window)
{
    m_windows.erase(window);
}

void Window::WindowImp::OpenGLState::error_callback(int error, const char* description)
{
    // global error reporting

    // send error to all registered windows
    for (auto window : state.m_windows)
    {
        Window::WindowImp::error_callback(window, error, description);
    }
}

void Window::WindowImp::OpenGLState::EnterMessageLoop()
{
    while (!m_windows.empty())
    {
//      glfwPollEvents();
        glfwWaitEventsTimeout(0.5);
    }
}

Window::WindowImp::OpenGLState Window::WindowImp::state;

// 
// WindowImp
//

Window::WindowImp::WindowImp(const shared_ptr<Window>& owner, CallbackHandler& callbackHandler, const std::string& title, const Options& options)
    : m_callbackHandler(callbackHandler)
    , m_title(title)
    , m_options(options)
    , m_window(nullptr)
    , m_owner(owner)
    , m_renderThread()
    , m_width(800)
    , m_height(600)
{
    state.AddWindow(this);
}

Window::WindowImp::~WindowImp()
{
    m_renderThread.join();
    state.RemoveWindow(this);
}

bool Window::WindowImp::Initialize()
{
    if (state.InitialisationSucceeded())
    {
        glfwWindowHint(GLFW_ALPHA_BITS, 8);
        // load the background image from disk before showing the window
        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
        if (!m_window)
        {
            //Log::Error("Could not create OpenGL window");
            return false;
        }
        glfwSetWindowUserPointer(m_window, this);
        OSSpecific::RemoveWindowDecorations(m_window);
        for (std::size_t i = 0; i < m_options.size(); ++i)
        {
            if (m_options.test(i))
            {
                HandleOption((Option)i);
            }
        }
        glfwMakeContextCurrent(m_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            //Log::Error("Could not load all required OpenGL functionality");
            return false;
        }

        glfwSwapInterval(1);

        SetCallbacks();

        //    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        size_callback(m_width, m_height);

        glfwMakeContextCurrent(nullptr);
        m_renderThread = std::thread([this] 
            { 
                MainLoop(); 
            });

        return true;
    }
    else
    {
        return false;
    }
}

void Window::WindowImp::EnterMessageLoop()
{
    state.EnterMessageLoop();
}

void Window::WindowImp::MainLoop()
{
    using namespace std::placeholders;

    glfwMakeContextCurrent(m_window);
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
    while (!glfwWindowShouldClose(m_window))
    {
        Draw();
        //std::this_thread::sleep_for(std::chrono::seconds(30));
    }

    PurgeCallbacks();
    glfwDestroyWindow(m_window);
    m_window = nullptr;
    state.RemoveWindow(this);
}

void Window::WindowImp::Draw()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    
    float ratio = m_width / (float)m_height;
    glViewport(0, 0, m_width, m_height);
    glStencilMask(0xFF);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glStencilMask(0x00);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-ratio, ratio, -1.0, 1.0, -1.1, 1.1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

//    DrawShapes();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);

//    DrawFPS();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDepthMask(GL_TRUE);

    lock.unlock();

    glfwSwapBuffers(m_window);
}

void Window::WindowImp::SetCallbacks()
{
    glfwSetKeyCallback(m_window, &WindowImp::key_callback);
    glfwSetWindowSizeCallback(m_window, &WindowImp::size_callback);
    glfwSetWindowRefreshCallback(m_window, &WindowImp::refresh_callback);
    glfwSetMouseButtonCallback(m_window, &WindowImp::mouse_button_callback);
    glfwSetCursorPosCallback(m_window, &WindowImp::cursor_pos_callback);
    glfwSetCharCallback(m_window, &WindowImp::char_callback);
    glfwSetScrollCallback(m_window, &WindowImp::scroll_callback);
    glfwSetWindowIconifyCallback(m_window, &WindowImp::iconify_callback);
    glfwSetWindowMaximizeCallback(m_window, &WindowImp::maximize_callback);
    glfwSetWindowFocusCallback(m_window, &WindowImp::focus_callback);
    glfwSetWindowPosCallback(m_window, &WindowImp::pos_callback);
    glfwSetWindowCloseCallback(m_window, &WindowImp::close_callback);
    glfwSetFramebufferSizeCallback(m_window, &WindowImp::framebuffer_size_callback);
    glfwSetWindowContentScaleCallback(m_window, &WindowImp::content_scale_callback);
}

void Window::WindowImp::PurgeCallbacks()
{
    glfwSetKeyCallback(m_window, nullptr);
    glfwSetWindowSizeCallback(m_window, nullptr);
    glfwSetWindowRefreshCallback(m_window, nullptr);
    glfwSetMouseButtonCallback(m_window, nullptr);
    glfwSetCursorPosCallback(m_window, nullptr);
    glfwSetCharCallback(m_window, nullptr);
    glfwSetScrollCallback(m_window, nullptr);
    glfwSetWindowIconifyCallback(m_window, nullptr);
    glfwSetWindowMaximizeCallback(m_window, nullptr);
    glfwSetWindowFocusCallback(m_window, nullptr);
    glfwSetWindowPosCallback(m_window, nullptr);
    glfwSetWindowCloseCallback(m_window, nullptr);
    glfwSetFramebufferSizeCallback(m_window, nullptr);
    glfwSetWindowContentScaleCallback(m_window, nullptr);
}

void Window::WindowImp::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto imp = static_cast<WindowImp*>(glfwGetWindowUserPointer(window));
    imp->m_callbackHandler.key_callback(imp->m_owner, key, scancode, action, mods);
}
void Window::WindowImp::char_callback(GLFWwindow* window, unsigned int c)
{
    auto imp = static_cast<WindowImp*>(glfwGetWindowUserPointer(window));
    imp->m_callbackHandler.char_callback(imp->m_owner, c);
}
void Window::WindowImp::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    auto imp = static_cast<WindowImp*>(glfwGetWindowUserPointer(window));
    imp->m_callbackHandler.cursor_pos_callback(imp->m_owner, xpos, ypos);
}
void Window::WindowImp::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    auto imp = static_cast<WindowImp*>(glfwGetWindowUserPointer(window));
    imp->m_callbackHandler.mouse_button_callback(imp->m_owner, button, action, mods);
}
void Window::WindowImp::size_callback(GLFWwindow* window, int width, int height)
{
    static_cast<WindowImp*>(glfwGetWindowUserPointer(window))->size_callback(width, height);
}
void Window::WindowImp::scroll_callback(GLFWwindow* window, double x, double y)
{
    auto imp = static_cast<WindowImp*>(glfwGetWindowUserPointer(window));
    imp->m_callbackHandler.scroll_callback(imp->m_owner, x, y);
}
void Window::WindowImp::refresh_callback(GLFWwindow* window)
{
    auto imp = static_cast<WindowImp*>(glfwGetWindowUserPointer(window));
    imp->m_callbackHandler.refresh_callback(imp->m_owner);
}
void Window::WindowImp::iconify_callback(GLFWwindow* window, int iconified)
{
    auto imp = static_cast<WindowImp*>(glfwGetWindowUserPointer(window));
    imp->m_callbackHandler.iconify_callback(imp->m_owner, iconified);
}
void Window::WindowImp::maximize_callback(GLFWwindow* window, int maximized)
{
    auto imp = static_cast<WindowImp*>(glfwGetWindowUserPointer(window));
    imp->m_callbackHandler.maximize_callback(imp->m_owner, maximized);
}
void Window::WindowImp::focus_callback(GLFWwindow* window, int focused)
{
    auto imp = static_cast<WindowImp*>(glfwGetWindowUserPointer(window));
    imp->m_callbackHandler.focus_callback(imp->m_owner, focused);
}
void Window::WindowImp::pos_callback(GLFWwindow* window, int xpos, int ypos)
{
    auto imp = static_cast<WindowImp*>(glfwGetWindowUserPointer(window));
    imp->m_callbackHandler.pos_callback(imp->m_owner, xpos,ypos);
}
void Window::WindowImp::close_callback(GLFWwindow* window)
{
    static_cast<WindowImp*>(glfwGetWindowUserPointer(window))->close_callback();
}
void Window::WindowImp::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    auto imp = static_cast<WindowImp*>(glfwGetWindowUserPointer(window));
    imp->m_callbackHandler.framebuffer_size_callback(imp->m_owner, width,height);
}
void Window::WindowImp::content_scale_callback(GLFWwindow* window, float xscale, float yscale)
{
    auto imp = static_cast<WindowImp*>(glfwGetWindowUserPointer(window));
    imp->m_callbackHandler.content_scale_callback(imp->m_owner, xscale, yscale);
}
void Window::WindowImp::error_callback(Window::WindowImp* window, int error, const char* description)
{
    window->m_callbackHandler.error_callback(window->m_owner, error, description);
}


void Window::WindowImp::size_callback(int width, int height)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_width = width;
    m_height = height;
    m_callbackHandler.size_callback(m_owner, width, height);
}

void Window::WindowImp::close_callback()
{
    OSSpecific::CloseOwningWindow(m_window);
    m_callbackHandler.close_callback(m_owner);
}


void Window::WindowImp::SetTitle(const std::string& title)
{
    m_title = title;
    glfwSetWindowTitle(m_window, m_title.c_str());
}
std::string Window::WindowImp::GetTitle() const
{
    return m_title;
}


void Window::WindowImp::HandleOption(const Option& option, const bool& value)
{
    switch (option)
    {
    case Option::FullScreen:
        {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(m_window, value?monitor:NULL, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        break;
    case Option::HasCloseButton:
        OSSpecific::EnableCloseButton(m_window, value);
        break;
    case Option::StayOnTop:
        OSSpecific::StayOnTop(m_window, value);
        break;
    case Option::ToolWindow:
        OSSpecific::MakeToolWindow(m_window, value);
        break;
    default:
        // unhandled option!
        break;
    }
}
bool Window::WindowImp::GetOption(const Option& option)
{
    return m_options.test(static_cast<std::underlying_type<Option>::type>(option));
}
bool Window::WindowImp::SetOption(const Option& option, const bool & value)
{
    bool prev = m_options.test(static_cast<std::underlying_type<Option>::type>(option));
    m_options.set(static_cast<std::underlying_type<Option>::type>(option),value);
    if (prev != value)
    {
        HandleOption(option, value);
    }
    return prev;
}
bool Window::WindowImp::ToggleOption(const Option& option)
{
    bool prev = m_options.test(static_cast<std::underlying_type<Option>::type>(option));
    m_options.set(static_cast<std::underlying_type<Option>::type>(option), !prev);
    HandleOption(option, !prev);
    return prev;
}


void Window::WindowImp::Minimize()
{
    glfwIconifyWindow(m_window);
}
void Window::WindowImp::Maximize()
{
    glfwMaximizeWindow(m_window);
}
void Window::WindowImp::Restore()
{
    glfwRestoreWindow(m_window);
}
void Window::WindowImp::Close()
{
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    close_callback();
}



/*



class State
{
public:
    State()
        : m_pixelSize(-1)
        , m_viewingMode(ViewingMode::Perspective)
    {
        SetFontName("Prida");
    }
    ~State()
    {
        assert(m_window == nullptr); // forgot to call cleanup?
        ClearShapes();
    }
    void KeyCallback(const int key, const int scancode, const int action, const int mods);
    void CharCallback(const unsigned int c);
    void CursorPosCallback(const double& xpos, const double& ypos);
    void MouseButtonCallback(const int button, const int action, const int mods);
    void WindowSizeCallback(const int width, const int height);
    void ScrollCallback(const double x, const double y);

    bool Init();
    bool Cleanup();
    void Run();
    void Draw();
    void DrawMouse();
    void DrawShapes();
    void DrawMenu();
    void DrawFPS();
    void Exit();

    void ClearShapes()
    {
        m_shapes.clear();
    }
    void AddShape(const Geometry::ShapePtr& shape)
    {
        m_shapes.emplace_back(std::move(shape));
    }
    void SetRenderMode(const Geometry::RenderMode renderMode)
    {
        for (auto& shape : m_shapes)
        {
            shape->SetRenderMode(renderMode);
        }
    }
    void SetViewingMode(const Geometry::ViewingMode viewingMode)
    {
        m_viewingMode = viewingMode;
    }
    void SetFontName(const std::string& fontName)
    {
        m_font = std::make_shared<Font>(std::move(std::string(fontName)), Settings::GetInt("FontSize", 40));
        m_font->PixelSize(m_pixelSize);
        m_menu.SetFont(m_font);
    }
    Quat State::CalculateRotation();

    Menu& GetMenu() { return m_menu; }

private:
    // state protection
    std::mutex m_mutex;

    // window
    GLFWwindow* m_window;
    GLsizei m_width;
    GLsizei m_height;
    GLfloat m_ratio;
    double m_x;
    double m_y;
    double m_pixelSize;

    // mouse related
    Quat m_rotation;
    double m_zoom;
    double m_zoomMin;
    double m_zoomMax;
    bool m_mouseDragging;
    Vector2d m_mousePos;
    Vector2d m_mouseDownPos;

    // rendering
    Geometry::ViewingMode m_viewingMode;

    // drawable objects
    std::vector<Geometry::ShapePtr> m_shapes;
    std::shared_ptr<Font> m_font;

    // menu
    Menu m_menu;
};

namespace
{
    std::shared_ptr<Internal::State> GetState()
    {
        static std::weak_ptr<Internal::State> weak_state;
        if (auto shared_state = weak_state.lock())
        {
            return shared_state;
        }
        auto shared_state = make_shared<Internal::State>();
        weak_state = shared_state;
        return shared_state;
    }
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        GetState()->KeyCallback(key, scancode, action, mods);
    }

    static void char_callback(GLFWwindow* window, unsigned int c)
    {
        GetState()->CharCallback(c);
    }

    static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
    {
        GetState()->CursorPosCallback(xpos, ypos);
    }

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        GetState()->MouseButtonCallback(button, action, mods);
    }

    static void error_callback(int error, const char* description)
    {
        //    Log::Error("{0}", description);
    }

    static void window_size_callback(GLFWwindow* window, int width, int height)
    {
        GetState()->WindowSizeCallback(width, height);
    }

    static void scroll_callback(GLFWwindow* window, double x, double y)
    {
        GetState()->ScrollCallback(x, y);
    }

    static void  window_refresh_callback(GLFWwindow* window)
    {
        //Log::Error("window_refresh_callback");
    }
}

namespace Internal
{
    bool State::Init()
    {
        m_zoom = 1.0;
        m_zoomMin = 0.5;
        m_zoomMax = 16;
        m_rotation.Set(Vector3d(1, 0, 0), 0);
        m_mouseDragging = false;

        glfwSetErrorCallback(error_callback);
        if (!glfwInit())
        {
            //        Log::Error("Could not initialize OpenGL");
            return false;
        }
        return true;
    }

    bool State::Cleanup()
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
        glfwTerminate();
        ClearShapes();
        DisposeAllRenderObjects();
        return true;
    }

    void State::Run()
    {
        glfwWindowHint(GLFW_ALPHA_BITS, 8);
        // load the background image from disk before showing the window
        m_window = glfwCreateWindow(800, 600, "Viewer", NULL, NULL);
        if (!m_window)
        {
            glfwTerminate();
            //Log::Error("Could not create OpenGL window");
            exit(EXIT_FAILURE);
        }
        glfwMakeContextCurrent(m_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            //Log::Error("Could not load all required OpenGL functionality");
            exit(EXIT_FAILURE);
        }

        glfwGetFramebufferSize(m_window, &m_width, &m_height);
        WindowSizeCallback(m_width, m_height);

        glfwSwapInterval(1);
        glfwSetKeyCallback(m_window, key_callback);
        glfwSetWindowSizeCallback(m_window, window_size_callback);
        glfwSetWindowRefreshCallback(m_window, window_refresh_callback);
        glfwSetMouseButtonCallback(m_window, mouse_button_callback);
        glfwSetCursorPosCallback(m_window, cursor_pos_callback);
        glfwSetCharCallback(m_window, char_callback);
        glfwSetScrollCallback(m_window, scroll_callback);
        //    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        auto render = [&]()
        {
            glfwMakeContextCurrent(m_window);
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
            while (!glfwWindowShouldClose(m_window))
            {
                Draw();
            }
        };

        glfwMakeContextCurrent(nullptr);
        thread renderThread(render);
        while (!glfwWindowShouldClose(m_window))
        {
            glfwPollEvents();
        }
        renderThread.join();

        glfwSetKeyCallback(m_window, nullptr);
        glfwSetWindowSizeCallback(m_window, nullptr);
        glfwSetWindowRefreshCallback(m_window, nullptr);
    }

    void State::Draw()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        bool text = false;
        glViewport(0, 0, m_width, m_height);
        glStencilMask(0xFF);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glStencilMask(0x00);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-m_ratio, m_ratio, -1.0, 1.0, -1.1, 1.1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        Shaders::Select(Shaders::Type::None);
        DrawShapes();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDepthMask(GL_FALSE);

        Shaders::Select(Shaders::Type::None);
        DrawMenu();

        Shaders::Select(Shaders::Type::None);
        DrawFPS();

        Shaders::Select(Shaders::Type::None);
        DrawMouse();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glDepthMask(GL_TRUE);

        lock.unlock();

        glfwSwapBuffers(m_window);

        HandleDisposedRenderObjects();
    }

    void State::DrawShapes()
    {
        RenderInfo renderInfo;

        auto EdgeDummy = [](const EdgeRaw& edge) {};
        auto EdgeNormal = [](const EdgeRaw& edge)
        {
            glNormal(edge->GetStartNormal());
        };
        auto EdgeTextureCoord = [](const EdgeRaw& edge)
        {
            glTextureCoord(edge->GetStartTextureCoord());
        };

        auto DrawFace2 = [&](const FaceRaw& face,
            std::function<void(const EdgeRaw & edge)> edgeNormal,
            std::function<void(const EdgeRaw & edge)> edgeTextureCoord)
        {
            face->ForEachEdge([&](const EdgeRaw& edge)
                {
                    renderInfo.Push(edge);
                    edgeNormal(edge);
                    edgeTextureCoord(edge);
                    glVertex(*edge->GetStartVertex());
                    renderInfo.Pop();
                });
        };
        auto DrawFace1 = [&](const FaceRaw& face, std::function<void(const EdgeRaw & edge)> edgeNormal)
        {
            if (face->GetStartEdge()->GetStartTextureCoord())
            {
                DrawFace2(face, edgeNormal, EdgeTextureCoord);
            }
            else
            {
                DrawFace2(face, edgeNormal, EdgeDummy);
            }
        };
        auto DrawFace0 = [&](const FaceRaw& face)
        {
            if (face->GetStartEdge()->GetStartNormal())
            {
                DrawFace1(face, EdgeNormal);
            }
            else if (face->GetNormal())
            {
                glNormal(*face->GetNormal());
                DrawFace1(face, EdgeDummy);
            }
            else
            {
                DrawFace1(face, EdgeDummy);
            }
        };
        auto DrawFaces = [&](const HullRaw& hull)
        {
            const auto& faces = hull->GetFaces();
            for (auto face : faces)
            {
                switch (face->GetEdgeCount())
                {
                case 0:
                case 1:
                case 2:
                    assert(false);
                case -1:
                default:
                    glBegin(GL_POLYGON);
                    break;
                case 3:
                    glBegin(GL_TRIANGLES);
                    break;
                }
                renderInfo.Push(face);
                DrawFace0(face);
                renderInfo.Pop();
                glEnd();
            }
        };
        // todo: do not draw patches which are outside the view.
        //Projection<double> projection;
        //Vertex front = projection.Project(Vector2d(projection.GetWidth()/2, projection.GetHeight()/2), -100);
        //front.Normalize();
        auto DrawHull = [&](const HullRaw& hull)
        {
            HullRenderObject& renderObject = GetRenderObject(hull);
            unsigned int updateNeeded = renderObject.NeedsUpdate();
            unsigned int displayList = renderObject.GetDisplayList();
            auto UpdateDisplayList = [&](std::unique_lock<std::mutex>&& lock)
            {
                if (lock)
                {
                    displayList = glGenLists(1);
                    glNewList(displayList, GL_COMPILE);
                    renderInfo.Push(hull);
                    //glBindTexture(GL_TEXTURE_2D, hull->GetTextureId());
                    DrawFaces(hull);
                    renderInfo.Pop();
                    glEndList();
                    renderObject.SetDisplayList(displayList);
                }
                else
                {
                    renderObject.Invalidate();
                }
            };
            if (updateNeeded > 5 || displayList == 0)
            {
                UpdateDisplayList(hull->GetLock());
            }
            else if (updateNeeded > 0)
            {
                UpdateDisplayList(hull->TryGetLock());
            }
            glCallList(displayList);
        };

        // todo: adjust far clipping plane?
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        if (m_viewingMode == ViewingMode::Orthogonal)
        {
            glOrtho(-m_ratio / m_zoom, m_ratio / m_zoom, -1. / m_zoom, 1. / m_zoom, -2, 2);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }
        else
        {
            double scale = 0.9 / m_zoom;
            glFrustum(-m_ratio * scale, m_ratio * scale, -1. * scale, 1. * scale, 3, 7);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        }

        glMultMatrix(CalculateRotation());

        for (const Geometry::ShapeRaw& shape : m_shapes)
        {
            renderInfo.Push(shape);
            shape->ForEachHull(DrawHull);
            renderInfo.Pop();
        }

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-m_ratio, m_ratio, -1., 1., 1.1, -1.1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

    }

    void State::DrawMouse()
    {
        // TODO
    }

    void State::DrawMenu()
    {
        assert(m_font);
        m_menu.Draw(m_width, m_height, m_x, m_y);
    }

    void State::DrawFPS()
    {
        static std::deque<double> times;
        times.push_back(glfwGetTime());
        if (times.size() > 41)
        {
            times.pop_front();
        }
        if (Settings::GetBool("ShowFPS") && times.size() > 13)
        {
            double f = (times.size() - 1) / (times.back() - times.front());
            std::string fps = (boost::format("FPS %1$.0f") % f).str();
            auto size = m_font->GetSize(fps);
            m_font->Color(Geometry::Color::Red()).Draw(m_x - size[0] - 2 * m_pixelSize, 2 * m_pixelSize - m_y, fps);
        }
    }

    Quat State::CalculateRotation()
    {
        if (m_mouseDragging)
        {
            Vector2d dir = (m_mousePos - m_mouseDownPos) / m_zoom;
            Vector3d axis(dir[1], -dir[0], 0);
            double angle = axis.Length();
            if (angle > 0)
            {
                axis = (axis / angle);
                axis = m_rotation.Transform(axis);
                return Quat(axis, angle) * m_rotation;
            }
            return m_rotation;
        }
        return m_rotation;
    }

    void State::KeyCallback(const int key, const int scancode, const int action, const int mods)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_menu.HandleKey(key, scancode, action, mods))
        {
        }
    }

    void State::CharCallback(const unsigned int c)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_menu.HandleChar(c))
        {
        }
    }

    void State::CursorPosCallback(const double& xpos, const double& ypos)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        double x = (2 * xpos / m_width - 1) * m_x;
        double y = (2 * ypos / m_height - 1) * m_y;
        m_mousePos.Set(x, -y);

        if (!m_menu.HandleCursorPos(xpos, ypos))
        {
        }

    }

    void State::MouseButtonCallback(const int button, const int action, const int mods)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_menu.HandleMouseButton(button, action, mods))
        {
            if (GLFW_MOUSE_BUTTON_LEFT == button && 0 == mods)
            {
                if (GLFW_PRESS == action)
                {
                    m_rotation = CalculateRotation();
                    m_rotation.Normalize();
                    m_mouseDragging = true;
                    m_mouseDownPos = m_mousePos;
                }
                else if (GLFW_RELEASE == action)
                {
                    m_rotation = CalculateRotation();
                    m_rotation.Normalize();
                    m_mouseDragging = false;
                }
            }
        }
    }

    void State::WindowSizeCallback(const int width, const int height)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_width = width;
        m_height = height;
        m_ratio = m_width / (float)m_height;
        m_x = 1;
        m_y = 1;
        if (m_ratio >= 1)
        {
            m_x = m_ratio;
        }
        else
        {
            m_y = 1 / m_ratio;
        }
        m_pixelSize = 2.0 / m_height;
        assert(m_font);
        m_font->PixelSize(m_pixelSize);
        if (!m_menu.HandleWindowSize(width, height))
        {
        }
    }

    void State::ScrollCallback(const double x, const double y)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        double factor = 1 + y / 10;
        m_zoom = Numerics::Clamp(m_zoom * factor, m_zoomMin, m_zoomMax);
        if (!m_menu.HandleScroll(x, y))
        {
        }
    }

    void State::Exit()
    {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }
} // namespace Internal


UserInterface::UserInterface()
    : m_state(GetState())
{
}
UserInterface::~UserInterface()
{
}

bool UserInterface::Init()
{
    return m_state->Init();
}

void UserInterface::Run()
{
    return m_state->Run();
}

bool UserInterface::Cleanup()
{
    return m_state->Cleanup();
}

void UserInterface::Draw()
{
    return m_state->Draw();
}

void UserInterface::AddShape(const Geometry::ShapePtr& shape)
{
    return m_state->AddShape(shape);
}

void UserInterface::ClearShapes()
{
    return m_state->ClearShapes();
}

void UserInterface::SetRenderMode(const Geometry::RenderMode renderMode)
{
    return m_state->SetRenderMode(renderMode);
}

void UserInterface::SetViewingMode(const Geometry::ViewingMode viewingMode)
{
    return m_state->SetViewingMode(viewingMode);
}

void UserInterface::SetFontName(const std::string& fontName)
{
    return m_state->SetFontName(fontName);
}

void UserInterface::Exit()
{
    return m_state->Exit();
}

Menu& UserInterface::GetMenu()
{
    return m_state->GetMenu();
}


*/






