#include "Scene.h"
#include "GameObject.h"
#include "Canvas.h"

#include "DirectionalLight.h"

Scene::Scene() : GameObject("Scene")
{
    GameObject *test = new GameObject("LightTest");
    test->AddComponent<DirectionalLight>();
    AddChild(test);

    p_gbuffer = new GBuffer(Canvas::GetWidth(), Canvas::GetHeight());
}

void Scene::_OnResize(int newWidth, int newHeight)
{
    p_gbuffer->Resize(newWidth, newHeight);
}

Scene::~Scene()
{
    this->_OnDestroy();
    delete m_defaultCamera;
    delete p_gbuffer;
}

void Scene::_OnRender()
{
    Camera *cam = p_cameraGameObject->GetComponent<Camera>();
    if(cam  && cam->GetAutoUpdateAspectRatio())
    {
        cam->SetAspectRatio( p_canvas->GetAspectRatio() );
    }

    p_gbuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //From 0 to 9
    for(m_currentRenderLayer = 0; m_currentRenderLayer <= 9; ++m_currentRenderLayer)
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        PROPAGATE_EVENT(_OnPreRender, m_children);
        PROPAGATE_EVENT(_OnRender, m_children);
    }

    // Apply lights to gbuffer
    std::list<DirectionalLight*> childrenLights =
            GetComponentsInChildren<DirectionalLight>();
    for (DirectionalLight *light : childrenLights)
    {
        light->ApplyLight(p_gbuffer);
    }

    p_gbuffer->UnBind();
    p_gbuffer->RenderToScreen();
}

void Scene::SetCamera(const Camera *cam)
{
    if(!cam)
    {
        this->p_cameraGameObject = nullptr;
        SetCamera(m_defaultCamera->GetComponent<Camera>());
    }
    else
    {
        this->p_cameraGameObject = cam->gameObject;
    }
}

Camera *Scene::GetCamera() const
{
    if(!p_cameraGameObject)
    {
        return nullptr;
    }

    return p_cameraGameObject->GetComponent<Camera>();
}

const Canvas *Scene::GetCanvas() const
{
    return p_canvas;
}

bool Scene::IsScene() const
{
    return true;
}

void Scene::Write(std::ostream &f) const
{
    f << "<Scene>" << std::endl;
    f << ((void*)this) << std::endl;   //internal file id
    f << this->GetName() << std::endl; //scene name

    f << "<children>" << std::endl;
    for(GameObject *e : m_children)
    {
        e->Write(f);
    }
    f << "</children>" << std::endl;

    //Not used ftm
    //f << "<cameraGameObject>" << std::endl;
    //f << "</cameraGameObject>" << std::endl;

    f << "</Scene>" << std::endl;
}
