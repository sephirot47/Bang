#ifndef INSPECTORSW_H
#define INSPECTORSW_H

#include <QWidget>

#include "IDragDropListener.h"
#include "ComponentWidget.h"
#include "DragDropAgent.h"

class AttributeWidget : public DragDropQWidget,
                        public IDragDropListener
{
    Q_OBJECT

protected:
    QBoxLayout *m_layout = nullptr;
    QLabel *m_label = nullptr;
    int m_rowIndexInGridLayout = 0;

    XMLAttribute m_xmlAttribute;

    bool m_readonly = false;
    bool m_enabled  = false;
    bool m_inlined  = false;
    bool m_hidden   = false;

    InspectorWidget *m_inspectorWidget = nullptr;
    XMLNode *m_relatedXMLInfo = nullptr;

    AttributeWidget(const XMLAttribute &xmlAttribute,
                    InspectorWidget *inspectorWidget,
                    bool isSubWidget = false,
                    bool createLabel = true,
                    bool needsLabelOnTop = false);

    void AfterConstructor();

public:

    virtual void Refresh(const XMLAttribute &attribute);

    static AttributeWidget* FromXMLAttribute(const XMLAttribute &xmlAttribute,
                                             InspectorWidget *inspectorWidget);
};

#endif // INSPECTORSW_H
