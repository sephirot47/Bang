#include "Canvas.h"

#include "SelectionFramebuffer.h"
#include "WindowMain.h"

Canvas *Canvas::mainBinaryCanvas = nullptr;

Canvas::Canvas(QWidget* parent) : QGLWidget(parent)
{
    setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
    clearColor = glm::vec4(1.0f) * 0.8f;

    connect(&drawTimer, SIGNAL(timeout()), this, SLOT(update()));
    drawTimer.setInterval(Canvas::RedrawDelay);
    drawTimer.start();
}

void Canvas::InitFromMainBinary()
{
    Canvas::mainBinaryCanvas = WindowMain::GetInstance()->canvas;
}

void Canvas::initializeGL()
{
    WindowMain::GetInstance()->buttonPauseResume
            ->setText( (paused ? QString("Resume") : QString("Pause")) );

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    lastRenderTime = Time::GetNow();
}

void Canvas::paintGL()
{
    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Time::GetInstance()->deltaTime = float(Time::GetNow() -
                                           lastRenderTime) / 1000.0f;

    if(currentScene)
    {
        lastRenderTime = Time::GetNow();
        currentScene->_OnUpdate();

        //Note: _OnPreRender() is called from scene _OnRender
        currentScene->_OnRender();
    }

    Input::GetInstance()->OnNewFrame();
}

void Canvas::updateGL()
{
}

void Canvas::resizeGL(int w, int h)
{
    glViewport(0, 0, (GLint)w, (GLint)h);
    width = w;
    height = h;
    aspectRatio = float(w) / h;

    if(currentScene )
    {
        currentScene->_OnResize(w,h);
    }
}

Scene *Canvas::AddScene(const std::string &name)
{
    Scene *st = new Scene();
    st->m_name = name;
    scenes.push_back(st);
    return st;
}

void Canvas::AddScene(Scene *scene)
{
    scenes.push_back(scene);
}

void Canvas::SetCurrentScene(Scene *scene)
{
    if(currentScene )
    {
        currentScene->_OnDestroy();
    }

    currentScene = scene;
    if(currentScene )
    {
        currentScene->_OnStart();
        WindowMain::GetInstance()->widgetHierarchy->Refresh();
    }
}

void Canvas::SetCurrentScene(const std::string &name)
{
    if(currentScene )
    {
        currentScene->_OnDestroy();
    }

    for(auto it = scenes.begin(); it != scenes.end(); ++it)
    {
        if((*it)->name == name)
        {
            SetCurrentScene((*it));
            return;
        }
    }

    Logger_Warn("Could not change Scene to '" << name << "', "<<
                   "because no scene with this name is added to the Canvas.");
}

Scene *Canvas::GetCurrentScene()
{
    return Canvas::GetInstance()->currentScene;
}

Scene *Canvas::GetScene(const std::string &name) const
{
    for(auto it = scenes.begin(); it != scenes.end(); ++it)
    {
        if((*it)->name == name) return (*it);
    }
    return nullptr;
}

void Canvas::RemoveScene(const std::string &name)
{
    for(auto it = scenes.begin(); it != scenes.end(); ++it)
    {
        if((*it)->name == name) { scenes.erase(it); return; }
    }
}

Canvas *Canvas::GetInstance()
{
    return Canvas::mainBinaryCanvas;
}

float Canvas::GetAspectRatio()
{
    return Canvas::mainBinaryCanvas->aspectRatio;
}

int Canvas::GetWidth()
{
    return Canvas::mainBinaryCanvas->width;
}

bool Canvas::IsPaused() const
{
    return paused;
}

int Canvas::GetHeight()
{
    return Canvas::mainBinaryCanvas->height;
}

void Canvas::SetCursor(Qt::CursorShape cs)
{
    WindowMain::GetInstance()->GetApplication()->
                setOverrideCursor( cs );
}

void Canvas::wheelEvent(QWheelEvent *event)
{
    Input::GetInstance()->HandleInputMouseWheel(event);
    QGLWidget::wheelEvent(event);
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    Input::GetInstance()->HandleInputMouseMove(event);
    QGLWidget::mouseMoveEvent(event);
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    Input::GetInstance()->HandleInputMousePress(event);
    QGLWidget::mousePressEvent(event);
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    Input::GetInstance()->HandleInputMouseRelease(event);
    QGLWidget::mouseReleaseEvent(event);
}

void Canvas::keyPressEvent(QKeyEvent *event)
{
    Input::GetInstance()->HandleInputKeyPress(event);
    QGLWidget::keyPressEvent(event);
}

void Canvas::keyReleaseEvent(QKeyEvent *event)
{
    Input::GetInstance()->HandleInputKeyReleased(event);
    QGLWidget::keyReleaseEvent(event);
}

void Canvas::OnTopKekPressed()
{
    Prefab *p = AssetsManager::GetAsset<Prefab>("Assets/Prefabs/luigiPrefab.bprefab");

    GameObject *e = p->Instantiate();

    GameObject *selected = WindowMain::GetInstance()->widgetHierarchy->
            GetFirstSelectedGameObject();

    if(selected ) selected->AddChild(e);
    else currentScene->AddChild(e);
}

void Canvas::OnPauseResumeButtonPressed()
{
    paused = !paused;
    WindowMain::GetInstance()->buttonPauseResume->setText( (paused ? QString("Resume") : QString("Pause")) );
}
