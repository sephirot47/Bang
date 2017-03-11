#include "EngineConfig.h"

#include "FileReader.h"
#include "IO.h"

EngineConfig::EngineConfig()
{
}

void EngineConfig::CleanOutdatedRecentProjectList()
{
    List<String> recentProjectsList = EngineConfig::GetRecentProjectsList();
    for (auto it = recentProjectsList.Begin();
         it != recentProjectsList.End(); ++it)
    {
        String recentProject = *it;
        if (!IO::ExistsFile(recentProject) ||              // Remove non-existing
            recentProjectsList.CountOccurrences(recentProject) > 1) // Remove repeated
        {
            it = recentProjectsList.Remove(it);
            --it;
        }
    }

    WriteListToFile(EngineConfig::GetRecentProjectsFilepath(),
                    recentProjectsList);
}

void EngineConfig::WriteListToFile(const String &filepath,
                                   const List<String> &list)
{
    String content = "";
    for (String str : list)
    {
        content += str;
        content += "\n";
    }
    IO::WriteToFile(filepath, content);
}

String EngineConfig::GetRecentProjectsFilepath()
{
    return IO::GetEngineRootAbs() + "/config/RecentProjects.cfg";
}

List<String> EngineConfig::GetRecentProjectsList()
{
    String recentProjectConfigFilepath = EngineConfig::GetRecentProjectsFilepath();
    String recentProjectsContent =
            IO::GetFileContents(recentProjectConfigFilepath);

    Array<String> projectsList = recentProjectsContent.Split('\n');

    return projectsList.ToList();
}

void EngineConfig::RegisterInRecentProjectsList(const String &projectFilepath)
{
    String recentProjectConfigFilepath = EngineConfig::GetRecentProjectsFilepath();

    List<String> recentProjectsList = EngineConfig::GetRecentProjectsList();
    recentProjectsList.Remove(projectFilepath);    // Remove it (to move later)
    recentProjectsList.PushFront(projectFilepath); // Add the first one

    EngineConfig::WriteListToFile(recentProjectConfigFilepath,
                                  recentProjectsList);
}
