#include "ListLogger.h"

#include "Debug.h"
#include "Color.h"
#include "StringUtils.h"
#include "EditorWindow.h"

int ListLogger::c_iconColumn     = 1;
int ListLogger::c_msgColumn      = 2;
int ListLogger::c_countColumn    = 3;
int ListLogger::c_lineColumn     = 4;
int ListLogger::c_fileNameColumn = 5;

ListLogger::ListLogger(QWidget *parent) : DragDropQTreeWidget()
{
    hideColumn(0); // No arrow

    m_logIcon   = QIcon(":/qss_icons/Icons/LogIcon.png");
    m_warnIcon  = QIcon(":/qss_icons/Icons/WarnIcon.png");
    m_errorIcon = QIcon(":/qss_icons/Icons/ErrorIcon.png");

    EditorWindow *win = EditorWindow::GetInstance();
    m_collapse          = win->buttonCollapse->isChecked();
    m_clearOnPlay       = win->buttonClearOnPlay->isChecked();
    m_autoScroll        = win->buttonAutoScroll->isChecked();
    m_showLogMessages   = win->buttonShowLogMessages->isChecked();
    m_showWarnMessages  = win->buttonShowWarnMessages->isChecked();
    m_showErrorMessages = win->buttonShowErrorMessages->isChecked();

    OnCollapseChanged(m_collapse);

    QObject::connect(win->buttonLoggerClear, SIGNAL( pressed() ),
                     this, SLOT(OnClear()));
    QObject::connect(win->buttonCollapse, SIGNAL(clicked(bool)),
                     this, SLOT(OnCollapseChanged(bool)));
    QObject::connect(win->buttonClearOnPlay, SIGNAL(clicked(bool)),
                     this, SLOT(OnClearOnPlayChanged(bool)));
    QObject::connect(win->buttonAutoScroll, SIGNAL(clicked(bool)),
                     this, SLOT(OnAutoScrollChanged(bool)));
    QObject::connect(win->buttonShowLogMessages, SIGNAL(clicked(bool)),
                     this, SLOT(OnShowLogMessagesChanged(bool)));
    QObject::connect(win->buttonShowWarnMessages, SIGNAL(clicked(bool)),
                     this, SLOT(OnShowWarnMessagesChanged(bool)));
    QObject::connect(win->buttonShowErrorMessages, SIGNAL(clicked(bool)),
                     this, SLOT(OnShowErrorMessagesChanged(bool)));

    // Set headers
    setHeaderHidden(false);
    setHeaderLabels( {"", "", "Message", "Count", "Line", "File name"} );
    header()->setSectionResizeMode(ListLogger::c_iconColumn,  QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(ListLogger::c_msgColumn,   QHeaderView::Stretch);
    header()->setSectionResizeMode(ListLogger::c_countColumn, QHeaderView::Fixed);
    header()->setSectionResizeMode(ListLogger::c_lineColumn,  QHeaderView::Fixed);
    header()->resizeSection(ListLogger::c_countColumn, 50);
    header()->resizeSection(ListLogger::c_lineColumn, 50);
    header()->setStretchLastSection(false);
}

void ListLogger::Clear()
{
    ListLogger *listLogger = ListLogger::GetInstance(); ASSERT(listLogger);
    listLogger->OnClear();
}

ListLogger::MessageId ListLogger::AddLog(
        const String &str, int line,
        const String &fileName,
        bool persistent)
{
    ListLogger *listLogger = ListLogger::GetInstance(); ASSERT(listLogger, "", -1);
    Message m(line, str, fileName, persistent, MessageType::Log);
    ++listLogger->m_totalLogMessages;
    return listLogger->AddMessage(m);
}

ListLogger::MessageId ListLogger::AddWarn(
        const String &str, int line,
        const String &fileName,
        bool persistent)
{
    ListLogger *listLogger = ListLogger::GetInstance(); ASSERT(listLogger, "", -1);
    Message m(line, str, fileName, persistent, MessageType::Warn);
    ++listLogger->m_totalWarnMessages;
    return listLogger->AddMessage(m);
}

ListLogger::MessageId ListLogger::AddError(
        const String &str, int line,
        const String &fileName,
        bool persistent)
{
    ListLogger *listLogger = ListLogger::GetInstance(); ASSERT(listLogger, "", -1);
    Message m(line, str, fileName, persistent, MessageType::Error);
    ++listLogger->m_totalErrorMessages;
    return listLogger->AddMessage(m);
}

void ListLogger::DecorateLastItem(const Color &color)
{
    QTreeWidgetItem *item = topLevelItem(topLevelItemCount() - 1);
    if (item)
    {
        item->setTextColor(c_msgColumn, color.ToQColor());
    }
}

void ListLogger::RefreshList()
{
    List<Message> msgsCopy = m_currentMessages.GetValues();

    OnClear(); // Clear

    // ReAdd the messages
    for (const Message &msg : msgsCopy)
    {
        if (!msg.persistent)
        {   // Avoid adding persistent messages twice,
            // since they are not cleared
            AddMessage(msg);
        }
    }
}

QTreeWidgetItem *ListLogger::CreateItemFromMessageRow(const Message &mr)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();

    const QIcon *icon;
    if      (mr.type == MessageType::Log)   { icon = &m_logIcon;   }
    else if (mr.type == MessageType::Warn)  { icon = &m_warnIcon;  }
    else if (mr.type == MessageType::Error) { icon = &m_errorIcon; }
    item->setIcon(c_iconColumn, *icon);

    String shortMessage = mr.msg; // StringUtils::Elide(mr.msg, 100, true);
    item->setText(c_msgColumn, shortMessage.ToQString());

    item->setText(c_lineColumn, String::ToString(mr.line).ToQString());
    item->setText(c_fileNameColumn, mr.fileName.ToQString());

    return item;
}

