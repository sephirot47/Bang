#include "AttrWidgetString.h"

AttrWidgetString::AttrWidgetString(const std::string &labelString,
                                   InspectorWidget *parent,
                                   bool readonly, bool inlined,
                                   bool bigText) :
    AttributeWidget(labelString, parent)
{
    QLayout *layout = nullptr;
    if (inlined)
    {
        layout = new QHBoxLayout();
    }
    else
    {
        layout = new QVBoxLayout();
    }
    setLayout(layout);

    QWidget *textWidget = nullptr;
    if (!bigText)
    {
        m_lineEdit = new LineEdit(readonly); //Right side
        m_lineEdit->setAlignment(Qt::AlignRight);
        m_lineEdit->setMinimumWidth(50);
        m_lineEdit->setMinimumHeight(25);
        textWidget = m_lineEdit;
    }
    else
    {
        m_textEdit = new TextEdit(readonly);
        m_textEdit->setAlignment(Qt::AlignLeft);
        textWidget = m_textEdit;
    }

    textWidget->setContentsMargins(0,0,0,0);
    textWidget->show();

    //connect(m_strField, SIGNAL(textChanged()), m_parent, SLOT(_OnSlotValueChanged()));

    layout->addWidget(GetLabelWidget(labelString));
    layout->addWidget(textWidget);

    setContentsMargins(0,0,0,0);
    show();

    setFocusPolicy(Qt::FocusPolicy::ClickFocus);
}

void AttrWidgetString::SetValue(const std::string &value)
{
    if (!m_editing)
    {
        if (m_lineEdit)
        {
            //disconnect(m_strField, SIGNAL(textChanged()), m_parent, SLOT(_OnSlotValueChanged()));
            m_lineEdit->setText( QString::fromStdString(value) );
            m_lineEdit->show();
            //connect(m_strField, SIGNAL(textChanged()), m_parent, SLOT(_OnSlotValueChanged()));
        }
        else if (m_textEdit)
        {
            m_textEdit->setText( QString::fromStdString(value) );
            m_textEdit->show();
        }
    }
}

const std::string AttrWidgetString::GetValue() const
{
    if (m_lineEdit)
    {
        return m_lineEdit->text().toStdString();
    }
    else if (m_textEdit)
    {
        return m_textEdit->toPlainText().toStdString();
    }

    return "";
}

void AttrWidgetString::OnFocusIn()
{
    m_editing = true;
}

void AttrWidgetString::OnFocusOut()
{
    m_editing = false;
    //m_parent->_OnSlotValueChanged();
}

QSize AttrWidgetString::sizeHint() const
{
    if (m_lineEdit)
    {
        return QSize(100, 30);
    }
    return AttributeWidget::sizeHint();
}

////////////////////////////////////////////////////////////

LineEdit::LineEdit(bool readonly) : QLineEdit()
{
    setReadOnly(readonly);
    if (readonly)
    {
        connect (this, SIGNAL(selectionChanged()),
                 this, SLOT(Deselect()));
    }
}

void LineEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    QTimer::singleShot(50, this, SLOT(SelectAll()));
    static_cast<AttrWidgetString*>(parent())->OnFocusIn();
}

void LineEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    static_cast<AttrWidgetString*>(parent())->OnFocusOut();
}

void LineEdit::keyPressEvent(QKeyEvent *event)
{
    QLineEdit::keyPressEvent(event);
    if (event->key() == QKeyEvent::Enter)
    {
        static_cast<AttrWidgetString*>(parent())->OnFocusOut();
    }
}

void LineEdit::Deselect()
{
    deselect();
}

void LineEdit::SelectAll()
{
    if (!this->isReadOnly())
    {
        this->selectAll();
    }
}

////////////////////////////////////////////////////////////

TextEdit::TextEdit(bool readonly)
{
    // setReadOnly(readonly);
    setReadOnly(true);
}

void TextEdit::focusInEvent(QFocusEvent *event)
{
    QTextEdit::focusInEvent(event);
    static_cast<AttrWidgetString*>(parent())->OnFocusIn();
}

void TextEdit::focusOutEvent(QFocusEvent *event)
{
    QTextEdit::focusOutEvent(event);
    static_cast<AttrWidgetString*>(parent())->OnFocusOut();
}
