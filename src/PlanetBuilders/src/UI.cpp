#include <cstdlib>
#include <thread>

#include "Log.h"
#include "UI.h"

using namespace std;
using namespace OpenGL;
using namespace Core;

UI::UI()
    : CallbackHandler()
    , m_mainWindow(*this, "PlanetBuilder", Window::Option::HasCloseButton)
{}

int UI::Run()
{
    if (!m_mainWindow.Initialize())
    {
        return EXIT_FAILURE;
    }
    else
    {
        OpenGL::Window::EnterMessageLoop();
        return EXIT_SUCCESS;
    }
}

void UI::ContextInit(const std::shared_ptr<OpenGL::Window>& window)
{
}

void UI::ContextFree(const std::shared_ptr<OpenGL::Window>& window)
{
}

void UI::Draw2D(const std::shared_ptr<OpenGL::Window>& window)
{
}

void UI::Draw3D(const std::shared_ptr<OpenGL::Window>& window)
{
}