ListLogger::MessageId ListLogger::AddMessage(const Message &msg,
                                             int forcedMessageId)
{
    bool hasToAddRow = true;
    if ( !m_currentMessagesIds.ContainsKey(msg) )
    {
        m_collapsedMsgsCount.Set(msg, 0);
    }
    else // Repeated msg
    {
        hasToAddRow = !m_collapse;
    }

    MessageId messageId = forcedMessageId >= 0 ? forcedMessageId : ++m_latestMessageId;
    m_currentMessages.Set(messageId, msg);
    m_currentMessagesIds.Set(msg, messageId);

    if ( (!m_showLogMessages   && msg.type == MessageType::Log  ) ||
         (!m_showWarnMessages  && msg.type == MessageType::Warn ) ||
         (!m_showErrorMessages && msg.type == MessageType::Error)
       )
    {
        hasToAddRow = false;
    }

    QTreeWidgetItem *item;
    if (hasToAddRow)
    {
        item = CreateItemFromMessageRow(msg);
        addTopLevelItem(item);
        DecorateLastItem( MessageTypeColor[msg.type] );
    }
    else
    {
        item = m_messageToItem[msg];
    }
    m_messageToItem[msg] = item;

    if (m_collapse)
    {
        // Update collapse count for this message
        m_collapsedMsgsCount[msg] += 1;
        String newCountStr = String::ToString( m_collapsedMsgsCount[msg] );
        item->setText(ListLogger::c_countColumn, newCountStr.ToQString());
    }

    if (m_autoScroll)
    {
        scrollToBottom();
    }

    EditorWindow *win = EditorWindow::GetInstance();
    win->buttonShowLogMessages->setText( String::ToString(m_totalLogMessages).ToQString() );
    win->buttonShowWarnMessages->setText( String::ToString(m_totalWarnMessages).ToQString() );
    win->buttonShowErrorMessages->setText( String::ToString(m_totalErrorMessages).ToQString() );
    return messageId;
}

ListLogger *ListLogger::GetInstance()
{
    return EditorWindow::GetInstance() ?
           EditorWindow::GetInstance()->listLogger : nullptr;
}

void ListLogger::dropEvent(QDropEvent *e)
{
    e->ignore();
}

void ListLogger::OnClear()
{
    clear();
    m_totalLogMessages   = 0;
    m_totalWarnMessages  = 0;
    m_totalErrorMessages = 0;

    Map<MessageId, Message> persistentMessages;
    for (const auto &pair : m_currentMessages)
    {
        const Message &msg = pair.second;
        if (msg.persistent)
        {
            int messageId = pair.first;
            persistentMessages.Set(messageId, msg);
        }
    }

    m_messageToItem.Clear();
    m_currentMessages.Clear();
    m_collapsedMsgsCount.Clear();
    m_currentMessagesIds.Clear();

    // Restore persistent messages
    for (const auto &pair : persistentMessages)
    {
        AddMessage(pair.second, pair.first);
    }
}

void ListLogger::OnEditorPlay()
{
    if (m_clearOnPlay)
    {
        OnClear();
    }
}

void ListLogger::ClearMessage(ListLogger::MessageId id)
{
    if (m_currentMessages.ContainsKey(id))
    {
        const Message &msg = m_currentMessages[id];

        QTreeWidgetItem *item = m_messageToItem[msg];
        if (item) { delete item; }

        m_currentMessagesIds.Remove(msg);
        m_messageToItem.Remove(msg);
        m_currentMessages.Remove(id);
        m_collapsedMsgsCount.Remove(msg);
    }
}

void ListLogger::OnCollapseChanged(bool collapse)
{
    m_collapse = collapse;
    setColumnHidden(ListLogger::c_countColumn, !m_collapse);
    RefreshList();
}

void ListLogger::OnClearOnPlayChanged(bool clearOnPlay)
{
    m_clearOnPlay = clearOnPlay;
}

void ListLogger::OnAutoScrollChanged(bool autoScroll)
{
    m_autoScroll = autoScroll;
    if (m_autoScroll) { scrollToBottom(); }
}

void ListLogger::OnShowLogMessagesChanged(bool showLogMessages)
{
    m_showLogMessages = showLogMessages;
    RefreshList();
}

void ListLogger::OnShowWarnMessagesChanged(bool showWarnMessages)
{
    m_showWarnMessages = showWarnMessages;
    RefreshList();
}

void ListLogger::OnShowErrorMessagesChanged(bool showErrorMessages)
{
    m_showErrorMessages = showErrorMessages;
    RefreshList();
}

bool ListLogger::Message::operator==(const ListLogger::Message &rhs) const
{

    return fileName == rhs.fileName &&
           line == rhs.line &&
           msg == rhs.msg &&
           type == rhs.type;
}

bool ListLogger::Message::operator<(const ListLogger::Message &rhs) const
{
    if (line < rhs.line) return true; else if (line > rhs.line) return false;
    else
    {
        int fileNameComp = fileName.compare(rhs.fileName);
        if (fileNameComp < 0) return true; else if (fileNameComp > 0) return false;
        else
        {
            int msgComp = msg.compare(rhs.msg);
            if (msgComp < 0) return true; else if (msgComp > 0) return false;
            else
            {
                return int(type) < int(rhs.type);
            }
        }
    }
}
