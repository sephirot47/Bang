#include "Bang/UIList.h"

#include "Bang/Rect.h"
#include "Bang/Input.h"
#include "Bang/GameObject.h"
#include "Bang/UIScrollBar.h"
#include "Bang/UIScrollArea.h"
#include "Bang/UIScrollPanel.h"
#include "Bang/RectTransform.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UILayoutIgnorer.h"
#include "Bang/UIVerticalLayout.h"
#include "Bang/GameObjectFactory.h"
#include "Bang/UIHorizontalLayout.h"
#include "Bang/UIContentSizeFitter.h"

USING_NAMESPACE_BANG

UIList::UIList()
{
}

UIList::~UIList()
{
}

void UIList::OnUpdate()
{
    RectTransform *rt = GetGameObject()->GetComponent<RectTransform>();
    if (rt->IsMouseOver())
    {
        // Selection In/Out
        int numItems = GetNumItems();
        if (Input::GetKeyDownRepeat(Key::Down))
        {
            SetSelection( (GetSelectedIndex() + 1) % numItems );
        }

        if (Input::GetKeyDownRepeat(Key::Up))
        {
            SetSelection( (GetSelectedIndex() - 1 + numItems) % numItems );
        }

        // Mouse In/Out
        GOItem *itemUnderMouse = nullptr;
        for (GOItem *childItem : p_items)
        {
            RectTransform *childRT = childItem->GetComponent<RectTransform>();
            if (childRT->IsEnabled(true) && childRT->IsMouseOver())
            { itemUnderMouse = childItem; break; }
        }

        if (p_itemUnderMouse != itemUnderMouse)
        {
            if (p_itemUnderMouse)
            {
                Callback(p_itemUnderMouse, Action::MouseOut);
            }

            p_itemUnderMouse = itemUnderMouse;
            if (p_itemUnderMouse)
            {
                Callback(p_itemUnderMouse, Action::MouseOver);
            }
        }

        // Clicked
        if (Input::GetKeyDownRepeat(Key::Right) ||
            Input::GetKeyDownRepeat(Key::Enter))
        {
            GOItem *selectedItem = GetSelectedItem();
            if (selectedItem) { Callback(selectedItem, Action::Pressed); }
        }

        if (p_itemUnderMouse)
        {
            if (Input::GetMouseButtonDown(MouseButton::Left))
            {
                SetSelection(p_itemUnderMouse);
                Callback(p_itemUnderMouse, Action::ClickedLeft);
            }

            if (Input::GetMouseButtonDown(MouseButton::Right))
            {
                Callback(p_itemUnderMouse, Action::ClickedRight);
            }

            if (Input::GetMouseButtonDoubleClick(MouseButton::Left))
            {
                Callback(p_itemUnderMouse, Action::DoubleClickedLeft);
            }
        }
    }
    else
    {
        if (p_itemUnderMouse) { Callback(p_itemUnderMouse, Action::MouseOut); }
        p_itemUnderMouse = nullptr;
    }
}

void UIList::AddItem(GOItem *newItem)
{
    bool hadSelectedGameObject = GetSelectedItem();

    p_items.PushBack(newItem);
    newItem->EventEmitter<IDestroyListener>::RegisterListener(this);
    newItem->SetParent(GetContainer());

    if (!hadSelectedGameObject) { SetSelection(0); }
}

void UIList::RemoveItem(GOItem *item)
{
    ASSERT( p_items.Contains(item) );
    GameObject::Destroy(item);

    p_items.Remove(item);

    if (p_itemUnderMouse == item) { p_itemUnderMouse = nullptr; }
    if (GetNumItems() > 0)
    {
        int selIndex = Math::Clamp(GetSelectedIndex()-1, 0, GetNumItems()-1);
        SetSelection(selIndex);
    }
    else { ClearSelection(); }
}

void UIList::ClearSelection()
{
    if (GetSelectedIndex() >= 0)
    {
        if (m_selectionCallback)
        {
            GOItem *selectedItem = GetSelectedItem();
            if (selectedItem)
            {
                m_selectionCallback(selectedItem, Action::SelectionOut);
            }
        }
        SetSelection(-1);
    }
}

void UIList::Clear()
{
    while (!p_items.IsEmpty()) { RemoveItem(p_items.Front()); }
    GetScrollPanel()->SetScrollingPercent( Vector2(0.0f) );
    ClearSelection();
}

int UIList::GetNumItems() const
{
    return p_items.Size();
}

void UIList::SetSelection(int index)
{
    if (GetSelectedIndex() != index)
    {
        GOItem *prevSelectedItem = GetSelectedItem();
        if (prevSelectedItem) { Callback(prevSelectedItem, Action::SelectionOut); }
    }

    m_selectionIndex = index;
    if (index >= 0 && index < GetNumItems())
    {
        GOItem *selectedItem = GetSelectedItem();
        if (selectedItem) { Callback(selectedItem, Action::SelectionIn); }
    }
}

void UIList::SetSelection(GOItem *item)
{
    SetSelection(p_items.IndexOf(item));
}

GameObject *UIList::GetContainer() const
{
    return p_container;
}

int UIList::GetSelectedIndex() const
{
    return m_selectionIndex;
}

GOItem *UIList::GetSelectedItem() const
{
    if (GetSelectedIndex() >= 0 && GetSelectedIndex() < p_items.Size())
    {
        auto it = p_items.Begin();
        std::advance(it, GetSelectedIndex());
        return *it;
    }
    return nullptr;
}

void UIList::OnDestroyed(Object *object)
{
    if (object == p_itemUnderMouse)
    {
        if (m_selectionCallback) { m_selectionCallback(p_itemUnderMouse,
                                                       Action::SelectionOut); }
        p_itemUnderMouse = nullptr;
    }
}

void UIList::SetSelectionCallback(SelectionCallback selectionCallback)
{
    m_selectionCallback = selectionCallback;
}

UIList* UIList::CreateInto(GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);

    UIScrollPanel *scrollPanel = GameObjectFactory::CreateUIScrollPanelInto(go);

    UILayoutElement *le = go->AddComponent<UILayoutElement>();
    le->SetMinHeight(10);

    GameObject *container = GameObjectFactory::CreateUIGameObject();

    UIDirLayout *dirLayout;
    const bool vertical = true;
    if (vertical) { dirLayout = container->AddComponent<UIVerticalLayout>(); }
    else { dirLayout = container->AddComponent<UIHorizontalLayout>(); }
    dirLayout->SetSpacing(0);
    dirLayout->SetPaddings(0);

    container->GetComponent<RectTransform>()->SetPivotPosition(Vector2(-1,1));
    UIContentSizeFitter *csf = container->AddComponent<UIContentSizeFitter>();
    csf->SetHorizontalSizeType(LayoutSizeType::Preferred);
    csf->SetVerticalSizeType(LayoutSizeType::Preferred);

    scrollPanel->GetScrollArea()->SetContainedGameObject(container);

    UIList *ld = go->AddComponent<UIList>();
    ld->p_scrollPanel = scrollPanel;
    ld->p_container = container;

    return ld;
}

void UIList::Callback(GOItem *item, Action action)
{
    if (m_selectionCallback) { m_selectionCallback(item, action); }
}

UIScrollPanel *UIList::GetScrollPanel() const { return p_scrollPanel; }
