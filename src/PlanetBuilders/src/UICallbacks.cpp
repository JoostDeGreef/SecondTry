#include <cstdlib>
#include <thread>

#include "Log.h"
#include "UI.h"

using namespace std;
using namespace OpenGL;
using namespace Core;

void UI::char_callback(
    const std::shared_ptr<Window>& window, 
    unsigned int c) 
{ 
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

void UI::close_callback(
    const std::shared_ptr<OpenGL::Window>& window) 
{}

void UI::content_scale_callback(
    const std::shared_ptr<OpenGL::Window>& window, 
    float xscale, 
    float yscale)
{}

void UI::cursor_pos_callback(
    const std::shared_ptr<OpenGL::Window>& window, 
    double xpos, 
    double ypos)
{}

void UI::error_callback(
    const std::shared_ptr<OpenGL::Window>& window, 
    int error, 
    const char* description)
{ 
    LogError("error_callback(...,{0},{1})", error, description); 
};

void UI::focus_callback(
    const std::shared_ptr<OpenGL::Window>& window, 
    int focused)
{}

void UI::framebuffer_size_callback(
    const std::shared_ptr<OpenGL::Window>& window, 
    int width, 
    int height)
{}

void UI::iconify_callback(
    const std::shared_ptr<OpenGL::Window>& window, 
    int iconified)
{
    // TODO: stop/reduce graphical updates, only do game calculations
}

void UI::key_callback(
    const std::shared_ptr<OpenGL::Window>& window, 
    int key, 
    int scancode, 
    int action, 
    int mods)
{}

void UI::maximize_callback(
    const std::shared_ptr<OpenGL::Window>& window, 
    int maximized)
{}    
    
void UI::mouse_button_callback(
    const std::shared_ptr<OpenGL::Window>& window, 
    int button, 
    int action, 
    int mods)
{}

void UI::pos_callback(
    const std::shared_ptr<OpenGL::Window>& window, 
    int xpos, 
    int ypos)
{}

void UI::size_callback(
    const std::shared_ptr<OpenGL::Window>& window, 
    int width, 
    int height)
{ 
    m_state2d.Size().Set(width, height);
    m_state2d.Projection().SetOrtho(0, width, 0, height, -1, 1);
};

void UI::scroll_callback(
    const std::shared_ptr<OpenGL::Window>& window, 
    double x, 
    double y)
{}

void UI::refresh_callback(
    const std::shared_ptr<OpenGL::Window>& window)
{}

