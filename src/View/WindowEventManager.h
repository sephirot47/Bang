#ifndef WINDOWEVENTMANAGER_H
#define WINDOWEVENTMANAGER_H

#include <GL/glew.h>

#include <vector>
#include <string>
#include <QGLWidget>
#include <chrono>
#include <QTimer>
#include <QApplication>

#include "IWindowEventManagerListener.h"

#define PROPAGATE_EVENT_PAR(FUNCTION, ITERABLE) do {\
    for(auto it = (ITERABLE).begin(); it != (ITERABLE).end(); ++it ) \
    {\
        (*it)->FUNCTION; \
    }\
} while(0)

class Entity;
class WindowEventManager : public QGLWidget
{
    Q_OBJECT
private:
    static WindowEventManager *wem;
    std::vector<IWindowEventManagerListener*> listeners;

public:
    explicit WindowEventManager(QWidget *parent = 0);

    void AddListener(IWindowEventManagerListener *listener);

    static WindowEventManager *GetInstance();


public slots:

    static void NotifyChildAdded(Entity *child);

    static void NotifyMenuBarItemClicked(int itemClcked);
    void _NotifyMenuBarItemClicked(int itemClicked);
};

#endif // WINDOWEVENTMANAGER_H
