#include "GameBuilder.h"

#include "Debug.h"
#include "Dialog.h"
#include "Project.h"
#include "Persistence.h"
#include "StringUtils.h"
#include "SystemUtils.h"
#include "EditorWindow.h"
#include "ProjectManager.h"
#include "GameBuildDialog.h"
#include "BehaviourManager.h"

GameBuilder *GameBuilder::s_instance = nullptr;

GameBuilder::GameBuilder()
{
}

GameBuilder::~GameBuilder()
{
}

GameBuilder *GameBuilder::GetInstance()
{
    if (!GameBuilder::s_instance)
    {
        GameBuilder::s_instance = new GameBuilder();
    }
    return GameBuilder::s_instance;
}

String GameBuilder::AskForExecutableFilepath()
{
    // Get the executable output filepath
    const String defaultOutputDirectory = Persistence::c_ProjectRootAbsolute;
    const String projectName = ProjectManager::GetCurrentProject()->GetProjectName();
    String executableFilepath =
        Dialog::GetSaveFilename("Choose the file where you want to create your game",
                                "exe",
                                defaultOutputDirectory,
                                projectName + ".exe");

    if (!executableFilepath.Empty())
    {
        executableFilepath =
                Persistence::AppendExtension(executableFilepath, "exe");
    }
    return executableFilepath;
}

void GameBuilder::BuildGame(bool runGame)
{
    if (m_gameBuilderThread && m_gameBuilderThread->isRunning())
    {
        Debug_Status("Game is already being built", 5.0f);
    }
    else
    {
        // First ask for the executable output file (in the main thread)
        m_latestGameExecutableFilepath = AskForExecutableFilepath();
        ASSERT(!m_latestGameExecutableFilepath.Empty());

        // Create the progress window
        if (m_gameBuildDialog)
        {
            delete m_gameBuildDialog;
        }
        m_gameBuildDialog = new GameBuildDialog();
        //

        // Create the game builder job and connect stuff
        if (m_gameBuilderJob)
        {
            delete m_gameBuilderJob;
        }
        m_gameBuilderJob = new GameBuilderJob();
        m_gameBuilderJob->m_executableFilepath = m_latestGameExecutableFilepath;
        m_gameBuilderJob->m_runGameAfterBuild  = runGame;
        //

        // Create and start the building thread
        if (m_gameBuilderThread)
        {
            m_gameBuilderThread->exit(0);
            delete m_gameBuilderThread;
        }
        m_gameBuilderThread = new QThread();
        //

        // Connect stuff
        QObject::connect(m_gameBuilderJob, SIGNAL(NotifyPercent(float)),
                         m_gameBuildDialog, SLOT(SetPercent(float)));
        QObject::connect(m_gameBuilderJob, SIGNAL(NotifyMessage(const QString&)),
                         m_gameBuildDialog, SLOT(SetMessage(const QString&)));
        QObject::connect(m_gameBuilderThread, SIGNAL(started()),
                         m_gameBuilderJob, SLOT(BuildGame()) );
        QObject::connect(m_gameBuilderJob, SIGNAL(NotifyGameHasBeenBuilt()),
                         this, SLOT(OnGameHasBeenBuilt()) );
        QObject::connect(m_gameBuilderJob, SIGNAL(NotifyGameBuildingHasFailed()),
                         m_gameBuildDialog, SLOT(cancel()));
        //

        // Start the thread
        m_gameBuilderJob->moveToThread(m_gameBuilderThread);
        m_gameBuilderThread->start();
    }
}

void GameBuilder::OnGameHasBeenBuilt()
{
    if (m_gameBuildDialog)
    {
        m_gameBuildDialog->close();
    }

    m_gameBuilderThread->exit(0);
}

void GameBuilder::OnGameBuildingHasFailed()
{
    Dialog::Error("Error building game",
                  "The game could not be built.");
}

void GameBuilder::OnGameBuildingHasBeenCanceled()
{
    m_gameBuilderJob->OnGameBuildingCanceled();
    // RemoveLatestGameBuild();
}

bool GameBuilder::CompileGameExecutable()
{
    String output = "";
    String cmd = Persistence::c_EngineRootAbsolute + "/scripts/compile.sh GAME";

    const String initialOutputDir = Persistence::c_EngineRootAbsolute + "/bin/Game.exe";
    Persistence::Remove(initialOutputDir);

    bool ok = false;
    SystemUtils::System(cmd.ToCString(), &output, &ok);
    ok = ok && Persistence::ExistsFile(initialOutputDir);
    if (!ok)
    {
        Debug_Error(output);
        return false;
    }
    return true;
}

bool GameBuilder::CreateDataDirectory(const String &executableDir)
{
    String dataDir = executableDir + "/GameData";
    if (!Persistence::CreateDirectory(dataDir)) { return false; }

    // Copy the Engine Assets in the GameData directory
    if (!Persistence::DuplicateDir(Persistence::c_EngineAssetsRootAbsolute,
                                   dataDir + "/EngineAssets"))
    {
        return false;
    }

    // Copy the Project Assets in the GameData directory
    if (!Persistence::DuplicateDir(Persistence::c_ProjectAssetsRootAbsolute,
                                   dataDir + "/Assets"))
    {
        return false;
    }

    return true;
}

Project* GameBuilder::CreateGameProject(const String &executableDir)
{
    String dataDir = executableDir + "/GameData";
    Project *gameProject =
            ProjectManager::CreateNewProjectFileOnly(dataDir + "/Game.bproject");
    return gameProject;
}

bool GameBuilder::CompileBehaviours(const String &executableDir,
                                    Project *gameProject,
                                    bool *cancel)
{
    String dataDir = executableDir + "/GameData";

    // Compile the behaviours and save them so the Game can
    // load them instantly and doesn't need to compile them
    List<String> behaviourFilepaths =
            Persistence::GetFiles(Persistence::c_ProjectAssetsRootAbsolute,
                                  true, { "*.cpp" });
    for (const String &behaviourFilepath : behaviourFilepaths)
    {
        if (*cancel) { return true; }

        String compiledLibFilepath =
                SystemUtils::CompileToSharedObject(behaviourFilepath, false);
        if (compiledLibFilepath.Empty())
        {
            Debug_Error("Failed to compile " << behaviourFilepath);
            return false;
        }

        String gameLibFilepath = compiledLibFilepath;
        gameLibFilepath.Replace(Persistence::c_ProjectAssetsRootAbsolute,
                               dataDir + "/Assets");
        String gameLibFilepathWithoutTimestamp =
                Persistence::GetDir(gameLibFilepath) + "/" +
                Persistence::GetFileName(gameLibFilepath) + ".so";

        // Add the random project Id to the name, to avoid library(*.so)
        // caching from by operative system (it happens, yes)
        String randomProjectId = gameProject->GetProjectRandomId();
        String gameLibWithRandomProjectId =
                gameLibFilepathWithoutTimestamp + "." + randomProjectId;
        Persistence::Rename(compiledLibFilepath, gameLibWithRandomProjectId);
    }

    return true;
}

void GameBuilder::RemoveLatestGameBuild()
{
    Persistence::Remove(m_latestGameExecutableFilepath);

    String executableDir = Persistence::GetDir(m_latestGameExecutableFilepath);
    String gameDataDir = executableDir + "/GameData";
    Persistence::Remove(gameDataDir);
}

GameBuildDialog *GameBuilder::GetGameBuildDialog()
{
    return m_gameBuildDialog;
}
