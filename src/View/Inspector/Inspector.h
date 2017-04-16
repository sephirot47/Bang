#ifndef LISTINSPECTOR_H
#define LISTINSPECTOR_H

#include <QLabel>
#include <QDropEvent>
#include "Bang/WinUndef.h"

#include "Bang/Map.h"
#include "Bang/Debug.h"
#include "Bang/Component.h"
#include "Bang/GameObject.h"
#include "Bang/DragDropQListWidget.h"
#include "Bang/InspectorContextMenu.h"
#include "Bang/PrefabAssetFileInspectable.h"
#include "Bang/IWindowEventManagerListener.h"

class Inspector : public DragDropQListWidget,
                  public IWindowEventManagerListener
{
    Q_OBJECT

public:
    explicit Inspector(QWidget *parent = nullptr);

    void OnWindowShown();

    void Clear();

    void SetInspectable(SerializableObject *inspectable,
                        const String &title = "");
    void ShowGameObjectInfo(GameObject *gameObject);

    void RefreshSizeHints();
    void OnEditorPlay();

public slots:
    void OnEnableGameObjectCheckBoxChanged(bool checked);
    void Refresh();

public:
    void ShowPrefabInspectableInfo(PrefabAssetFileInspectable *prefabInspectable);

    template<class ComponentClass>
    void OnMenuBarAddComponentClicked()
    {
        ENSURE(p_currentGameObject);
        p_currentGameObject->AddComponent<ComponentClass>();
        Refresh();
    }
    void OnMenuBarAddNewBehaviourClicked();
    void OnGameObjectDestroyed(GameObject *destroyed);

    void AddWidget(InspectorWidget *widget, int row = -1);

    bool IsShowingInspectable(SerializableObject *inspectable) const;
    static Inspector* GetInstance();

    void dropEvent(QDropEvent *e) override;

    static String FormatInspectorLabel(const String &labelString);

protected:
    //To set the scroll step to a smaller one
    virtual void updateGeometries() override;

private:
    InspectorContextMenu m_iContextMenu;

    QLabel *m_titleLabel = nullptr;
    QCheckBox *m_enableGameObjectCheckBox = nullptr;

    GameObject *p_currentGameObject = nullptr;
    List<InspectorWidget*> m_currentInspectorWidgets;
    List<SerializableObject*> m_currentInspectables;

    Map<InspectorWidget*, QListWidgetItem*> m_widget_To_Item;
    Map<InspectorWidget*, SerializableObject*> m_widget_To_Inspectables;

    friend class InspectorWidget;
    friend class InspectorContextMenu;
};

#endif // LISTINSPECTOR_H
