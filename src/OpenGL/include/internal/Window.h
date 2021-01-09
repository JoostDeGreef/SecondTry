#pragma once

class CallbackHandler 
{
public:
    virtual void key_callback(int key, int scancode, int action, int mods) {};
    virtual void char_callback(unsigned int c) {};
    virtual void cursor_pos_callback(double xpos, double ypos) {};
    virtual void mouse_button_callback(int button, int action, int mods) {};
    virtual void size_callback(int width, int height) {};
    virtual void scroll_callback(double x, double y) {};
    virtual void refresh_callback() {};
    virtual void iconify_callback(int iconified) {};
    virtual void maximize_callback(int maximized) {};
    virtual void focus_callback(int focused) {};
};


class Window
{
public:
    Window(
        CallbackHandler& callbackHandler,
        const std::string& title);

    ~Window();

    bool Init();
    void Run();
    void Cleanup();

    // Window title
    std::string GetTitle() const;
    void SetTitle(const std::string& title);
    template<typename T0, typename... TN>
    void SetTitle(const std::string& title, T0&& t0, TN&&... tn)
    {
        SetTitle(fmt::format(title, t0, tn...));
    }

    // Window sizing
    void Minimize();
    void Maximize();
    void Restore();

private:
    class WindowImp;
    std::unique_ptr<WindowImp> m_imp;
};
