#include "Bang/Paths.h"
#include "Bang/Scene.h"
#include "Bang/Debug.h"
#include "Bang/Project.h"
#include "Bang/Application.h"
#include "Bang/GameBuilder.h"
#include "Bang/ProjectManager.h"
#include "Bang/BehaviourManager.h"

USING_NAMESPACE_BANG

int main(int argc, char **argv)
{
    Application app(argc, argv);

    // Load game project
    Path gameProjectFilepath(Paths::ExecutablePath().GetDirectory()
                             .Append("GameData/Game.bproject"));
    if (!gameProjectFilepath.Exists())
    {
        Debug_Error("Could not find " << gameProjectFilepath);
        return 1;
    }

    Paths::SetEngineRoot( gameProjectFilepath.GetDirectory() );

    Path projectPath(gameProjectFilepath);
    ProjectManager pm;
    Project *project = pm.OpenProject(projectPath);

    // Find and load Behaviours library
    Path libsDir = gameProjectFilepath.GetDirectory().Append("Libraries");
    List<Path> behavioursLibs = libsDir.FindFiles(Path::FindFlag::Simple);
    for (auto it = behavioursLibs.Begin(); it != behavioursLibs.End(); )
    {
        const Path &behaviourLib = *it;
        if (!behaviourLib.GetAbsolute().Contains(".so."))
        {
            it = behavioursLibs.Remove(it);
        }
        else { ++it; }
    }

    if (behavioursLibs.IsEmpty())
    {
        Debug_Error("Behaviours library could not be found in '" << libsDir << "'");
        return 2;
    }
    Path behavioursLib = behavioursLibs.Front();
    BehaviourManager::LoadBehavioursLibrary(behavioursLib);

    app.CreateWindow();
    project->OpenFirstFoundScene();
    return app.MainLoop();
}

