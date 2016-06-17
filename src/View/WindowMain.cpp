#include "WindowMain.h"

#include "SingletonManager.h"
#include "WindowEventManager.h"

#include "Toolbar.h"

WindowMain *WindowMain::win = nullptr;

void WindowMain::InitFromMainBinary(QMainWindow *window, QApplication *application)
{
    win = new WindowMain();
    SingletonManager::GetInstance()->SetWindowMainSingleton(win);

    win->setupUi(window);

    win->mainWindow = window;
    win->mainWindow->showMaximized();

    win->app = application;

    Toolbar::Init();
}

WindowMain *WindowMain::GetInstance()
{
    // This will always be initialized,
    // since main inits it using WindowMain::InitFromMainBinary()
    return SingletonManager::GetInstance()->GetWindowMainSingleton();
}

QMainWindow *WindowMain::GetMainWindow()
{
    return GetInstance()->mainWindow;
}

QApplication *WindowMain::GetApplication() const
{
    return GetInstance()->app;
}
