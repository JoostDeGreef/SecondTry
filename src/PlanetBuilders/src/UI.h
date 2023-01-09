#pragma once

#include "OpenGL.h"

class UI : public OpenGL::CallbackHandler
{
public:
    UI();

    void char_callback(const std::shared_ptr<OpenGL::Window>& window, unsigned int c) override;
    void close_callback(const std::shared_ptr<OpenGL::Window>& window) override;
    void content_scale_callback(const std::shared_ptr<OpenGL::Window>& window, float xscale, float yscale) override;
    void cursor_pos_callback(const std::shared_ptr<OpenGL::Window>& window, double xpos, double ypos) override;
    void error_callback(const std::shared_ptr<OpenGL::Window>& window, int error, const char* description) override;
    void focus_callback(const std::shared_ptr<OpenGL::Window>& window, int focused) override;
    void framebuffer_size_callback(const std::shared_ptr<OpenGL::Window>& window, int width, int height) override;
    void iconify_callback(const std::shared_ptr<OpenGL::Window>& window, int iconified) override;
    void key_callback(const std::shared_ptr<OpenGL::Window>& window, int key, int scancode, int action, int mods) override;
    void maximize_callback(const std::shared_ptr<OpenGL::Window>& window, int maximized) override;
    void mouse_button_callback(const std::shared_ptr<OpenGL::Window>& window, int button, int action, int mods) override;
    void pos_callback(const std::shared_ptr<OpenGL::Window>& window, int xpos, int ypos) override;
    void size_callback(const std::shared_ptr<OpenGL::Window>& window, int width, int height) override; 
    void scroll_callback(const std::shared_ptr<OpenGL::Window>& window, double x, double y) override;
    void refresh_callback(const std::shared_ptr<OpenGL::Window>& window) override;

    void ContextInit(const std::shared_ptr<OpenGL::Window>& window) override;
    void Draw3D(const std::shared_ptr<OpenGL::Window>& window) override;
    void Draw2D(const std::shared_ptr<OpenGL::Window>& window) override;
    void ContextFree(const std::shared_ptr<OpenGL::Window>& window) override;

    int Run();

private:
    OpenGL::Window m_mainWindow;
};

