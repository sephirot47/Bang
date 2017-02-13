#include "RectTransform.h"

#include "Math.h"
#include "Debug.h"
#include "Screen.h"
#include "XMLNode.h"
#include "Vector4.h"
#include "GameObject.h"

RectTransform::RectTransform()
{
}

RectTransform::~RectTransform()
{
}

void RectTransform::CloneInto(ICloneable *clone) const
{
    Transform::CloneInto(clone);
    RectTransform *rt = static_cast<RectTransform*>(clone);

    rt->SetMarginLeft ( GetMarginLeft()  );
    rt->SetMarginTop  ( GetMarginTop()   );
    rt->SetMarginRight( GetMarginRight() );
    rt->SetMarginBot  ( GetMarginBot()   );

    rt->SetAnchorMin( GetAnchorMin()  );
    rt->SetAnchorMax( GetAnchorMax()  );

    rt->SetPivotPosition( GetPivotPosition() );
}


ICloneable *RectTransform::Clone() const
{
    RectTransform *rt = new RectTransform();
    CloneInto(rt);
    return rt;
}

void RectTransform::SetMarginLeft(int marginLeft)
{
    m_marginLeft = marginLeft;
    OnChanged();
}

void RectTransform::SetMarginTop(int marginTop)
{
    m_marginTop = marginTop;
    OnChanged();
}

void RectTransform::SetMarginRight(int marginRight)
{
    m_marginRight = marginRight;
    OnChanged();
}

void RectTransform::SetMarginBot(int marginBot)
{
    m_marginBot = marginBot;
    OnChanged();
}

void RectTransform::SetPivotPosition(const Vector2 &pivotPosition)
{
    m_pivotPosition = pivotPosition;
    OnChanged();
}

void RectTransform::SetAnchorMin(const Vector2 &anchorMin)
{
    m_anchorMin = anchorMin;
    m_anchorMin.x = Math::Min(m_anchorMin.x, m_anchorMax.x);
    m_anchorMin.y = Math::Min(m_anchorMin.y, m_anchorMax.y);
    OnChanged();
}

void RectTransform::SetAnchorMax(const Vector2 &anchorMax)
{
    m_anchorMax = anchorMax;
    m_anchorMax.x = Math::Max(m_anchorMax.x, m_anchorMin.x);
    m_anchorMax.y = Math::Max(m_anchorMax.y, m_anchorMin.y);
    OnChanged();
}

int RectTransform::GetMarginLeft() const
{
    return m_marginLeft;
}

int RectTransform::GetMarginTop() const
{
    return m_marginTop;
}

int RectTransform::GetMarginRight() const
{
    return m_marginRight;
}

int RectTransform::GetMarginBot() const
{
    return m_marginBot;
}

Vector2 RectTransform::GetMarginLeftTop() const
{
    return Vector2(m_marginLeft, m_marginTop);
}

Vector2 RectTransform::GetMarginRightBot() const
{
    return Vector2(m_marginRight, m_marginBot);
}

Vector2 RectTransform::GetPivotPosition() const
{
    return m_pivotPosition;
}

Vector2 RectTransform::GetAnchorMin() const
{
    return m_anchorMin;
}

Vector2 RectTransform::GetAnchorMax() const
{
    return m_anchorMax;
}

void RectTransform::ReadXMLInfo(const XMLNode *xmlInfo)
{
    Transform::ReadXMLInfo(xmlInfo);

    SetMarginLeft ( xmlInfo->GetInt("MarginLeft")  );
    SetMarginTop  ( xmlInfo->GetInt("MarginTop")   );
    SetMarginRight( xmlInfo->GetInt("MarginRight") );
    SetMarginBot  ( xmlInfo->GetInt("MarginBot")   );

    SetPivotPosition( xmlInfo->GetVector2("PivotPosition") );
    SetAnchorMin    ( xmlInfo->GetVector2("AnchorMin")     );
    SetAnchorMax    ( xmlInfo->GetVector2("AnchorMax")     );
}

