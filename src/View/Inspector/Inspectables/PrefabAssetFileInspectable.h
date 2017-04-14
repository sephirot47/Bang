#ifndef PREFABASSETFILEINSPECTABLE_H
#define PREFABASSETFILEINSPECTABLE_H

#include <QTimer>
#include "Bang/WinUndef.h"

#include "Bang/File.h"
#include "Bang/SerializableObject.h"
#include "Bang/IWindowEventManagerListener.h"

class GameObject;
class InspectorWidget;
class GameObjectPrefabInspectable;
class PrefabAssetFileInspectable : public SerializableObject,
                                   public IWindowEventManagerListener
{
    OBJECT(PrefabAssetFileInspectable)

public:
    PrefabAssetFileInspectable(const File &f);
    virtual ~PrefabAssetFileInspectable();

    void ShowInInspector();

    virtual void Read(const XMLNode &xmlInfo) override;

    GameObject *GetPrefabTempGameObject() const;

private:
    File m_file;
    GameObject *m_prefabTempGameObject = nullptr;
};

#endif // PREFABASSETFILEINSPECTABLE_H
