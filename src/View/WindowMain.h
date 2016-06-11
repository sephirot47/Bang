#ifndef WINDOWMAIN_H
#define WINDOWMAIN_H

#include "Bang.h"

#include "Logger.h"
#include "WindowMainAutoGenerated.h"

extern int var;

/**
 *  This will be a singleton shared amongst ALL *.o and *.so
 *  Cross-library shared singleton
 */
class QApplication;
class WindowMain : public Ui_WindowMain
{

private:
    WindowMain() : Ui_WindowMain() {}

    static WindowMain *win;
    static WindowMain **winPointer;

    QMainWindow *mainWindow = nullptr;
    QApplication *app = nullptr;

public:

    static void Init(QMainWindow *window, QApplication *app);

    /**
     * @brief This must be called by the behaviours linked to the
     * main binary in runtime.
     * @param singletonWindowMain
     */
    static void Init(WindowMain *singletonWindowMain);


    static WindowMain *GetInstance();
    static QMainWindow *GetMainWindow();
    QApplication *GetApplication() const;
};

#endif // WINDOWMAIN_H
