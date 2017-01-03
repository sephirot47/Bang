#ifndef WINDOWMAIN_H
#define WINDOWMAIN_H

#include <QApplication>

#include "Screen.h"
#include "IWindow.h"
#include "ui_EditorWindow.h"

/**
 *  This will be a singleton shared amongst ALL *.o and *.so
 *  Cross-library shared singleton
 */
class QApplication;
class EditorWindow : public IWindow,
                     public Ui_EditorWindow
{

private:
    EditorWindow();

    static EditorWindow *s_m_win;

public:

    /**
     * @brief This must be called by main() in the mainBinary.
     * This method will register the EditorWindow singleton into the
     * SingletonManager, for later use in shared libraries.
     * @return The created Singleton,
     */
    static void InitFromMainBinary(QMainWindow *window, QApplication *app);

    static EditorWindow *GetInstance();
    QMainWindow *GetMainWindow() const override;
    QApplication *GetApplication() const override;

};

#endif // WINDOWMAIN_H
