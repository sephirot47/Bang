#ifndef WINDOWMAIN_H
#define WINDOWMAIN_H

#include "Bang.h"

#include "Logger.h"
#include "WindowMainAutoGenerated.h"

class QApplication;
class WindowMain : public Ui_WindowMain
{

private:
    static WindowMain *win;

    static QMainWindow *mainWindow;
    static QApplication *app;

public:
    WindowMain();

    static void Init(QMainWindow *window, QApplication *app);
    static WindowMain *GetInstance();
    static QMainWindow *GetMainWindow();
    QApplication *GetApplication() const;
};

#endif // WINDOWMAIN_H
