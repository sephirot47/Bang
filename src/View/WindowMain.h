#ifndef WINDOWMAIN_H
#define WINDOWMAIN_H

#include "Bang.h"

#include "Logger.h"
#include "WindowMainAutoGenerated.h"

class WindowMain : public Ui_WindowMain
{

private:
    static WindowMain *win;
    static QMainWindow *mainWindow;

public:
    WindowMain();

    static void Init(QMainWindow *window);
    static WindowMain *GetInstance();
    static QMainWindow *GetMainWindow();
};

#endif // WINDOWMAIN_H
