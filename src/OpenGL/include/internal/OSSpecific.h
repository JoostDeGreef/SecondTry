#pragma once

namespace OSSpecific
{
    /*
    ** Remove all title bar decorations
    */
    void RemoveWindowDecorations(GLFWwindow* window);

    /*
    ** Enable/disable 'x' button. On windows this also enables the system menu
    */
    void EnableCloseButton(GLFWwindow* window, const bool enabled);

    /*
    ** Make the window a tool window, keep it off the taskbar
    */
    void MakeToolWindow(GLFWwindow* window, const bool enabled);

    /*
    ** Make the window stay on top
    */
    void StayOnTop(GLFWwindow* window, const bool enabled);

}
