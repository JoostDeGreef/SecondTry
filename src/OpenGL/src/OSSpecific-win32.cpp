
#include "OpenGL.h"

#define GLFW_EXPOSE_NATIVE_WIN32 1

#if defined(APIENTRY)
  #undef APIENTRY
#endif

#include "GLFW/glfw3native.h"

void CloseOwningWindow(GLFWwindow* window)
{
    HWND hwnd = glfwGetWin32Window(window);
    CloseWindow(hwnd);
}

void RemoveWindowDecorations(GLFWwindow* window)
{
    glfwSetWindowOpacity(window, 0.75f);
    HWND hwnd = glfwGetWin32Window(window);
    SetWindowLongPtr(hwnd, GWL_STYLE, GetWindowLongPtrA(hwnd, GWL_STYLE) & ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX | WS_SYSMENU));
}