void RectTransform::FillXMLInfo(XMLNode *xmlInfo) const
{
    xmlInfo->SetInt("MarginLeft",  GetMarginLeft() );
    xmlInfo->SetInt("MarginTop",   GetMarginTop()  );
    xmlInfo->SetInt("MarginRight", GetMarginRight());
    xmlInfo->SetInt("MarginBot",   GetMarginBot()  );

    xmlInfo->SetVector2("PivotPosition",  GetPivotPosition());
    xmlInfo->SetVector2("AnchorMin",      GetAnchorMin()    );
    xmlInfo->SetVector2("AnchorMax",      GetAnchorMax()    );

    Transform::FillXMLInfo(xmlInfo);
    xmlInfo->SetTagName("RectTransform");
    xmlInfo->SetVector3("Position", Vector3::Zero, {XMLProperty::Hidden});
}

Rect RectTransform::GetParentScreenRect() const
{
    Rect parentScreenRect = Rect::ScreenRect;
    RectTransform *parentRectTransform = nullptr;
    if (gameObject->parent)
    {
        parentRectTransform = gameObject->parent->GetComponent<RectTransform>();
    }

    if (parentRectTransform)
    {
        parentScreenRect = parentRectTransform->GetScreenContainingRect();
    }

    return parentScreenRect;
}

Rect RectTransform::GetScreenContainingRect() const
{
    Matrix4 localToWorld;
    GetLocalToWorldMatrix(&localToWorld);
    return localToWorld * Rect::ScreenRect;
}

Rect RectTransform::GetContainingRectInParentSpace() const
{
    return GetLocalToParentMatrix() * Rect::ScreenRect;
}

void RectTransform::OnChanged()
{
    m_hasChanged = true;
    List<RectTransform*> rectTransforms =
            gameObject->GetComponentsInChildren<RectTransform>();
    for (RectTransform *rt : rectTransforms)
    {
        rt->OnParentSizeChanged();
    }
}

void RectTransform::OnParentSizeChanged()
{
    OnChanged();
}

const Matrix4 &RectTransform::GetLocalToParentMatrix() const
{
    if (m_hasChanged)
    {
        const Vector2 screenSize = Screen::GetSize();
        Vector2 parentSizeInPx = GetParentScreenRect().GetSize() * screenSize;
        parentSizeInPx.x = Math::Max(1.0f, parentSizeInPx.x);
        parentSizeInPx.y = Math::Max(1.0f, parentSizeInPx.y);

        float marginMultiplier = 4.0f; // Dont know why, havent thought it much
        Vector2 pixelSizeInParent = (1.0f / parentSizeInPx) * marginMultiplier;

        Vector2 minMarginedAnchor
                (m_anchorMin + GetMarginLeftTop()  * pixelSizeInParent);
        Vector2 maxMarginedAnchor
                (m_anchorMax - GetMarginRightBot() * pixelSizeInParent);
        Vector3 anchorScalingV
                ((maxMarginedAnchor - minMarginedAnchor) * 0.5f, 1);
        Matrix4 anchorScaling = Matrix4::ScaleMatrix(anchorScalingV);

        Vector3 moveToPivotV(-m_pivotPosition, 0);
        moveToPivotV.y *= -1.0f;
        Matrix4 moveToPivot = Matrix4::TranslateMatrix(moveToPivotV);

        Vector3 moveToAnchorCenterV(
                    (maxMarginedAnchor + minMarginedAnchor) * 0.5f, 0);
        moveToAnchorCenterV.y *= -1.0f;
        Matrix4 moveToAnchorCenter =
                Matrix4::TranslateMatrix(moveToAnchorCenterV);

        const Matrix4 &rotScaleTransform = Transform::GetLocalToParentMatrix();
        m_localToParentMatrix = rotScaleTransform *
                                moveToAnchorCenter *
                                anchorScaling *
                                moveToPivot;
        m_hasChanged = false;
    }
    return m_localToParentMatrix;
}
