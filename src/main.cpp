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
#include "MeshPyramid.h"
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
    FileReader::ReadScene(filename , scene);
    if(scene)
    {
        Canvas::GetInstance()->AddScene(scene);
        Canvas::GetInstance()->SetCurrentScene(scene);
        Persistence::SetCurrentSceneFilepath(filename);
    }

    #else

    Scene *scene = new Scene();
    std::string filename = "./Assets/Scenes/test2.bscene";
    FileReader::ReadScene(filename , scene);
    if(scene)
    {
        Canvas::GetInstance()->AddScene(scene);
        Canvas::GetInstance()->SetCurrentScene(scene);
        Persistence::SetCurrentSceneFilepath(filename);
    }
    #endif

    return app.exec();
}
