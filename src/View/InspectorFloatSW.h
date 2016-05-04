#ifndef INSPECTORFLOATCOMPONENTSLOTWIDGET_H
#define INSPECTORFLOATCOMPONENTSLOTWIDGET_H

#include <string>
#include <QTimer>
#include <QFocusEvent>
#include <QDoubleSpinBox>

#include "InspectorSW.h"

class FloatComponentSlotSpinBox;
class InspectorFloatSW : public InspectorSW //Slot for a float (label + float)
{
    Q_OBJECT

private:
    FloatComponentSlotSpinBox *spinbox = nullptr;
    bool editing =false;

public:
    InspectorFloatSW(const std::string &labelString, float initialValue,
                                 InspectorComponentWidget *parent);

    virtual void SetValue(float f);
    virtual float GetValue();

    virtual void OnSpinBoxFocusIn();
    virtual void OnSpinBoxFocusOut();
    QSize sizeHint() const override;
};

class FloatComponentSlotSpinBox : public QDoubleSpinBox //Slot for a float (label + float)
{
    Q_OBJECT

public:
    FloatComponentSlotSpinBox() : QDoubleSpinBox()
    {
        connect(this, SIGNAL(valueChanged(double)), this, SLOT(AdjustStep(double)));
        AdjustStep(value());
    }

    virtual void focusInEvent(QFocusEvent * event) override
    {
        QDoubleSpinBox::focusInEvent(event);
        QTimer::singleShot(50, this, SLOT(SelectAll()));
        static_cast<InspectorFloatSW*>(parent())->OnSpinBoxFocusIn();
    }

    virtual void focusOutEvent(QFocusEvent * event) override
    {
        QDoubleSpinBox::focusOutEvent(event);
        static_cast<InspectorFloatSW*>(parent())->OnSpinBoxFocusOut();
    }

    virtual void  keyPressEvent(QKeyEvent *event) override
    {
        QDoubleSpinBox::keyPressEvent(event);
        if(event->key() == QKeyEvent::Enter)
        {
            static_cast<InspectorFloatSW*>(parent())->OnSpinBoxFocusOut();
        }
    }

public slots:
    void AdjustStep(double v)
    {
        if(abs(v) <= 1.0f) setSingleStep(0.1f);
        else setSingleStep( pow(10.0, int(log10(v == 0.0f ? 0.1f : abs(v)))-1) );
    }

    void SelectAll() { selectAll(); }
};

#endif // INSPECTORFLOATCOMPONENTSLOTWIDGET_H
