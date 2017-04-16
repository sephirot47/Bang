#include "Bang/FileReferencesManager.h"

#include "Bang/Map.h"
#include "Bang/File.h"
#include "Bang/Debug.h"
#include "Bang/XMLNode.h"
#include "Bang/Explorer.h"
#include "Bang/IO.h"
#include "Bang/XMLAttribute.h"
#include "Bang/SerializableObject.h"

FileReferencesManager::FileReferencesManager()
{
}

FileReferencesManager::~FileReferencesManager()
{

}

void FileReferencesManager::RegisterSerializableObject(SerializableObject *fileable)
{
    m_inMemorySerialObjects.Insert(fileable);
}

void FileReferencesManager::UnRegisterSerializableObject(SerializableObject *fileable)
{
    m_inMemorySerialObjects.Remove(fileable);
}

void FileReferencesManager::OnFileOrDirNameAboutToBeChanged(
                                            const String &absFilepathBefore,
                                            const String &absFilepathNow)
{
    ENSURE(!absFilepathBefore.Empty());
    // ENSURE(!absFilepathNow.Empty());

    m_queuedNameChanges.push( std::make_pair(absFilepathBefore,
                                             absFilepathNow));

    QTimer::singleShot(100, this, SLOT(TreatNextQueuedFileOrDirNameChange()));
}

void FileReferencesManager::TreatNextQueuedFileOrDirNameChange()
{
    const std::pair<String,String> &fileNamesPair = m_queuedNameChanges.front();
    String absFilepathBefore = fileNamesPair.first;
    String absFilepathNow    = fileNamesPair.second;
    m_queuedNameChanges.pop();

    bool fileHasMoved = !absFilepathBefore.Empty() &&
                        !IO::Exists(absFilepathBefore) &&
                         IO::Exists(absFilepathNow);

    if (fileHasMoved) // Sometimes it triggers drop but nothing is moved, check
    {
        String relPathBefore = IO::ToRelative(absFilepathBefore,false);
        String relPathNow    = IO::ToRelative(absFilepathNow, false);
        if ( IO::IsDir(absFilepathNow) )
        {
            relPathBefore += "/";
            relPathNow += "/";
        }
        RefactorFiles(relPathBefore, relPathNow);
        RefactorSerializableObject(relPathBefore, relPathNow);
    }
}

FileReferencesManager *FileReferencesManager::GetInstance()
{
    Explorer *exp = Explorer::GetInstance();
    return exp ? exp->m_fileRefsManager : nullptr;
}

void FileReferencesManager::RefactorFiles(const String &relPathBefore,
                                          const String &relPathNow)
{
    List<String> allFiles =
          IO::GetFiles(IO::GetProjectAssetsRootAbs(), true);
    for (const String &filepath : allFiles)
    {
        File f(filepath);
        if (!f.IsAsset()) { continue; }

        String fileXMLContents = f.GetContents();
        XMLNode *auxXMLInfo = XMLNode::FromString(fileXMLContents);
        if (filepath.Contains("tex2d"))
        {
            Debug_Log("Trying to refactor file " << filepath << " from " <<
                      relPathBefore << " to " << relPathNow);
        }
        if (auxXMLInfo &&
            RefactorXMLInfo(auxXMLInfo, relPathBefore, relPathNow, true))
        {
            Debug_Log("Refactored file " << filepath);
            IO::WriteToFile(filepath, auxXMLInfo->ToString());
        }

        if (auxXMLInfo) { delete auxXMLInfo; }
    }
}

void FileReferencesManager::RefactorSerializableObject(const String &relPathBefore,
                                                       const String &relPathNow)
{
    for (SerializableObject *serialObject : m_inMemorySerialObjects)
    {
        // First get its xml description, create an XMLNode, pass it to the
        // refactoring function, let the fileable read it to be updated, and
        // delete the created aux XMLNode
        String xmlInfoStr = serialObject->GetSerializedString();
        XMLNode *auxXMLInfo = XMLNode::FromString(xmlInfoStr);
        if (auxXMLInfo &&
            RefactorXMLInfo(auxXMLInfo, relPathBefore, relPathNow, false))
        {
            serialObject->Read(*auxXMLInfo);
        }
        if (auxXMLInfo) { delete auxXMLInfo; }
    }
}

bool FileReferencesManager::RefactorXMLInfo(XMLNode *xmlInfo,
                                            const String &relPathBefore,
                                            const String &relPathNow,
                                            bool refactorXMLChildren)
{
    bool hasBeenModified = false;

    // Refactor all its file attributes that are non-engine
    const Map<String, XMLAttribute> &xmlAttributes = xmlInfo->GetAttributes();
    for (const std::pair<String, XMLAttribute> &name_attr : xmlAttributes)
    {
        XMLAttribute attr = name_attr.second;
        if (attr.GetType() != XMLAttribute::Type::File)  { continue; }
        if (attr.HasProperty(XMLProperty::IsEngineFile)) { continue; }

        String relFilepath = attr.GetValue();
        int numReplacements = relFilepath.ReplaceInSitu(relPathBefore, relPathNow);
        if (numReplacements > 0)
        {
            XMLAttribute *attrPointer = xmlInfo->GetAttribute(attr.GetName());
            attrPointer->SetValue(relFilepath);
            hasBeenModified = true;
        }
    }

    if (refactorXMLChildren)
    {
        const List<XMLNode*>& xmlChildren = xmlInfo->GetChildren();
        for (XMLNode *xmlChild : xmlChildren)
        {
            bool modified = RefactorXMLInfo(xmlChild, relPathBefore,
                                            relPathNow, true);
            hasBeenModified = hasBeenModified || modified;
        }
    }

    return hasBeenModified;
}

