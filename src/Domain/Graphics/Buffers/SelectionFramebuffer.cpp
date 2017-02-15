#include "SelectionFramebuffer.h"

#include "Scene.h"
#include "Debug.h"
#include "Cursor.h"
#include "Material.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Persistence.h"
#include "ShaderProgram.h"
#include "TextureRender.h"
#include "ShaderContract.h"

#ifdef BANG_EDITOR
#include "Hierarchy.h"
#include "EditorWindow.h"
#include "EditorGameObject.h"
#include "WindowEventManager.h"
#endif

SelectionFramebuffer::SelectionFramebuffer(int width, int height) :
    Framebuffer(width, height)
{
    m_selectionProgram = new ShaderProgram(
                Persistence::ToAbsolute("Shaders/SelectionBuffer.vert", true),
                Persistence::ToAbsolute("Shaders/SelectionBuffer.frag", true));

    m_colorTexture = new TextureRender();
    m_worldPosTexture = new TextureRender();
    SetColorAttachment(Attachment::ColorAttachment, m_colorTexture);
    SetColorAttachment(Attachment::WorldPosition,   m_worldPosTexture);
    CreateDepthRenderbufferAttachment();
}

SelectionFramebuffer::~SelectionFramebuffer()
{
    delete m_selectionProgram;
}

void SelectionFramebuffer::PrepareForRender(const Scene *scene)
{
    // Assign id's
    int id = 0;
    m_gameObject_To_Id.Clear();
    m_id_To_GameObject.Clear();
    List<GameObject*> gameObjects = scene->GetChildrenRecursivelyEditor();
    for (GameObject *go : gameObjects)
    {
        m_gameObject_To_Id[go] = id;
        m_id_To_GameObject[id] = go;
        ++id;
    }
    //
}

void SelectionFramebuffer::PrepareNextGameObject(GameObject *go)
{
    m_nextGameObjectToBeRendered = go;
}

void SelectionFramebuffer::RenderForSelectionBuffer(Renderer *rend)
{
    SetAllDrawBuffers();

    GameObject *go = !rend->IsGizmo() ? rend->gameObject :
                                        m_nextGameObjectToBeRendered;
    if (CanRenderGameObject(go))
    {
        ShaderProgram *selectionSp = m_selectionProgram;
        selectionSp->SetUniformColor("selectionColor", GetSelectionColor(go));

        Material *rendMaterial = rend->GetMaterial();
        ShaderProgram *originalSP =
                rendMaterial ? rendMaterial->GetShaderProgram() : nullptr;
        rendMaterial->SetShaderProgram( selectionSp );
        rend->Render();
        rendMaterial->SetShaderProgram( originalSP );
    }
}

void SelectionFramebuffer::ProcessSelection()
{
    // Get mouse coordinates and read pixel color
    Vector2 coords = Input::GetMouseCoords();
    coords.y = Screen::GetHeight() - coords.y;
    GameObject *mouseOverGO = GetGameObjectInPosition(coords.x, coords.y);
    if (!Screen::MouseOver()) { mouseOverGO = nullptr; }

    if (m_lastMouseOverGO  && m_lastMouseOverGO != mouseOverGO)
    {
        m_lastMouseOverGO->OnMouseExit(false);
    }

    if (mouseOverGO)
    {
        if (m_lastMouseOverGO != mouseOverGO)
        {
            mouseOverGO->OnMouseEnter(false);
        }

        mouseOverGO->OnMouseOver(false);
    }

    m_lastMouseOverGO = mouseOverGO;

    // Selection (clicking over) Here we just handle non-EditorGameObjects
    if (Input::GetMouseButtonDown(Input::MouseButton::MLeft))
    {
        if (mouseOverGO)
        {
            if (!mouseOverGO->IsEditorGameObject()) // Selection of a GameObject
            {
                EditorWindow::GetInstance()->
                        widgetHierarchy->SelectGameObject(mouseOverGO);
                if (Input::GetMouseButtonDoubleClick(Input::MouseButton::MLeft))
                {
                    // Double clicking
                    WindowEventManager::
                            NotifyHierarchyGameObjectDoubleClicked(mouseOverGO);
                }
            }
        }
        else // Background has been pressed
        {
            EditorWindow::GetInstance()->widgetHierarchy->UnselectAll();
        }
    }
}

GameObject *SelectionFramebuffer::GetGameObjectInPosition(int x, int y)
{
    Color mouseOverColor = ReadColor(x, y, Attachment::ColorAttachment);
    int id = MapColorToId(mouseOverColor);
    if (mouseOverColor != Color::Zero &&
        m_id_To_GameObject.ContainsKey(id))
    {
        return m_id_To_GameObject[id];
    }
    return nullptr;
}

Color SelectionFramebuffer::GetSelectionColor(GameObject *go) const
{
    return MapIdToColor(m_gameObject_To_Id[go]);
}

Vector3 SelectionFramebuffer::GetWorldPositionAt(int x, int y)
{
    return ReadColor(x, y, Attachment::WorldPosition).ToVector3();
}

bool SelectionFramebuffer::IsPassing() const
{
    return m_isPassing;
}

void SelectionFramebuffer::OnGameObjectDestroyed(GameObject *destroyed)
{
    if (destroyed == m_lastMouseOverGO)
    {
        Cursor::SetIcon( Cursor::CursorIcon::Arrow );
        m_lastMouseOverGO = nullptr;
    }
}

Color SelectionFramebuffer::MapIdToColor(long id)
{
    const int C = 256;
    Color color =
            Color(
                    double(  id           % C),
                    double( (id / C)      % C),
                    double(((id / C) / C) % C)
                   );
   return color / float(C);
}

long SelectionFramebuffer::MapColorToId(const Color &color)
{
    const int C = 256;
    return long(color.r * C) +
           long(color.g * C * C) +
           long(color.b * C * C * C);
}

bool SelectionFramebuffer::CanRenderGameObject(const GameObject *go)
{
    if (go->IsEditorGameObject())
    {
       const EditorGameObject *ego = static_cast<const EditorGameObject*>(go);
       return ego && ego->IsEnabled() && !ego->IsDraggedGameObject() &&
              ego->IsRenderInSelectionFramebuffer();
    }
    return go && go->IsEnabled() && !go->IsDraggedGameObject();
}


TextureRender *SelectionFramebuffer::GetColorTexture() const
{
    return m_colorTexture;
}

TextureRender *SelectionFramebuffer::GetWorldPosTexture() const
{
    return m_worldPosTexture;
}
