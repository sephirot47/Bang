#include <vector>
#include <string>
#include <iostream>

#include <QApplication>
#include <QTextStream>

#include "glm/glm.hpp"

#include "Bang.h"

#include "SingletonManager.h"
#include "ShaderProgram.h"
#include "MeshRenderer.h"
#include "Framebuffer.h"
#include "GameObject.h"
#include "FileReader.h"
#include "Behaviour.h"
#include "Texture2D.h"
#include "Camera.h"
#include "Canvas.h"
#include "Shader.h"
#include "Timer.h"

#ifdef BANG_EDITOR

#include "EditorScene.h"
#include "WindowMain.h"
#include "WindowMainAutoGenerated.h"

#else

#include "GameWindow.h"

#endif


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SingletonManager::InitSingletonMangerFromMainBinary();

    QMainWindow *window = new QMainWindow();

    #ifdef BANG_EDITOR

    QFont font;
    font.setFamily(font.defaultFamily());
    font.setPixelSize(11);
    app.setFont(font);

    WindowMain::InitFromMainBinary(window, &app);

    #else

    GameWindow::InitFromMainBinary(window, &app);

    #endif



    Canvas::InitFromMainBinary();
    Time::InitFromMainBinary();
    Input::InitFromMainBinary();



    #ifdef BANG_EDITOR

    // Load dark Window theme
    QFile f("Assets/Engine/qdarkstyle/style.qss");
    if (!f.exists())
    {
        Logger_Error("Unable to set dark stylesheet, stylesheet not found.");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        app.setStyleSheet(ts.readAll());
    }
    //

    Scene *scene = new EditorScene();
    std::string filename = "./Assets/Scenes/test2.bscene";
    FileReader::ReadScene(filename, scene);
    if (scene)
    {
        Canvas::GetInstance()->AddScene(scene);
        Canvas::GetInstance()->SetCurrentScene(scene);
        Persistence::SetCurrentSceneFilepath(filename);
    }

    /* TEST STUFF
    GameObject *go1 = new GameObject();
    GameObject *go2 = new GameObject();
    go2->SetParent(go1);
    go1->transform->SetPosition(Vector3(5,5,5));
    go2->transform->SetPosition(Vector3(0,0,0));

    Logger_Log("GO1: ");
    Logger_Log(go1->transform);
    Logger_Log(go1->transform->GetPosition());
    Logger_Log("");

    Logger_Log("GO2: ");
    Logger_Log(go2->transform);
    Logger_Log(go2->transform->GetPosition());
    Logger_Log("");
    */

    #else

    Scene *scene = new Scene();
    std::string filename = "./Assets/Scenes/test2.bscene";
    FileReader::ReadScene(filename, scene);
    if (scene)
    {
        Canvas::GetInstance()->AddScene(scene);
        Canvas::GetInstance()->SetCurrentScene(scene);
        Persistence::SetCurrentSceneFilepath(filename);

        GameObject *m_defaultCamera = new GameObject("DefaultCamera");
        m_defaultCamera->transform->SetPosition(Vector3(90));
        m_defaultCamera->transform->LookAt(Vector3::zero);
        m_defaultCamera->SetParent(scene);

        Camera *cam = m_defaultCamera->AddComponent<Camera>();
        cam->SetFovDegrees(60.0f);
        cam->SetZNear(0.1f);
        cam->SetZFar(99999.0f);
        scene->SetCamera(cam);
    }
    #endif

    return app.exec();
}
