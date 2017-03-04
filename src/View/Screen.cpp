#include "Screen.h"

#include "Input.h"
#include "Debug.h"
#include "Scene.h"
#include "Prefab.h"
#include "Camera.h"
#include "Material.h"
#include "Transform.h"
#include "GameObject.h"
#include "Application.h"
#include "SceneManager.h"
#include "AssetsManager.h"
#include "GraphicPipeline.h"
#include "SingletonManager.h"

#ifdef BANG_EDITOR
#include "Explorer.h"
#include "Hierarchy.h"
#include "EditorScene.h"
#include "EditorWindow.h"
#include "ShortcutManager.h"
#include "SelectionFramebuffer.h"
#else
#include "GameWindow.h"
#endif

Screen::Screen(QWidget* parent) : QGLWidget(parent)
{
    setFormat(QGLFormat(QGL::DoubleBuffer));
    setAutoBufferSwap(false);
}

Screen::~Screen()
{
    if (m_gPipeline) { delete m_gPipeline; }
}

void Screen::initializeGL()
{
    glewExperimental = GL_TRUE;
    glewInit();

    GL::Enable(GL_DEPTH_TEST);
    GL::Enable(GL_CULL_FACE);

    static bool firstInitializeGL = true;
    if (firstInitializeGL)
    {
        m_gPipeline = new GraphicPipeline(this);
        firstInitializeGL = false;
    }
}

void Screen::paintGL()
{
    Render();
}

void Screen::Render()
{
    Scene *activeScene = SceneManager::GetActiveScene();
    if (activeScene)
    {
        m_gPipeline->RenderScene(activeScene, IsRenderingInGame());
    }
    else
    {
        GL::ClearDepthBuffer();
        GL::ClearColorBuffer(Color::One);
    }
}

void Screen::resizeGL(int w, int h)
{
    GL::SetViewport(0, 0, w, h);
    m_width = w;
    m_height = h;
    m_aspectRatio = float(w) / h;

    m_gPipeline->OnResize(w, h);

    Scene *activeScene = SceneManager::GetActiveScene();
    if (activeScene)
    {
        activeScene->_OnResize(w,h);
    }
}

bool Screen::IsRenderingInGame() const
{
    #ifdef BANG_EDITOR
    EditorWindow *win = EditorWindow::GetInstance();
    ASSERT(win, "", return false);

    return win->tabContainerSceneGame->currentWidget() ==
           win->tabGame;
    #else
    return true;
    #endif
}

Screen *Screen::GetInstance()
{
    IWindow *w = IWindow::GetInstance();
    #ifdef BANG_EDITOR
    return w ? Object::SCast<EditorWindow>(w)->screen : nullptr;
    #else
    return w ? Object::SCast<GameWindow>(w)->screen : nullptr;
    #endif
}

float Screen::GetAspectRatio()
{
    return Screen::GetInstance()->m_aspectRatio;
}

int Screen::GetWidth()
{
    return Screen::GetInstance()->m_width;
}

Vector2 Screen::GetSize()
{
    return Vector2(Screen::GetWidth(), Screen::GetHeight());
}

Vector2 Screen::GetPixelClipSize()
{
    return 1.0f / Screen::GetSize();
}

bool Screen::MouseOver()
{
    return Screen::GetInstance()->underMouse();
}

int Screen::GetHeight()
{
    return Screen::GetInstance()->m_height;
}

GraphicPipeline *Screen::GetGraphicPipeline() const
{
    return m_gPipeline;
}

#ifdef BANG_EDITOR
void Screen::OnShortcutPressed()
{
    if (hasFocus())
    {
        if ( ShortcutManager::IsPressed({Input::Key::Control, Input::Key::C}) )
        { // Copy
            Hierarchy::GetInstance()->m_hContextMenu.OnCopyClicked();
        }
        else if ( ShortcutManager::IsPressed({Input::Key::Control, Input::Key::V}) )
        { // Paste
            Hierarchy::GetInstance()->m_hContextMenu.OnPasteClicked();
        }
        else if ( ShortcutManager::IsPressed({Input::Key::Control, Input::Key::D}) )
        { // Duplicate
            Hierarchy::GetInstance()->m_hContextMenu.OnDuplicateClicked();
        }
        else if (ShortcutManager::IsPressed(Input::Key::Delete))
        { // Delete
            Hierarchy::GetInstance()->m_hContextMenu.OnDeleteClicked();
        }
    }
}

void Screen::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept();
}

