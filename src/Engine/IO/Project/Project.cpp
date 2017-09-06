#include "Bang/Project.h"

#include <string>

#include "Bang/Paths.h"
#include "Bang/Time.h"
#include "Bang/XMLNode.h"
#include "Bang/Extensions.h"
#include "Bang/SceneManager.h"

USING_NAMESPACE_BANG

Project::Project()
{
    m_randomId = ::std::to_string(Time::GetNow());
}

Project::~Project()
{
}

const Path&  Project::GetProjectDirPath() const
{
    return m_projectRootFilepath;
}

Path Project::GetProjectAssetsRootFilepath() const
{
    return Path(m_projectRootFilepath + "/Assets");
}

Path Project::GetProjectFileFilepath() const
{
    return Path(m_projectRootFilepath + "/" +
                GetProjectName() + "." + Extensions::Get<Project>());
}

String Project::GetProjectName() const
{
    return m_projectRootFilepath.GetName();
}

String Project::GetProjectRandomId() const
{
    return m_randomId;
}

void Project::SetProjectRootFilepath(const Path &projectDir)
{
    m_projectRootFilepath = projectDir;
}

bool Project::OpenFirstFoundScene() const
{
    List<Path> sceneFilepaths = GetProjectAssetsRootFilepath()
                                .FindFiles(Path::FindFlag::Recursive,
                                           {Extensions::Get<Scene>()});
    bool foundSceneFile = !sceneFilepaths.IsEmpty();
    if (foundSceneFile)
    {
        SceneManager::OpenScene(sceneFilepaths.Front());
    }

    return foundSceneFile;
}


void Project::ImportXML(const XMLNode &xmlInfo)
{
    if (xmlInfo.Contains("RandomID"))
    { m_randomId = xmlInfo.Get<String>("RandomID"); }
}

void Project::ExportXML(XMLNode *xmlInfo) const
{
    xmlInfo->SetTagName("Project");
    xmlInfo->Set("RandomID", m_randomId);
}


