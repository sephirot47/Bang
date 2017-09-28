#ifndef DIALOGWINDOW_H
#define DIALOGWINDOW_H

#include "Bang/Window.h"

NAMESPACE_BANG_BEGIN

class DialogWindow : public Window
{
public:
    DialogWindow(Window *parentWindow);
    virtual ~DialogWindow();

    virtual void Create(uint flags) override;
};

NAMESPACE_BANG_END

#endif // DIALOGWINDOW_H