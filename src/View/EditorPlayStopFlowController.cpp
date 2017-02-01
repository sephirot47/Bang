#include "EditorPlayStopFlowController.h"

#include <QProgressDialog>

#include "Debug.h"
#include "Scene.h"
#include "Screen.h"
#include "Dialog.h"
#include "ListLogger.h"
#include "EditorState.h"
#include "Application.h"
#include "EditorScene.h"
#include "EditorWindow.h"
#include "SceneManager.h"
#include "EditorCamera.h"
#include "BehaviourManager.h"

EditorPlayStopFlowController::EditorPlayStopFlowController()
{
}

bool EditorPlayStopFlowController::OnPlayClicked()
{
    return EditorPlayStopFlowController::GetInstance()->PlayScene();
}

void EditorPlayStopFlowController::OnStopClicked()
{
    EditorPlayStopFlowController::GetInstance()->StopScene();
}

EditorPlayStopFlowController *EditorPlayStopFlowController::GetInstance()
{
    return EditorWindow::GetInstance()->m_playStopController;
}

bool EditorPlayStopFlowController::PlayScene()
{
    ASSERT(!EditorState::IsPlaying(), "", return true);

    // Start refreshing all scene behaviours...
    BehaviourManager::RefreshAllBehaviours();

    m_playingCanceled = false; // Wait for behaviours
    if (!WaitForAllBehavioursToBeLoaded()) { return false; }

    m_playing = true;
    ListLogger::GetInstance()->OnEditorPlay();

    // Make a copy of the current scene, to restore it later
    m_latestSceneBeforePlaying = SceneManager::GetActiveScene();

    Scene *sceneCopy = static_cast<Scene*>( m_latestSceneBeforePlaying->Clone() );
    if (sceneCopy)
    {
        SceneManager::SetActiveScene(sceneCopy);
    }

    EditorScene *edScene = static_cast<EditorScene*>(sceneCopy);
    edScene->SetFirstFoundCameraOrDefaultOne();

    EditorWindow *win = EditorWindow::GetInstance();
    win->tabContainerSceneGame->setCurrentWidget(win->tabGame);

    Screen::GetInstance()->setFocus();

    return true;
}

void EditorPlayStopFlowController::StopScene()
{
    ASSERT(EditorState::IsPlaying());

    m_playing = false;

    Scene *sceneCopy = SceneManager::GetActiveScene();
    if (sceneCopy)
    {
        Hierarchy::GetInstance()->Clear();
        SceneManager::SetActiveScene(nullptr);
        delete sceneCopy;
    }

    SceneManager::SetActiveScene(m_latestSceneBeforePlaying);
    EditorScene *edScene = static_cast<EditorScene*>(m_latestSceneBeforePlaying);
    edScene->SetCamera( edScene->GetEditorCamera()->GetCamera() );

    EditorWindow *win = EditorWindow::GetInstance();
    win->tabContainerSceneGame->setCurrentWidget(win->tabScene);
}

bool EditorPlayStopFlowController::WaitForAllBehavioursToBeLoaded()
{
    if (BehaviourManager::AllBehaviourHoldersUpdated()) { return true; }

    EditorWindow *win = EditorWindow::GetInstance();
    QMainWindow *mainWin = win->GetMainWindow();

    // Create a progress dialog to show progress
    QProgressDialog progressDialog(mainWin);
    progressDialog.setRange(0, 100);
    progressDialog.setModal(true); // Important to avoid Ctrl+P smashing
    progressDialog.setWindowTitle("Compiling behaviours");
    progressDialog.setLabelText("Waiting for all behaviours to be correctly compiled...");
    progressDialog.show();
    QObject::connect(&progressDialog, SIGNAL(canceled()),
                     this, SLOT(OnWaitingForBehavioursCanceled()));

    // Actual waiting
    float percentUpdated = 0.0f;
    Application::processEvents();
    while ( !BehaviourManager::AllBehaviourHoldersUpdated(&percentUpdated) )
    {
        progressDialog.setValue( int(percentUpdated * 100) );

        if (BehaviourManager::SomeBehaviourWithError())
        {
            String fixErrorsMsg =
                    "Please fix all the behaviour errors before playing.";
            String msg = fixErrorsMsg + "\nCheck the Logger.";
            Debug_Error(fixErrorsMsg);
            Dialog::Error("Error", msg);
            return false;
        }

        // Progress dialog cancel button pressed
        if (m_playingCanceled) { return false; }

        QThread::currentThread()->msleep(100);

        // Let the timers tick and update GUI.
        Application::processEvents();
    }
    progressDialog.close();

    return true;
}

void EditorPlayStopFlowController::OnWaitingForBehavioursCanceled()
{
    m_playingCanceled = true;
}
