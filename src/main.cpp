#include <vector>
#include <string>
#include <iostream>
#include <QApplication>

#include "glm/glm.hpp"

#include "Bang.h"
#include "ShaderProgram.h"
#include "MeshRenderer.h"
#include "MeshPyramid.h"
#include "GameObject.h"
#include "Camera.h"
#include "Canvas.h"
#include "Shader.h"
#include "Timer.h"
#include "Framebuffer.h"
#include "Texture2D.h"
#include "TestBehaviour.h"
#include "FileReader.h"
#include "TestCameraBehaviour.h"

#include "EditorScene.h"
#include "WindowMain.h"
#include "WindowMainAutoGenerated.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFont font;
    font.setFamily(font.defaultFamily());
    font.setPixelSize(12);
    app.setFont(font);

    QMainWindow *window = new QMainWindow;
    WindowMain::Init(window, &app); //init window main
    WindowMain *windowMain = WindowMain::GetInstance();
    windowMain->menubar->CreateNewScene();

    Scene *scene = new EditorScene();
    std::string filename = "./res/Assets/sceneTest25.bscene";
    FileReader::ReadScene(filename , scene);
    if(scene != nullptr)
    {
        Canvas::GetInstance()->AddScene(scene);
        Canvas::GetInstance()->SetCurrentScene(scene);
        Persistence::SetCurrentSceneFilepath(filename);
    }

    /*
    Scene *scene = nullptr;
    #ifndef BANG_EDITOR
    scene = new Scene();
    #else
    scene = new EditorScene();
    #endif
    FileReader::ReadScene("res/Assets/sceneTest", scene);
    windowMain->canvas->AddScene(scene);
    windowMain->canvas->SetCurrentScene("MainScene");
    **/
    ///////

    return app.exec();
}
