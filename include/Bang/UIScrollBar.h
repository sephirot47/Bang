#ifndef UISCROLLBAR_H
#define UISCROLLBAR_H

#include "Bang/Alignment.h"
#include "Bang/Component.h"
#include "Bang/IFocusListener.h"

NAMESPACE_BANG_BEGIN

FORWARD class UIFocusable;
FORWARD class UIScrollArea;
FORWARD class UIImageRenderer;

class UIScrollBar : public Component,
                    public IFocusListener
{
    COMPONENT(UIScrollBar)

public:
	UIScrollBar();
	virtual ~UIScrollBar();

    void OnStart() override;
    void OnUpdate() override;

    void SetSide(Side side);
    void SetScrolling(int scrollingPx);
    void SetScrollingPercent(float percent);
    void SetLength(int lengthPx);
    void SetLengthPercent(float lengthPercent);
    void SetThickness(int thickPx);

    Side GetSide() const;
    int GetScrolling() const;
    float GetScrollingPercent() const;
    int GetLength() const;
    int GetThickness() const;
    Axis GetScrollAxis() const;

    bool IsBeingGrabbed() const;

private:
    int m_length = 0;
    int m_thickness = 0;
    int m_scrollingPx = 0;
    bool m_wasGrabbed = false;
    Vector2 m_grabOffsetPx = Vector2::Zero;

    Side m_side = Undef<Side>();

    GameObject *p_bar = nullptr;
    UIFocusable *p_button = nullptr;
    UIImageRenderer *p_barImg = nullptr;
    UIScrollArea *p_scrollArea = nullptr;

    static UIScrollBar* CreateInto(GameObject *go);

    void UpdateLengthThicknessMargins();

    int GetScrollingSpacePx() const;
    AARect GetScrollingRect() const;
    UIScrollArea *GetScrollArea() const;
    UIFocusable* GetFocusable() const;
    GameObject* GetBar() const;

    // IFocusListener
    void OnMouseEnter(IFocusable *focusable) override;
    void OnMouseExit(IFocusable *focusable) override;

    friend class GameObjectFactory;
};

NAMESPACE_BANG_END

#endif // UISCROLLBAR_H_H

