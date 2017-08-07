/*
#include "Bang/Application.h"

#include "Bang/Paths.h"
#include "Bang/Scene.h"
#include "Bang/Debug.h"
#include "Bang/Project.h"
#include "Bang/ProjectManager.h"

int main(int argc, char **argv)
{
    Application app(argc, argv);

    if (argc <= 1)
    {
        Debug_Error("Please specify the project filepath as first parameter");
        return 1;
    }

    Path projectPath(argv[1]);
    ProjectManager pm;
    Project *proj = pm.OpenProject(projectPath);

    app.CreateWindow();
    proj->OpenFirstFoundScene();
    app.MainLoop();

    return 0;
}
*/
#include <iostream>

#include "Bang/Paths.h"
#include "Bang/Scene.h"
#include "Bang/GUILabel.h"
#include "Bang/GUIImage.h"
#include "Bang/GUICanvas.h"
#include "Bang/GUIButton.h"
#include "Bang/Application.h"
#include "Bang/GUIInputText.h"
#include "Bang/SceneManager.h"
#include "Bang/RectTransform.h"
#include "Bang/UITextRenderer.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/GUIVerticalLayout.h"
#include "Bang/GUIHorizontalLayout.h"

class Menu : public GUIHorizontalLayout
{
public:
    Menu() : GUIHorizontalLayout() {}

    void OnUpdate() override
    {
        UIGameObject::OnUpdate();
        GameObject *nameGo = GameObject::Find("nameGameObject");
        GameObject *surnameGo = GameObject::Find("surnameGameObject");
        GameObject *resultGo = GameObject::Find("resultGameObject");

        String name = SCAST<GUIInputText*>(nameGo)->GetText()->GetContent();
        String surname = SCAST<GUIInputText*>(surnameGo)->GetText()->GetContent();
        resultGo->GetComponent<UITextRenderer>()->SetContent("Hello " + name + " " + surname);
    }
};

int main(int argc, char **argv)
{
    Application app(argc, argv);
    Paths::SetEngineRoot( Path("/home/sephirot47/Bang") );
    app.CreateWindow();

    Scene *scene = new Scene();

    GUIImage *uiImgGo = new GUIImage( Path("/home/sephirot47/BangUITest/test.png") );
    GUILabel *rightLabel = new GUILabel("Lorem ipsum dolor sit amet. El veloz "
                                        "murcielago hindu comia feliz cardillo "
                                        "y kiwi. La ciguena tocaba el saxofon "
                                        "detras del palenque de paja");
    rightLabel->GetText()->SetTint(Color::White);
    rightLabel->GetText()->SetHorizontalAlign(HorizontalAlignment::Center);
    rightLabel->GetText()->SetVerticalAlign(VerticalAlignment::Center);
    rightLabel->GetText()->SetVerticalWrapMode(WrapMode::Hide);
    rightLabel->GetText()->SetHorizontalWrapMode(WrapMode::Wrap);

    GUIVerticalLayout *rightVLayout = new GUIVerticalLayout();
    rightVLayout->rectTransform->SetMargins(50, 10, 10, 10);
    rightVLayout->Add(uiImgGo);
    rightVLayout->Add(rightLabel);

    GUIButton *buttonPlay = new GUIButton("Play");
    GUIButton *buttonInstructions = new GUIButton("Instructions");
    GUIButton *buttonCredits = new GUIButton("Credits");
    GUIButton *buttonExit = new GUIButton("Exit");
    buttonPlay->AddClickedCallback([](UIButton*){ Debug_Log("Pressed Play"); });
    buttonInstructions->AddClickedCallback([](UIButton*){ Debug_Log("Pressed Instructions"); });
    buttonCredits->AddClickedCallback([](UIButton*){ Debug_Log("Pressed Credits"); });
    buttonExit->AddClickedCallback([](UIButton*){ Debug_Log("Pressed Exit"); });

    GUILabel *nameLabel = new GUILabel();
    nameLabel->GetText()->SetContent("Name:");
    nameLabel->GetText()->SetHorizontalAlign(HorizontalAlignment::Right);
    GUIInputText *nameInput = new GUIInputText();
    nameInput->SetName("nameGameObject");
    nameInput->GetText()->SetContent("Your name here...");
    GUIHorizontalLayout *nameSubHLayout = new GUIHorizontalLayout();
    nameSubHLayout->Add(nameLabel);
    nameSubHLayout->Add(nameInput);

    GUILabel *surnameLabel = new GUILabel();
    surnameLabel->GetText()->SetContent("Surname:");
    surnameLabel->GetText()->SetHorizontalAlign(HorizontalAlignment::Right);
    GUIInputText *surnameInput = new GUIInputText();
    surnameInput->SetName("surnameGameObject");
    surnameInput->GetText()->SetContent("Your surname here...");
    GUIHorizontalLayout *surnameSubHLayout = new GUIHorizontalLayout();
    surnameSubHLayout->Add(surnameLabel);
    surnameSubHLayout->Add(surnameInput);

    GUIVerticalLayout *nameVLayout = new GUIVerticalLayout();
    nameVLayout->Add(nameSubHLayout);
    nameVLayout->Add(surnameSubHLayout);

    GUILabel *namesResult = new GUILabel("RESULT");
    namesResult->SetName("resultGameObject");
    namesResult->GetText()->SetHorizontalWrapMode(WrapMode::Hide);

    GUIVerticalLayout *namesVLayout = new GUIVerticalLayout();
    UIImageRenderer *img4 = namesVLayout->AddComponent<UIImageRenderer>(0);
    img4->SetTint(Color::LightGray);
    namesVLayout->Add(nameVLayout);
    namesVLayout->Add(namesResult);

    UIGameObject *uiVContainer = new UIGameObject();
    uiVContainer->rectTransform->SetMargins(20);
    UIImageRenderer *uiImg = uiVContainer->AddComponent<UIImageRenderer>(0);
    uiImg->SetTint(Color::Red);

    GUIVerticalLayout *menuVLayout = new GUIVerticalLayout();
    menuVLayout->rectTransform->SetMargins(30);
    menuVLayout->SetSpacing(10);
    menuVLayout->Add(buttonPlay);
    menuVLayout->Add(buttonInstructions);
    menuVLayout->Add(buttonCredits);
    menuVLayout->Add(buttonExit);
    menuVLayout->SetParent(uiVContainer);

    List<UITextRenderer*> txts = menuVLayout->GetComponentsInChildren<UITextRenderer>();
    for (UITextRenderer *txt : txts)
    {
        txt->SetTextSize(20);
        txt->SetHorizontalAlign(HorizontalAlignment::Center);
        txt->SetVerticalAlign(VerticalAlignment::Center);
    }
    menuVLayout->Add(namesVLayout);

    int s = 10;
    nameLabel->GetText()->SetTextSize(s);
    surnameLabel->GetText()->SetTextSize(s);
    nameInput->GetText()->SetTextSize(s);
    surnameInput->GetText()->SetTextSize(s);

    Menu *mainHLayout = new Menu();
    mainHLayout->SetSpacing(0);
    mainHLayout->Add(rightVLayout);
    mainHLayout->Add(uiVContainer, 0);

    mainHLayout->SetStretch(0, 0.6f);
    mainHLayout->SetStretch(1, 0.4f);

    GUICanvas *canvas = new GUICanvas();
    mainHLayout->SetParent(canvas);
    canvas->SetParent(scene);

    SceneManager::LoadScene(scene);
    app.MainLoop();

    return 0;
}
