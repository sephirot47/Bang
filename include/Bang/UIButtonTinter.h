#ifndef UIBUTTONTINTER_H
#define UIBUTTONTINTER_H

#include "Bang/Color.h"
#include "Bang/UIButton.h"

NAMESPACE_BANG_BEGIN

class UIButtonTinter : public UIButton,
                       public UIButtonListener
{
    COMPONENT(UIButtonTinter)

public:
    UIButtonTinter();
    virtual ~UIButtonTinter();

    // Component
    virtual void OnUpdate() override;

    void AddGameObjectToTint(GameObject *go);

    void SetOverTintColor(const Color &tintColor);
    void SetIdleTintColor(const Color &tintColor);
    void SetPressedTintColor(const Color &tintColor);

    const Color& GetIdleTintColor() const;
    const Color& GetOverTintColor() const;
    const Color& GetPressedTintColor() const;

protected:
    virtual void OnButton_MouseEnter(UIButton *btn) override;
    virtual void OnButton_MouseExit(UIButton *btn) override;
    virtual void
    OnButton_MouseDown(UIButton *btn, Input::MouseButton mb) override;
    virtual void
    OnButton_MouseUp(UIButton *btn, Input::MouseButton mb) override;

private:
    Set<GameObject*> p_gameObjectsToTint;
    Color m_idleTintColor    = Color::White;
    Color m_overTintColor    = Color::LightBlue;
    Color m_pressedTintColor = Color::DarkBlue;

    Color m_currentTintColor = m_idleTintColor;

    void ApplyTintToGameObjects(const Color &tintColor);
};

NAMESPACE_BANG_END

#endif // UIBUTTONTINTER_H
