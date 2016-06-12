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
#include "FileReader.h"

#include "Behaviour.h"
//#include "TestBehaviour.h"
//#include "TestCameraBehaviour.h"

#include "EditorScene.h"
#include "WindowMain.h"
#include "WindowMainAutoGenerated.h"

#include "SingletonManager.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFont font;
    font.setFamily(font.defaultFamily());
    font.setPixelSize(10);
    app.setFont(font);


    // Init SINGLETON's
    SingletonManager::InitFromMainBinary();
    QMainWindow *window = new QMainWindow();
    WindowMain::InitFromMainBinary(window, &app);
    Canvas::InitFromMainBinary();
    Time::InitFromMainBinary();
    //

    Scene *scene = new EditorScene();
    std::string filename = "./res/Assets/selectionScene.bscene";
    FileReader::ReadScene(filename , scene);
    if(scene != nullptr)
    {
        Canvas::GetInstance()->AddScene(scene);
        Canvas::GetInstance()->SetCurrentScene(scene);
        Persistence::SetCurrentSceneFilepath(filename);
    }

    return app.exec();
}
