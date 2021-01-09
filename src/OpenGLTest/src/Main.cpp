#include <cstdlib>

#include "OpenGL.h"

class UI : public CallbackHandler
{
public:
    UI()
        : CallbackHandler()
        , window(*this, "Test window")
    {}

    void key_callback(int key, int scancode, int action, int mods) override;
    void char_callback(unsigned int c) override;
    void cursor_pos_callback(double xpos, double ypos) override;
    void mouse_button_callback(int button, int action, int mods) override;
    void size_callback(int width, int height) override;
    void scroll_callback(double x, double y) override;
    void refresh_callback() override;
    void iconify_callback(int iconified) override;
    void maximize_callback(int maximized) override;
    void focus_callback(int focused) override;

    int Run();

private:
    Window window;
};

void UI::key_callback(int key, int scancode, int action, int mods)
{}
void UI::char_callback(unsigned int c)
{
    std::cout << (char)c;
}
void UI::cursor_pos_callback(double xpos, double ypos)
{
    std::cout << "cursor_pos_callback(" << xpos << "," << ypos << ")" << std::endl;
}
void UI::mouse_button_callback(int button, int action, int mods)
{
    window.SetTitle("mouse_button_callback({0},{1},{2})", button, action, mods);
}
void UI::size_callback(int width, int height)
{
    window.SetTitle("window_size_callback({0},{1})",width,height);
}
void UI::scroll_callback(double x, double y)
{
    window.SetTitle("scroll_callback({0},{1})", x, y);
}
void UI::refresh_callback()
{}
void UI::iconify_callback(int iconified)
{}
void UI::maximize_callback(int maximized)
{}
void UI::focus_callback(int focused)
{}


int UI::Run()
{
    if (!window.Init())
    {
        return EXIT_FAILURE;
    }

    // main loop
    window.Run();
    window.Cleanup();

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    UI ui;
    return ui.Run();
}
