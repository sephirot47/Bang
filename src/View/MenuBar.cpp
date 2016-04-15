#include "MenuBar.h"

#include "WindowEventManager.h"
#include "WindowMain.h"
#include "Canvas.h"

#include "StageReader.h"
#include "FileWriter.h"

MenuBar::MenuBar(QWidget *parent) : QMenuBar(parent)
{
    WindowMain *w = WindowMain::GetInstance();
    connect(w->actionOpenStage,  SIGNAL(triggered()), this, SLOT(OnOpenStage()));
    connect(w->actionSaveCurrentStage,  SIGNAL(triggered()), this, SLOT(OnSaveCurrentStage()));

    connect(w->actionCreateEmptyEntity,  SIGNAL(triggered()), this, SLOT(OnCreateEmptyEntity()));
    connect(w->actionCreatePrefab,  SIGNAL(triggered()), this, SLOT(OnCreatePrefab()));
    connect(w->actionCreateMaterial,  SIGNAL(triggered()), this, SLOT(OnCreateMaterial()));
    connect(w->actionCreateMesh,  SIGNAL(triggered()), this, SLOT(OnCreateMesh()));
    connect(w->actionCreateShaderProgram,  SIGNAL(triggered()), this, SLOT(OnCreateShaderProgram()));
    connect(w->actionCreateTexture2D,  SIGNAL(triggered()), this, SLOT(OnCreateTexture2D()));

    connect(w->actionAddPartBehaviour,  SIGNAL(triggered()), this, SLOT(OnAddPartBehaviour()));
    connect(w->actionAddPartCamera,  SIGNAL(triggered()), this, SLOT(OnAddPartCamera()));
    connect(w->actionAddPartMeshRenderer,  SIGNAL(triggered()), this, SLOT(OnAddPartMeshRenderer()));
    connect(w->actionAddPartTransform,  SIGNAL(triggered()), this, SLOT(OnAddPartTransform()));
}

void MenuBar::OnOpenStage() const
{
    WindowEventManager::GetInstance()->NotifyMenuBarActionClicked(Action::OpenStage);

    QFileDialog qfd(WindowMain::GetMainWindow(),
                    QString::fromStdString("Open stage"),
                    QString::fromStdString(Explorer::GetTopPath()));
    qfd.setModal(true);

    QTimer timer; //Every X seconds, refresh the fileDialog window
    connect(&timer, SIGNAL(timeout()), &qfd, SLOT(repaint()));
    timer.start(50);

    std::string filename = qfd.getOpenFileName().toStdString();

    if(filename == "") return;

    Stage *stage = new Stage();
    StageReader::ReadStage(filename, stage);
    if(stage != nullptr)
    {
        Canvas::GetInstance()->AddStage(stage);
        Canvas::GetInstance()->SetCurrentStage(stage->GetName());
    }
    else
    {
        Logger_Error("Stage from file '" << filename << "' could not be loaded.");
    }
}

void MenuBar::OnSaveCurrentStage() const
{
    WindowEventManager::GetInstance()->NotifyMenuBarActionClicked(Action::SaveCurrentStage);


    Stage *stage = Canvas::GetInstance()->GetCurrentStage();
    if(stage == nullptr) return;

    QFileDialog qfd(WindowMain::GetMainWindow(),
                    QString::fromStdString("Save current stage"),
                    QString::fromStdString(Explorer::GetTopPath()));
    qfd.setModal(true);

    QTimer timer; //Every X seconds, refresh the fileDialog window
    connect(&timer, SIGNAL(timeout()), &qfd, SLOT(repaint()));
    timer.start(50);

    std::string filename = qfd.getSaveFileName().toStdString();


    if(filename == "") return;

    FileWriter::SaveStage(filename, stage);
}

void MenuBar::OnCreateEmptyEntity() const
{
    WindowEventManager::GetInstance()->NotifyMenuBarActionClicked(Action::CreateEmptyEntity);
}
void MenuBar::OnCreatePrefab() const
{
    WindowEventManager::GetInstance()->NotifyMenuBarActionClicked(Action::CreatePrefab);
}
void MenuBar::OnCreateMaterial() const
{
    WindowEventManager::GetInstance()->NotifyMenuBarActionClicked(Action::CreateMaterial);
}
void MenuBar::OnCreateMesh() const
{
    WindowEventManager::GetInstance()->NotifyMenuBarActionClicked(Action::CreateMesh);
}
void MenuBar::OnCreateShaderProgram() const
{
    WindowEventManager::GetInstance()->NotifyMenuBarActionClicked(Action::CreateShaderProgram);
}
void MenuBar::OnCreateTexture2D() const
{
    WindowEventManager::GetInstance()->NotifyMenuBarActionClicked(Action::CreateTexture2D);
}


void MenuBar::OnAddPartBehaviour() const
{
    WindowEventManager::GetInstance()->NotifyMenuBarActionClicked(Action::AddPartBehaviour);
}
void MenuBar::OnAddPartCamera() const
{
    WindowEventManager::GetInstance()->NotifyMenuBarActionClicked(Action::AddPartCamera);
}
void MenuBar::OnAddPartMeshRenderer() const
{
    WindowEventManager::GetInstance()->NotifyMenuBarActionClicked(Action::AddPartMeshRenderer);
}
void MenuBar::OnAddPartTransform() const
{
    WindowEventManager::GetInstance()->NotifyMenuBarActionClicked(Action::AddPartTransform);
}
