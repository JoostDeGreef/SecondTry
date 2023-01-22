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
#include "Log.h"

using namespace std;
using namespace std::chrono_literals;
using namespace OpenGL;

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

    OpenGL::State & GetState2d() { return m_state2d; }
    OpenGL::State & GetState3d() { return m_state3d; }
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
    OpenGL::State m_state2d;
    OpenGL::State m_state3d;
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

OpenGL::State & Window::GetState2d() { return m_imp->GetState2d(); }
OpenGL::State & Window::GetState3d() { return m_imp->GetState3d(); }

//
// Global OpenGL state
//

Window::WindowImp::OpenGLState::OpenGLState()
    : m_initialisationSucceeded(false)
{
    // setup glfw
    glfwSetErrorCallback(error_callback);
    m_initialisationSucceeded = GLFW_TRUE == glfwInit();
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
    LogDebug("Entering message loop\n");
    while (!m_windows.empty())
    {
//      glfwPollEvents();
        glfwWaitEventsTimeout(0.5);
    }
    LogInfo("All windows closed\n");
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
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_ALPHA_BITS, 8);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        // load the background image from disk before showing the window
        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
//        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), glfwGetPrimaryMonitor(), NULL);
        if (m_window == nullptr)
        {
            LogError("Could not create OpenGL window");
            return false;
        }
        glfwMakeContextCurrent(m_window);
        glfwSetWindowUserPointer(m_window, this);
     // OSSpecific::RemoveWindowDecorations(m_window);
        for (std::size_t i = 0; i < m_options.size(); ++i)
        {
            if (m_options.test(i))
            {
                HandleOption((Option)i);
            }
        }

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            LogError("Could not load all required OpenGL functionality");
            return false;
        }

        // glfwSwapInterval(1);

        SetCallbacks();

        // glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        m_state2d.Model().SetIdentity();
        m_state2d.View().SetIdentity();
        m_state2d.Projection().SetIdentity();
        m_state2d.Size().Set(m_width, m_height);

        m_state3d.Model().SetIdentity();
        m_state3d.View().SetIdentity();
        m_state3d.Projection().SetIdentity();
        m_state3d.Size().Set(m_width, m_height);

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
    LogDebug("Entering main loop\n");

    using namespace std::placeholders;

    auto imp = static_cast<WindowImp*>(glfwGetWindowUserPointer(m_window));

    glfwMakeContextCurrent(m_window);

    ShaderCache::LoadAll();

    m_callbackHandler.ContextInit(imp->m_owner);

    while (!glfwWindowShouldClose(m_window))
    {
        Draw();
        // todo: ratelimit update
        //std::this_thread::sleep_for(std::chrono::seconds(30));
    }

    PurgeCallbacks();

    m_callbackHandler.ContextFree(imp->m_owner);

    ShaderCache::UnloadAll();

    state.RemoveWindow(this);
    glfwDestroyWindow(m_window);
    m_window = nullptr;

    LogDebug("Leaving main loop\n");
}

void Window::WindowImp::Draw()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    auto imp = static_cast<WindowImp*>(glfwGetWindowUserPointer(m_window));

    float ratio = m_width / (float)m_height;
    GLCHECK(glViewport(0, 0, m_width, m_height));
    //glStencilMask(0xFF);
    GLCHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
    //glStencilMask(0x00);

    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_LIGHTING);
    // glDepthMask(GL_TRUE);

    m_callbackHandler.Draw3D(imp->m_owner);

    // glDisable(GL_DEPTH_TEST);
    // glDisable(GL_LIGHTING);
    // glDepthMask(GL_FALSE);

    m_callbackHandler.Draw2D(imp->m_owner); 

    lock.unlock();

    GLCHECK(glfwSwapBuffers(m_window));
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
            if (value)
            {
                glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width/2, mode->height/2, mode->refreshRate);
            }
            else
            {
                glfwSetWindowMonitor(m_window, NULL, mode->width/4, mode->height/4, mode->width, mode->height, 0);
            }
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



