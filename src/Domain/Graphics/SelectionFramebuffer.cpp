#include "SelectionFramebuffer.h"

#include "WindowMain.h"
#include "Hierarchy.h"

SelectionFramebuffer::SelectionFramebuffer(int width, int height) :
    Framebuffer(width, height)
{
    program = new ShaderProgram(
                ShaderContract::Filepath_Shader_Vertex_PVM_Position,
                ShaderContract::Filepath_Shader_Fragment_Selection);

    CreateTextureAttachment(0, AttachmentType::Color);
}

SelectionFramebuffer::~SelectionFramebuffer()
{
    delete program;
}

void SelectionFramebuffer::RenderSelectionBuffer(const Scene *scene)
{
    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Logger_Log("______________");
    program->Bind();
    std::list<Renderer*> childrenRenderers =
            scene->GetComponentsInChildren<Renderer>();
    for(auto it = childrenRenderers.begin(); it != childrenRenderers.end(); ++it)
    {
        Renderer *renderer = *it;
        Matrix4 model, view, projection, pvm;
        renderer->GetMatrices(model, view, projection, pvm);

        program->SetUniformMat4(ShaderContract::Uniform_Matrix_Model,
                                model, false);
        program->SetUniformMat4(ShaderContract::Uniform_Matrix_View,
                                view, false);
        program->SetUniformMat4(ShaderContract::Uniform_Matrix_Projection,
                                projection, false);
        program->SetUniformMat4(ShaderContract::Uniform_Matrix_PVM,
                                pvm, false);

        GameObject *go = renderer->GetOwner();
        renderer->RenderWithoutBindingMaterial();
        //Logger_Log(gameObjectsToId[go]);
        Vector3 selectionColor = MapIdToColor(gameObjectsToId[go]);
        program->SetUniformVec3("selectionColor", selectionColor);

        //Logger_Log(renderer->GetOwner()->GetName());
    }
    program->UnBind();
}

void SelectionFramebuffer::ProcessSelection()
{
    if(Input::GetMouseButtonDown(Input::MouseButton::MLeft))
    {
        glm::vec2 coords = Input::GetMouseCoords();
        Vector3 selectedColor = ReadPixel(coords.x, Canvas::GetHeight()-coords.y, 0, false);
        Logger_Log(selectedColor);

        if(selectedColor.r < 254 || selectedColor.g < 254 || selectedColor.b < 254)
        {
            int id = MapColorToId(selectedColor);
            if(idToGameObject.find(id) != idToGameObject.end())
            {
                WindowMain::GetInstance()->widgetHierarchy->SelectGameObject(idToGameObject[id]);
            }
        }
    }
}

void SelectionFramebuffer::OnChildAdded(GameObject *child)
{
    gameObjectsToId[child] = idCount;
    idToGameObject[idCount] = child;
    ++idCount;

    //Logger_Log("Received " << child);
}

Vector3 SelectionFramebuffer::MapIdToColor(long id)
{
    Vector3 charColor =
            Vector3(
                    double(id % 256),
                    double((id / 256) % 256),
                    double(((id / 256) / 256) % 256)
                   );

   return charColor / 256.0d;
}

long SelectionFramebuffer::MapColorToId(const Vector3 &charColor)
{
    Vector3 color = charColor / 256.0d;
    return long(color.r * 256) +
           long(color.g * 256 * 256) +
           long(color.b * 256 * 256 * 256);
}