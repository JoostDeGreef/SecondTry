#pragma once

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
};

class Window : std::enable_shared_from_this<Window>
{
public:
    Window(
        CallbackHandler& callbackHandler,
        const std::string& title);

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

    // Window sizing/pos/events
    void Minimize();
    void Maximize();
    void Restore();
    void Close();
    
private:
    class WindowImp;
    std::unique_ptr<WindowImp> m_imp;
};
