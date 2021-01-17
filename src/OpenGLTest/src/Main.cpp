#include <cstdlib>
#include <thread>

#include "OpenGL.h"

using namespace std;

class UI : public CallbackHandler
{
public:
    UI()
        : CallbackHandler()
        , window1(*this, "Test window 1", Window::Option::HasCloseButton)
        , window2(*this, "Test window 2")
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

    int Run();

private:
    Window window1;
    Window window2;
};

int UI::Run()
{
    if (!window1.Initialize())
    {
        return EXIT_FAILURE;
    }
    if (!window2.Initialize())
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