void Screen::HandleGameObjectDragging(QDragMoveEvent *e, QWidget *origin)
{
    Scene *activeScene = SceneManager::GetActiveScene();
    EditorScene *scene = Object::SCast<EditorScene>(activeScene);
    SelectionFramebuffer *sfb = GraphicPipeline::GetActive()->
                                                    GetSelectionFramebuffer();
    int x = e->pos().x();
    int y = Screen::GetHeight() - e->pos().y();
    GameObject *overedGo = sfb->GetGameObjectInPosition(x, y);
    m_dragOrigin = origin;

    if (!m_gameObjectBeingDragged)
    {
        Explorer *explorer = Explorer::GetInstance();
        Hierarchy *hierarchy = Hierarchy::GetInstance();
        if (origin == hierarchy)
        {
            GameObject *selectedGo = hierarchy->GetFirstSelectedGameObject();
            if (selectedGo)
            {
                m_gameObjectBeingDragged =
                        Object::SCast<GameObject>(selectedGo->Clone());
            }
        }
        else if (origin == explorer)
        {
            File f = explorer->GetSelectedFile();
            if (f.IsPrefabAsset())
            {
                Prefab *prefab = new Prefab();
                prefab->ReadXMLInfoFromString(f.GetContents());
                m_gameObjectBeingDragged = prefab->InstantiateWithoutStarting();
                delete prefab;
            }
        }
    }
    else
    {
        Vector3 worldPos;
        if (overedGo)
        {
            worldPos = sfb->GetWorldPositionAt(x, y);
        }
        else
        {
            Camera *cam = scene->GetCamera();
            Vector2 ndcPos = Vector2(x, Screen::GetHeight() - y);
            ndcPos /= Screen::GetSize();
            ndcPos = ndcPos * 2.0f - 1.0f;
            ndcPos.y *= -1.0f;
            float z = Vector3::Distance(cam->transform->GetPosition(),
                                        scene->GetAABBox().GetCenter());
            worldPos = cam->ScreenNDCPointToWorld(ndcPos, z);
        }

        m_gameObjectBeingDragged->transform->SetPosition(worldPos);
    }

    if (m_gameObjectBeingDragged)
    {
        m_gameObjectBeingDragged->m_isDragged = true;
        if (m_gameObjectBeingDragged->parent != scene)
        {
            m_gameObjectBeingDragged->SetParent(scene);
        }
    }
}

void Screen::dragMoveEvent(QDragMoveEvent *e)
{
    e->accept();

    Scene *activeScene = SceneManager::GetActiveScene();
    ASSERT(activeScene);

    SelectionFramebuffer *sfb =
            GraphicPipeline::GetActive()->GetSelectionFramebuffer();
    int x = e->pos().x(), y = Screen::GetHeight() - e->pos().y();
    GameObject *overedGo = sfb->GetGameObjectInPosition(x, y);

    Explorer *explorer = Explorer::GetInstance();
    Hierarchy *hierarchy = Hierarchy::GetInstance();
    if (e->source() == explorer)
    {
        File f = explorer->GetSelectedFile();
        if (f.IsMaterialAsset())
        {
            Material *mat = AssetsManager::Load<Material>(f.GetRelativePath());
            if (mat)
            {
                if (m_lastGameObjectOvered != overedGo)
                {
                    if (overedGo)
                    {
                        overedGo->OnDragEnterMaterial(mat);
                    }

                    if (m_lastGameObjectOvered)
                    {
                        m_lastGameObjectOvered->OnDragLeaveMaterial(mat);
                    }
                }
            }
        }
        else if (f.IsPrefabAsset())
        {
            HandleGameObjectDragging(e, explorer);
        }
    }
    else if (e->source() == hierarchy)
    {
        HandleGameObjectDragging(e, hierarchy);
    }

    m_lastGameObjectOvered = overedGo;
}

void Screen::dragLeaveEvent(QDragLeaveEvent *e)
{
    if (m_gameObjectBeingDragged)
    {
        m_lastGameObjectOvered = nullptr;
        m_gameObjectBeingDragged->SetParent(nullptr);
    }

    e->accept();
}

void Screen::dropEvent(QDropEvent *e)
{
    // ASSERT(m_activeScene);
    e->ignore();
}

void Screen::OnDrop(const DragDropInfo &ddi)
{
    Scene *activeScene = SceneManager::GetActiveScene();

    if ( MouseOverMe() )
    {
        if (m_gameObjectBeingDragged)
        {
            m_gameObjectBeingDragged->m_isDragged = false;
            m_gameObjectBeingDragged->SetParent(activeScene);
            Hierarchy::GetInstance()->SelectGameObject(m_gameObjectBeingDragged);
        }
    }
    else
    {
        if (m_gameObjectBeingDragged)
        {
            delete m_gameObjectBeingDragged;
        }
    }

    m_lastGameObjectOvered = nullptr;
    m_gameObjectBeingDragged = nullptr;
}

#endif
