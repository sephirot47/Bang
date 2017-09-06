#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "Bang/Bang.h"

FORWARD class Project;

class ProjectManager
{
public:
    ProjectManager();

    static Project* OpenProject(const Path &projectFilepath);

    static Project* CreateNewProject(const Path &projectDirPath,
                                     const String &projectName);
    static Project* CreateNewProjectFileOnly(const Path &projectFilepath);
    static void ExportProject(const Project *project);
    static void ExportCurrentProject();
    static void CloseCurrentProject();

    static Project* GetCurrentProject();

private:
    static Project *s_currentProject;
};

#endif // PROJECTMANAGER_H
