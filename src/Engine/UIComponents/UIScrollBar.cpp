#include "Bang/UIScrollBar.h"

#include "Bang/Rect.h"
#include "Bang/UIScrollArea.h"
#include "Bang/UIFocusTaker.h"
#include "Bang/RectTransform.h"
#include "Bang/UIFrameLayout.h"
#include "Bang/UITintedButton.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UIImageRenderer.h"

USING_NAMESPACE_BANG

UIScrollBar::UIScrollBar()
{
}

UIScrollBar::~UIScrollBar()
{
}

void UIScrollBar::OnUpdate()
{
    Component::OnUpdate();

    bool isBeingGrabbed = GetButton()->IsBeingPressed();
    if (isBeingGrabbed)
    {
        Vector2i mouseCoords = Input::GetMouseCoords();
        Recti scrollRectPx  = GetScrollingRect();
        Recti barRectPx = GetBar()->GetComponent<RectTransform>()->
                          GetScreenSpaceRectPx();
        if (!m_wasGrabbed)
        {
            m_grabOffset = (mouseCoords - barRectPx.GetMin());
        }

        Vector2i offsettedMouseCoords = mouseCoords - m_grabOffset;
        Vector2 mousePercent =
                Vector2(offsettedMouseCoords - scrollRectPx.GetMin()) /
                Vector2(scrollRectPx.GetSize() - barRectPx.GetSize());
        float scrollPercent = IsVertical() ? mousePercent.y : mousePercent.x;
        scrollPercent = Math::Clamp(scrollPercent, 0.0f, 1.0f);

        ScrollTo(scrollPercent);
    }
    m_wasGrabbed = isBeingGrabbed;
}

void UIScrollBar::ScrollTo(float percent)
{
    Recti rectPx = GetScrollingRect();
    int containerLength = (IsVertical() ? rectPx.GetHeight() : rectPx.GetWidth())
                          - GetLength();

    int scrollingPx = Math::Round<int>(percent * (containerLength + 1));

    scrollingPx = -scrollingPx;
    Vector2i scrolling = IsVertical() ? Vector2i(0, scrollingPx) :
                                        Vector2i(scrollingPx, 0);
    GetScrollArea()->SetScrolling(scrolling);
}

void UIScrollBar::SetLength(int lengthPx)
{
    UILayoutElement *barLE = GetBar()->GetComponent<UILayoutElement>();
    if (IsVertical())
    { barLE->SetMinHeight(lengthPx); barLE->SetPreferredHeight(lengthPx); }
    else
    { barLE->SetMinWidth (lengthPx); barLE->SetPreferredWidth(lengthPx); }
}

void UIScrollBar::SetThickness(int thickPx)
{
    UILayoutElement *barLE = GetBar()->GetComponent<UILayoutElement>();
    if (IsVertical())
    { barLE->SetMinWidth(thickPx);  barLE->SetPreferredWidth(thickPx); }
    else
    { barLE->SetMinHeight(thickPx); barLE->SetPreferredHeight(thickPx); }
}

void UIScrollBar::SetVertical(bool vertical)
{
    UILayoutElement *le = gameObject->GetComponent<UILayoutElement>();
    le->SetFlexibleHeight(vertical ? 1 : 0);
    le->SetFlexibleWidth (vertical ? 0 : 1);
}

int UIScrollBar::GetLength() const
{
    UILayoutElement *barLE = GetBar()->GetComponent<UILayoutElement>();
    return IsVertical() ? barLE->GetMinHeight() : barLE->GetMinWidth();
}

int UIScrollBar::GetThickness() const
{
    UILayoutElement *barLE = GetBar()->GetComponent<UILayoutElement>();
    return IsVertical() ? barLE->GetMinWidth() : barLE->GetMinHeight();
}

bool UIScrollBar::IsVertical() const
{
    UILayoutElement *le = gameObject->GetComponent<UILayoutElement>();
    return le->GetFlexibleHeight() == 1;
}

UIScrollBar *UIScrollBar::CreateInto(GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);
    REQUIRE_COMPONENT(go, UIFocusTaker);

    UIScrollBar *scrollBar = go->AddComponent<UIScrollBar>();
    UILayoutElement *le = go->AddComponent<UILayoutElement>();
    le->SetFlexibleSize( Vector2::Zero );

    UIImageRenderer *bg = go->AddComponent<UIImageRenderer>();
    bg->SetTint(Color::LightGray);

    UIScrollArea *scrollArea = GameObjectFactory::CreateUIScrollAreaInto(go);
    GameObject *container = scrollArea->GetContainer();
    UIFrameLayout *contFL = container->AddComponent<UIFrameLayout>();
    contFL->SetChildrenVerticalStretch(Stretch::None);
    contFL->SetChildrenHorizontalStretch(Stretch::None);
    contFL->SetChildrenHorizontalAlignment(HorizontalAlignment::Left);
    contFL->SetChildrenVerticalAlignment(VerticalAlignment::Top);
    contFL->SetPaddings(0);

    GameObject *bar = GameObjectFactory::CreateUIGameObject();
    UILayoutElement *barLE = bar->AddComponent<UILayoutElement>();
    barLE->SetFlexibleSize( Vector2::Zero );

    UIImageRenderer *barImg = bar->AddComponent<UIImageRenderer>();
    barImg->SetTint(Color::Black);

    UITintedButton *btn = bar->AddComponent<UITintedButton>();
    btn->AddAgent(bar);
    btn->AddToTint(bar);
    btn->SetMode(UIButtonMode::UseRectTransform);
    btn->SetIdleTintColor( Color::Black );
    btn->SetOverTintColor( Color::Gray * 1.3f );
    btn->SetPressedTintColor( Color::Gray);

    scrollBar->p_bar = bar;
    scrollBar->p_button = btn;
    scrollBar->p_scrollArea = scrollArea;
    scrollBar->SetVertical(true);
    scrollBar->SetLength(50);
    scrollBar->SetThickness(10);

    container->AddChild(bar);

    return scrollBar;
}

Recti UIScrollBar::GetScrollingRect() const
{
    GameObject *cont = GetScrollArea()->gameObject;
    RectTransform *rt = cont->GetComponent<RectTransform>();
    return rt->GetScreenSpaceRectPx();
}

UIScrollArea *UIScrollBar::GetScrollArea() const { return p_scrollArea; }
GameObject *UIScrollBar::GetBar() const { return p_bar; }
UIButton *UIScrollBar::GetButton() const { return p_button; }
