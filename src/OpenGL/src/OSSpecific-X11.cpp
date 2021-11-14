
#include "OpenGL.h"

#if defined(APIENTRY)
  #undef APIENTRY
#endif

#include "GLFW/glfw3native.h"

namespace OSSpecific
{
    void CloseOwningWindow(GLFWwindow* window)
    {
        // HWND hwnd = glfwGetWin32Window(window);
        // CloseWindow(hwnd);
    }

    void RemoveWindowDecorations(GLFWwindow* window)
    {
        // glfwSetWindowOpacity(window, 0.75f);
        // HWND hwnd = glfwGetWin32Window(window);
        // SetWindowLongPtr(hwnd, GWL_STYLE, GetWindowLongPtr(hwnd, GWL_STYLE) & ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX | WS_SYSMENU));
    }

    void EnableCloseButton(GLFWwindow* window, const bool enabled)
    {
        // HWND hwnd = glfwGetWin32Window(window);
        // SetWindowLongPtr(hwnd, GWL_STYLE, (GetWindowLongPtr(hwnd, GWL_STYLE) & ~WS_SYSMENU) | (enabled ? WS_SYSMENU : 0));
    }

    void MakeToolWindow(GLFWwindow* window, const bool enabled)
    {
        // HWND hwnd = glfwGetWin32Window(window);
        // SetWindowLongPtr(hwnd, GWL_EXSTYLE, (GetWindowLongPtr(hwnd, GWL_EXSTYLE) & ~WS_EX_TOOLWINDOW) | (enabled ? WS_EX_TOOLWINDOW : 0));
        // glfwSetWindowOpacity(window, 0.75f);
    }

    void StayOnTop(GLFWwindow* window, const bool enabled)
    {
        // HWND hwnd = glfwGetWin32Window(window);
        // SetWindowPos(hwnd, (enabled ? HWND_TOPMOST : HWND_NOTOPMOST), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        // glfwSetWindowOpacity(window, 0.75f);
    }
}
