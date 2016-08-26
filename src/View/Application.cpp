#include "Application.h"

#ifdef BANG_EDITOR
#include "EditorWindow.h"
#include "DragDropManager.h"
#include "ShortcutManager.h"
#include "WindowEventManager.h"
#else
#include "GameWindow.h"
#endif

#include "Scene.h"
#include "Screen.h"
#include "SceneManager.h"

Application::Application(int &argc, char **argv) : QApplication(argc, argv)
{
    m_sceneManager = new SceneManager();

    connect(&m_drawTimer, SIGNAL(timeout()), this, SLOT(OnDrawTimerTick()));

    m_drawTimer.setInterval(c_redrawDelay);
    m_drawTimer.start();

    m_lastRenderTime = Time::GetNow();
}

void Application::OnDrawTimerTick()
{
    // Update deltaTime
    float deltaTime = float(Time::GetNow() - m_lastRenderTime) / 1000.0f;
    Time::GetInstance()->m_deltaTime = deltaTime;

    // Update mainBinary static deltaTime, for internal engine use
    // (this is not Behaviours' static deltaTime, theirs is
    //  updated in Behaviour _OnUpdate)
    Time::s_deltaTime = deltaTime;
    //

    // Process mouse and key events, so the Input is available in OnUpdate
    // as accurate as possible.
    // Lost events in between Update and Render will be delayed by Input.
    processEvents();
    Input::GetInstance()->ProcessEnqueuedEvents();

    Scene *activeScene = SceneManager::GetActiveScene();
    if (activeScene)
    {
        m_lastRenderTime = Time::GetNow();
        activeScene->_OnUpdate();
    }

    // Render screen  (_OnRender mainly)
    Screen::GetInstance()->Render();
    Screen::GetInstance()->swapBuffers();

    Input::GetInstance()->OnFrameFinished(); // Notify to Input that a new frame has passed
}


Application *Application::GetInstance()
{
    #ifdef BANG_EDITOR
    return static_cast<Application*>(EditorWindow::GetInstance()->GetApplication());
    #else
    return static_cast<Application*>(GameWindow::GetInstance()->GetApplication());
    #endif
}

bool Application::notify(QObject *receiver, QEvent *e)
{
    #ifdef BANG_EDITOR
    if (e->type() == QEvent::MouseButtonPress)
    {
        DragDropManager::HandleGlobalMousePress(receiver, e);
    }
    else if (e->type() == QEvent::MouseButtonRelease)
    {
        DragDropManager::HandleGlobalMouseRelease(receiver, e);
    }

    if (e->type() == QEvent::KeyPress)
    {
        QKeyEvent *ev = static_cast<QKeyEvent*>(e);
        if (!ev->isAutoRepeat())
        {
            ShortcutManager::GetInstance()->OnKeyPressed( Input::Key(ev->key()) );
        }
    }
    else if (e->type() == QEvent::KeyRelease)
    {
        QKeyEvent *ev = static_cast<QKeyEvent*>(e);
        if (!ev->isAutoRepeat())
        {
            ShortcutManager::GetInstance()->OnKeyReleased( Input::Key(ev->key()) );
        }
    }

    if (e->type() == QEvent::Shortcut)
    {
        ShortcutManager::GetInstance()->Clear();
    }
    #endif

    return QApplication::notify(receiver, e);
}