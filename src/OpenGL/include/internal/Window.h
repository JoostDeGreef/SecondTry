#pragma once

namespace OpenGL
{

class Window;

class CallbackHandler 
{
public:
    virtual void char_callback(const std::shared_ptr<Window> & window, unsigned int c) {};
    virtual void close_callback(const std::shared_ptr<Window>& window) {};
    virtual void content_scale_callback(const std::shared_ptr<Window>& window, float xscale, float yscale) {};
    virtual void cursor_pos_callback(const std::shared_ptr<Window>& window, double xpos, double ypos) {};
    virtual void error_callback(const std::shared_ptr<Window>& window, int error, const char* description) {};
    virtual void focus_callback(const std::shared_ptr<Window>& window, int focused) {};
    virtual void framebuffer_size_callback(const std::shared_ptr<Window>& window, int width, int height) {};
    virtual void iconify_callback(const std::shared_ptr<Window>& window, int iconified) {};
    virtual void key_callback(const std::shared_ptr<Window>& window, int key, int scancode, int action, int mods) {};
    virtual void maximize_callback(const std::shared_ptr<Window>& window, int maximized) {};
    virtual void mouse_button_callback(const std::shared_ptr<Window>& window, int button, int action, int mods) {};
    virtual void pos_callback(const std::shared_ptr<Window>& window, int xpos, int ypos) {};
    virtual void size_callback(const std::shared_ptr<Window>& window, int width, int height) {};
    virtual void scroll_callback(const std::shared_ptr<Window>& window, double x, double y) {};
    virtual void refresh_callback(const std::shared_ptr<Window>& window) {};

    virtual void ContextInit(const std::shared_ptr<Window>& window) {};
    virtual void Draw3D(const std::shared_ptr<Window>& window) {};
    virtual void Draw2D(const std::shared_ptr<Window>& window) {};
    virtual void ContextFree(const std::shared_ptr<Window>& window) {};
};

class Window // : std::enable_shared_from_this<Window>
{
private:
    typedef std::bitset<16> Options;

public:
    enum class Option
    {
        FullScreen = 0,
        HasCloseButton = 1,
        StayOnTop = 2,
        ToolWindow = 4,
    };

    /*
    ** Create an OpenGL window.
    ** arguments:
    **    callbackHandler : respond to window events
    **    title           : window title
    **    option 0..n     : window options
    */
    template<typename... Args>
    Window(
        CallbackHandler& callbackHandler,
        const std::string& title,
        Option option_1, Args... options)
        : Window(callbackHandler, title, PackOptions(option_1,options...))
    {}
    Window(
        CallbackHandler& callbackHandler,
        const std::string& title)
        : Window(callbackHandler, title, Options{})
    {}

private:
    Window(
        CallbackHandler& callbackHandler,
        const std::string& title,
        const Options& options);

    Options PackOptions(
        Option option_1)
    {
        return Options().set(static_cast<std::underlying_type<Option>::type>(option_1));
    }
    template<typename... Args>
    Options PackOptions(
        Option option_1, Args... options)
    {
        return PackOptions(options...).set(static_cast<std::underlying_type<Option>::type>(option_1));
    }

public:    

    ~Window();

    bool Initialize();
    static void EnterMessageLoop();

    // Window title
    std::string GetTitle() const;
    void SetTitle(const std::string& title);
    template<typename T0, typename... TN>
    void SetTitle(const std::string& title, T0&& t0, TN&&... tn)
    {
        SetTitle(fmt::format(title, t0, tn...));
    }

    // get/set window options
    bool GetOption(const Option& option); // get current option value
    bool SetOption(const Option& option, const bool& value = true); // set option, return previous value
    bool ToggleOption(const Option& option); // invert the option, return previous value
    bool ClearOption(const Option& option) { return SetOption(option, false); }; // clear option, return previous value

    // Window sizing/pos/events
    void Minimize();
    void Maximize();
    void Restore();
    void Close();

    // 2d/3d window state
    OpenGL::State & GetState2d();
    OpenGL::State & GetState3d();

private:
    class WindowImp;
    std::unique_ptr<WindowImp> m_imp;
};

}
