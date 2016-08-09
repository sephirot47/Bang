#ifndef LISTINSPECTOR_H
#define LISTINSPECTOR_H

#include <map>

#include "Bang.h"

#include <QLabel>
#include <QListWidget>

#include "ComponentWidget.h"
#include "PrefabAssetFileInspectable.h"
#include "IWindowEventManagerListener.h"

class Inspector : public QListWidget,
                  public IWindowEventManagerListener
{
    Q_OBJECT

private:
    QLabel *m_titleLabel = nullptr;
    GameObject *m_currentGameObject = nullptr;
    std::vector<InspectorWidget*> m_currentInspectorWidgets;
    std::vector<IInspectable*> m_currentInspectables;

    std::map<InspectorWidget*, QListWidgetItem*> m_widgetToItem;

    void AddWidget(InspectorWidget *widget);

protected:
    //To set the scroll step to a smaller one
    virtual void updateGeometries() override;

public:
    explicit Inspector(QWidget *parent = 0);

    void Clear();
    void Refresh();

    void SetInspectable(IInspectable *inspectable, const std::string &title = "");
    void ShowGameObjectInfo(GameObject *gameObject);
    void ShowPrefabInspectableInfo(PrefabAssetFileInspectable *prefabInspectable);

    void MoveUp(InspectorWidget *w);
    void MoveDown(InspectorWidget *w);

    void OnMenuBarActionClicked(MenuBar::Action clickedAction) override;

    std::vector<IInspectable*> GetCurrentInspectables() const;
    bool IsShowingInspectable(IInspectable *inspectable) const;
    static Inspector* GetInstance();
};

#endif // LISTINSPECTOR_H
