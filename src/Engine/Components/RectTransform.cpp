#include "Bang/RectTransform.h"

#include "Bang/GL.h"
#include "Bang/Rect.h"
#include "Bang/Debug.h"
#include "Bang/Input.h"
#include "Bang/AARect.h"
#include "Bang/Gizmos.h"
#include "Bang/Window.h"
#include "Bang/XMLNode.h"
#include "Bang/Matrix4.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/UILayoutManager.h"
#include "Bang/ITransformListener.h"

USING_NAMESPACE_BANG

RectTransform::RectTransform()
{
}

RectTransform::~RectTransform()
{
}

Vector2 RectTransform::
FromViewportPointToLocalPointNDC(const Vector2 &vpPoint) const
{
    return (GetLocalToWorldMatrixInv() * Vector4(vpPoint, 0, 1)).xy();
}
Vector2 RectTransform::
FromViewportPointToLocalPointNDC(const Vector2i &vpPoint) const
{
    return FromViewportPointToLocalPointNDC( Vector2(vpPoint) );
}

Vector2 RectTransform::FromViewportPointToLocalPoint(const Vector2 &vpPoint) const
{
    return FromLocalPointNDCToLocalPoint(
                FromViewportPointToLocalPointNDC(vpPoint) );
}

Vector2 RectTransform::FromViewportPointToLocalPoint(const Vector2i &vpPoint) const
{
    return FromViewportPointToLocalPoint( Vector2(vpPoint) );
}

Vector2 RectTransform::FromViewportAmountToLocalAmountNDC(const Vector2 &vpAmount) const
{
    Rect parentWinRect = GetParentViewportRect();
    Vector2 parentSizePxVp = Vector2::Max(Vector2::One, parentWinRect.GetSize());
    return GL::FromAmountToAmountNDC( Vector2(vpAmount) , parentSizePxVp );
}
Vector2 RectTransform::FromViewportAmountToLocalAmountNDC(const Vector2i &vpAmount) const
{
    return FromWindowAmountToLocalAmountNDC( Vector2(vpAmount) );
}

Vector2 RectTransform::FromWindowAmountToLocalAmountNDC(const Vector2 &winAmount) const
{
    Vector2 winViewportProportion =
       (Vector2(Window::GetActive()->GetSize()) /
        Vector2::Max(Vector2::One, Vector2(GL::GetViewportSize())) );
    Vector2 vpAmount = winAmount * winViewportProportion;
    return FromViewportAmountToLocalAmountNDC(vpAmount);
}
Vector2 RectTransform::
FromWindowAmountToLocalAmountNDC(const Vector2i &winAmount) const
{
    return FromWindowAmountToLocalAmountNDC( Vector2(winAmount) );
}

Vector2 RectTransform::
FromLocalAmountNDCToViewportAmount(const Vector2 &localAmountNDC) const
{
    Rect parentWinRect = GetParentViewportRect();
    Vector2 parentSizePx = Vector2::Max(Vector2::One, parentWinRect.GetSize());
    return GL::FromAmountNDCToAmount(localAmountNDC, parentSizePx);
}

Vector2 RectTransform::
FromLocalPointNDCToViewportPoint(const Vector2 &localPointNDC) const
{
    return GL::FromViewportPointNDCToViewportPoint(
                FromLocalPointNDCToViewportPointNDC(localPointNDC) );
}

Vector2 RectTransform::
FromViewportPointNDCToLocalPointNDC(const Vector2 &vpPointNDC) const
{
    return (GetLocalToWorldMatrixInv() *
            Vector4(GL::FromViewportPointNDCToViewportPoint(vpPointNDC), 0, 1)).xy();
}

AARect RectTransform::
FromViewportAARectNDCToLocalAARectNDC(const AARect &vpAARectNDC) const
{
    return AARect( FromViewportPointNDCToLocalPointNDC(vpAARectNDC.GetMin()),
                   FromViewportPointNDCToLocalPointNDC(vpAARectNDC.GetMax()));

}
AARect RectTransform::
FromLocalAARectNDCToViewportAARectNDC(const AARect &localAARectNDC) const
{
    return AARect( FromLocalPointNDCToViewportPointNDC(localAARectNDC.GetMin()),
                   FromLocalPointNDCToViewportPointNDC(localAARectNDC.GetMax()));
}
Rect RectTransform::
FromViewportRectNDCToLocalRectNDC(const Rect &vpRectNDC) const
{
    return GetLocalToWorldMatrixInv() * vpRectNDC;
}
Rect RectTransform::
FromLocalRectNDCToViewportRectNDC(const Rect &localRectNDC) const
{
    return GetLocalToWorldMatrix() * localRectNDC;
}

Vector2 RectTransform::
FromLocalPointNDCToViewportPointNDC(const Vector2 &localPointNDC) const
{
    return GL::FromViewportPointToViewportPointNDC(
                (GetLocalToWorldMatrix() * Vector4(localPointNDC, 0, 1) ).xy() );
}

Vector2 RectTransform::FromLocalPointToLocalPointNDC(const Vector2 &localPoint) const
{
    return GL::FromPointToPointNDC(localPoint, GetViewportAARect().GetSize());
}

Vector2 RectTransform::FromLocalPointNDCToLocalPoint(const Vector2 &localPointNDC) const
{
    return GL::FromPointNDCToPoint(localPointNDC, GetViewportAARect().GetSize());
}

void RectTransform::SetMarginLeft(int marginLeft)
{
    if (GetMarginLeft() != marginLeft)
    {
        m_marginLeftBot.x = marginLeft;
        InvalidateTransform();
    }
}

void RectTransform::SetMarginTop(int marginTop)
{
    if (GetMarginTop() != marginTop)
    {
        m_marginRightTop.y = marginTop;
        InvalidateTransform();
    }
}

void RectTransform::SetMarginRight(int marginRight)
{
    if (GetMarginRight() != marginRight)
    {
        m_marginRightTop.x = marginRight;
        InvalidateTransform();
    }
}

void RectTransform::SetMarginBot(int marginBot)
{
    if (GetMarginBot() != marginBot)
    {
        m_marginLeftBot.y = marginBot;
        InvalidateTransform();
    }
}

void RectTransform::AddMarginLeft(int marginLeft)
{
    SetMarginLeft(GetMarginLeft() + marginLeft);
}

void RectTransform::AddMarginTop(int marginTop)
{
    SetMarginTop(GetMarginTop() + marginTop);
}

void RectTransform::AddMarginRight(int marginRight)
{
    SetMarginRight(GetMarginRight() + marginRight);
}

void RectTransform::AddMarginBot(int marginBot)
{
    SetMarginBot(GetMarginBot() + marginBot);
}

void RectTransform::SetMargins(int marginAll)
{
    SetMargins(marginAll, marginAll, marginAll, marginAll);
}

void RectTransform::SetMarginMin(Axis axis, int marginMax)
{
    SetMargins(axis, GetMarginMin(axis), marginMax);
}
void RectTransform::SetMarginMax(Axis axis, int marginMin)
{
    SetMargins(axis, marginMin, GetMarginMax(axis));
}

void RectTransform::SetMarginLeftBot(const Vector2i &marginLeftBot)
{
    SetMarginLeft(marginLeftBot.x);
    SetMarginBot(marginLeftBot.y);
}

void RectTransform::SetMarginRightTop(const Vector2i &marginRightTop)
{
    SetMarginRight(marginRightTop.x);
    SetMarginTop(marginRightTop.y);
}

void RectTransform::SetMargins(const Vector2i &marginLeftBot,
                               const Vector2i &marginRightTop)
{
    SetMarginLeftBot(marginLeftBot);
    SetMarginRightTop(marginRightTop);
}

void RectTransform::SetMargins(Axis axis, const Vector2i &margins)
{
    SetMargins(axis, margins[0], margins[1]);
}

void RectTransform::SetMargins(Axis axis, int marginMin, int marginMax)
{
    if (axis == Axis::Vertical)
    { SetMarginBot(marginMin); SetMarginTop(marginMax); }

    if (axis == Axis::Horizontal)
    { SetMarginLeft(marginMin); SetMarginRight(marginMax); }
}

void RectTransform::SetMargins(int left, int top, int right, int bot)
{
    SetMarginBot(bot);
    SetMarginTop(top);
    SetMarginLeft(left);
    SetMarginRight(right);
}

void RectTransform::SetPivotPosition(const Vector2 &pivotPosition)
{
    if (m_pivotPosition != pivotPosition)
    {
        m_pivotPosition = pivotPosition;
        InvalidateTransform();
    }
}

void RectTransform::SetAnchorMin(const Vector2 &anchorMin)
{
    SetAnchors(anchorMin, GetAnchorMax());
}

void RectTransform::SetAnchorMax(const Vector2 &anchorMax)
{
    SetAnchors(GetAnchorMin(), anchorMax);
}

void RectTransform::SetAnchorMinX(float anchorMinX)
{
    SetAnchorMin( Vector2(anchorMinX, GetAnchorMin().y) );
}

void RectTransform::SetAnchorMinY(float anchorMinY)
{
    SetAnchorMin( Vector2(GetAnchorMin().x, anchorMinY) );
}

void RectTransform::SetAnchorMaxX(float anchorMaxX)
{
    SetAnchorMax( Vector2(anchorMaxX, GetAnchorMax().y) );
}

void RectTransform::SetAnchorMaxY(float anchorMaxY)
{
    SetAnchorMax( Vector2(GetAnchorMax().x, anchorMaxY) );
}

void RectTransform::SetAnchorX(const Vector2 &anchorX)
{
    SetAnchors(Vector2(anchorX[0], GetAnchorMin().y),
               Vector2(anchorX[1], GetAnchorMax().y) );
}

void RectTransform::SetAnchorY(const Vector2 &anchorY)
{
    SetAnchors(Vector2(GetAnchorMin().x, anchorY[0]),
               Vector2(GetAnchorMax().x, anchorY[1]) );
}

void RectTransform::SetAnchors(const Vector2 &anchorPoint)
{
    SetAnchors(anchorPoint, anchorPoint);
}

void RectTransform::SetAnchors(const Vector2 &anchorMin,
                               const Vector2 &anchorMax)
{
    bool changed = false;

    if (GetAnchorMin() != anchorMin)
    {
        m_anchorMin = anchorMin;
        changed = true;
    }

    if (GetAnchorMax() != anchorMax)
    {
        m_anchorMax = anchorMax;
        changed = true;
    }

    if (changed)
    {
        WarnWrongAnchorsIfNeeded();
        InvalidateTransform();
    }
}

void RectTransform::SetWidthFromPivot(int width)
{
    SetMarginLeft ( -(GetPivotPosition().x - (-1)) * width / 2 );
    SetMarginRight(  (GetPivotPosition().x - ( 1)) * width / 2 );
}

void RectTransform::SetHeightFromPivot(int height)
{
    SetMarginBot( -(GetPivotPosition().y - (-1)) * height / 2 );
    SetMarginTop(  (GetPivotPosition().y - ( 1)) * height / 2 );
}

int RectTransform::GetMarginLeft() const { return GetMarginLeftBot().x; }
int RectTransform::GetMarginTop() const { return GetMarginRightTop().y; }
int RectTransform::GetMarginRight() const { return GetMarginRightTop().x; }
int RectTransform::GetMarginBot() const { return GetMarginLeftBot().y; }
int RectTransform::GetMarginMin(Axis axis) const { return GetMargins(axis)[0]; }
int RectTransform::GetMarginMax(Axis axis) const { return GetMargins(axis)[1]; }
Vector2 RectTransform::GetMargins(Axis axis) const
{
    return axis == Axis::Horizontal ?
                Vector2(GetMarginLeft(), GetMarginRight()) :
                Vector2(GetMarginBot(),  GetMarginTop());

}

const Vector2i& RectTransform::GetMarginLeftBot() const { return m_marginLeftBot; }
const Vector2i& RectTransform::GetMarginRightTop() const { return m_marginRightTop; }
const Vector2& RectTransform::GetPivotPosition() const { return m_pivotPosition; }
const Vector2& RectTransform::GetAnchorMin() const { return m_anchorMin; }
const Vector2& RectTransform::GetAnchorMax() const { return m_anchorMax; }

Rect RectTransform::GetViewportRect() const
{
    return GetLocalToWorldMatrix() * Rect::NDCRect;
}
RectPoints RectTransform::GetViewportRectPointsNDC() const
{
    RectPoints vpRectPoints = GetViewportRect().GetPoints();
    for (int i = 0; i < 4; ++i)
    {
        vpRectPoints[i] = GL::FromViewportPointToViewportPointNDC(vpRectPoints[i]);
    }
    return vpRectPoints;
}

RectPoints RectTransform::GetParentViewportRectPointsNDC() const
{
    GameObject *parent = GetGameObject()->GetParent();
    if (!parent || !parent->GetRectTransform()) { return Rect::NDCRect.GetPoints(); }
    return parent->GetRectTransform()->GetViewportRectPointsNDC();
}

AARect RectTransform::GetViewportAARect() const
{
    return GetRectLocalToWorldMatrix() * AARect::NDCRect;
}

AARect RectTransform::GetViewportAARectNDC() const
{
    return GL::FromViewportRectToViewportRectNDC( GetViewportAARect() );
}

AARect RectTransform::GetParentViewportAARect() const
{
    GameObject *parent = GetGameObject()->GetParent();
    if (!parent || !parent->GetRectTransform())
    { return AARect(Vector2::Zero, Vector2( GL::GetViewportSize() )); }
    return parent->GetRectTransform()->GetViewportAARect();
}

AARect RectTransform::GetParentViewportAARectNDC() const
{
    GameObject *parent = GetGameObject()->GetParent();
    if (!parent || !parent->GetRectTransform()) { return AARect::NDCRect; }
    return parent->GetRectTransform()->GetViewportAARectNDC();
}

Rect RectTransform::GetParentViewportRect() const
{
    GameObject *parent = GetGameObject()->GetParent();
    if (!parent || !parent->GetRectTransform())
    { return AARect(GL::GetViewportRect()).ToRect(); }
    return parent->GetRectTransform()->GetViewportRect();
}

void RectTransform::CalculateLocalToParentMatrix() const
{
    AARect thisVPAARect = GetViewportAARect();
    Vector2 thisSize = Vector2::Max(thisVPAARect.GetSize(), Vector2::One);
    Vector2 pivotVp = -thisVPAARect.GetCenter() - GetPivotPosition() * thisSize * 0.5f;
    Matrix4 translateToPivot = Matrix4::TranslateMatrix( Vector3(pivotVp, 0) );
    m_localToParentMatrix = Matrix4::TranslateMatrix( GetLocalPosition() ) *
                            translateToPivot.Inversed() *
                            Matrix4::RotateMatrix( GetLocalRotation() ) *
                            translateToPivot *
                            Matrix4::ScaleMatrix( GetLocalScale() );
    m_localToParentMatrixInv = m_localToParentMatrix.Inversed();
}

void RectTransform::OnTransformInvalidated()
{
    Transform::OnTransformInvalidated();
    m_vpInWhichRectLocalToWorldWasCalc = AARecti::Zero;
    m_vpInWhichRectTransformLocalToWorldWasCalc = AARecti::Zero;
}

void RectTransform::InvalidateTransform()
{
    Transform::InvalidateTransform();
    m_vpInWhichRectLocalToWorldWasCalc = AARecti::Zero;
    m_vpInWhichRectTransformLocalToWorldWasCalc = AARecti::Zero;
}

void RectTransform::CalculateRectLocalToWorldMatrix() const
{
    const AARect parentAARect = GetParentViewportAARect();
    const Vector2 parentSize = parentAARect.GetSize();

    const Vector2 minVPAnchor = (GetAnchorMin() * 0.5f + 0.5f) * parentSize;
    const Vector2 maxVPAnchor = (GetAnchorMax() * 0.5f + 0.5f) * parentSize;
    const Vector2 minMarginedVPAnchor = minVPAnchor + Vector2(GetMarginLeftBot());
    const Vector2 maxMarginedVPAnchor = maxVPAnchor - Vector2(GetMarginRightTop());
    const Vector3 anchorScaling ((maxMarginedVPAnchor - minMarginedVPAnchor) * 0.5f, 1);
    const Vector2 moveToAnchorCenterOffset ( (maxMarginedVPAnchor + minMarginedVPAnchor) * 0.5f );
    const Vector3 moveToAnchorCenter (moveToAnchorCenterOffset + parentAARect.GetMin(), 0);

    const Matrix4 scaleMat = Matrix4::ScaleMatrix(anchorScaling);
    const Matrix4 translateToAnchorCenterMat = Matrix4::TranslateMatrix(moveToAnchorCenter);

    m_rectLocalToWorldMatrix = translateToAnchorCenterMat * scaleMat;
    m_rectLocalToWorldMatrixInv = m_rectLocalToWorldMatrix.Inversed();
    m_vpInWhichRectLocalToWorldWasCalc = GL::GetViewportRect();
}

void RectTransform::CalculateRectTransformLocalToWorldMatrix() const
{
    RecalculateParentMatricesIfNeeded();
    RecalculateWorldMatricesIfNeeded();
    const Matrix4& localToWorldMatrix = Transform::GetLocalToWorldMatrix();
    m_rectTransformLocalToWorldMatrix = localToWorldMatrix * GetRectLocalToWorldMatrix();
    m_rectTransformLocalToWorldMatrixInv = m_rectTransformLocalToWorldMatrix.Inversed();
    m_vpInWhichRectTransformLocalToWorldWasCalc = GL::GetViewportRect();
}

bool RectTransform::IsMouseOver(bool recursive) const
{
    if (!Input::IsMouseInsideWindow()) { return false; }

    if (IsActive() && GetGameObject()->IsActive() &&
        GetViewportRect().Contains( Vector2(Input::GetMousePosition()) ))
    {
        return true;
    }

    if (recursive)
    {
        List<RectTransform*> childrenRTs =
             GetGameObject()->GetComponentsInChildrenOnly<RectTransform>(true);
        for (RectTransform *childRT : childrenRTs)
        {
            if (childRT->IsMouseOver(false)) { return true; }
        }
    }
    return false;
}

const Matrix4 &RectTransform::GetLocalToWorldMatrix() const
{
    if (m_vpInWhichRectTransformLocalToWorldWasCalc != GL::GetViewportRect())
    {
        CalculateRectTransformLocalToWorldMatrix();
    }
    return m_rectTransformLocalToWorldMatrix;
}

const Matrix4 &RectTransform::GetLocalToWorldMatrixInv() const
{
    if (m_vpInWhichRectTransformLocalToWorldWasCalc != GL::GetViewportRect())
    {
        CalculateRectTransformLocalToWorldMatrix();
    }
    return m_rectTransformLocalToWorldMatrixInv;
}

const Matrix4 &RectTransform::GetRectLocalToWorldMatrix() const
{
    if (m_vpInWhichRectLocalToWorldWasCalc != GL::GetViewportRect())
    {
        CalculateRectLocalToWorldMatrix();
    }
    return m_rectLocalToWorldMatrix;
}

const Matrix4 &RectTransform::GetRectLocalToWorldMatrixInv() const
{
    if (m_vpInWhichRectLocalToWorldWasCalc != GL::GetViewportRect())
    {
        CalculateRectLocalToWorldMatrix();
    }
    return m_rectLocalToWorldMatrixInv;
}

void RectTransform::OnRender(RenderPass rp)
{
    Transform::OnRender(rp);
    /*
    if (rp != RenderPass::Overlay) { return; }

    Gizmos::Reset();
    Gizmos::SetThickness(1.0f);

    Rect r = GetViewportRectNDC(); (void)r;

    // Random::SetSeed(GetInstanceId());
    Gizmos::SetColor(Random::GetColorOpaque());
    // Gizmos::RenderFillRect(r);

    Gizmos::SetColor(Color::Green);
    Gizmos::RenderRect(AARect(r));
    Gizmos::SetColor(Color::Yellow);
    Gizmos::RenderScreenLine(r.GetMinXMaxY(), r.GetMaxXMinY());
    Gizmos::SetColor(Color::Yellow);
    Gizmos::RenderScreenLine(r.GetMinXMinY(), r.GetMaxXMaxY());
    float size = GL::FromViewportAmountToViewportAmountNDC(Vector2(2)).x;
    Gizmos::SetColor(Color::Red);
    Gizmos::RenderRect(Rect(r.GetCenter() - Vector2(size),
                            r.GetCenter() + Vector2(size)));

    Gizmos::SetColor(Color::Blue);
    Rect anchorRect = FromLocalNDCToViewportNDC(
                                Rect(GetAnchorMin(), GetAnchorMax()));
    Gizmos::RenderRect(anchorRect);
    */
}

void RectTransform::CloneInto(ICloneable *clone) const
{
    Transform::CloneInto(clone);
    RectTransform *rt = Cast<RectTransform*>(clone);

    rt->SetMargins( GetMarginLeftBot(), GetMarginRightTop() );
    rt->SetAnchors( GetAnchorMin(), GetAnchorMax() );
    rt->SetPivotPosition( GetPivotPosition() );
}

void RectTransform::ImportXML(const XMLNode &xmlInfo)
{
    Transform::ImportXML(xmlInfo);

    if (xmlInfo.Contains("MarginLeftBot"))
    {
        SetMargins (xmlInfo.Get<Vector2i>("MarginLeftBot"),
                    xmlInfo.Get<Vector2i>("MarginRightTop"));
    }

    if (xmlInfo.Contains("PivotPosition"))
    { SetPivotPosition( xmlInfo.Get<Vector2>("PivotPosition") ); }

    if (xmlInfo.Contains("AnchorMin"))
    { SetAnchorMin( xmlInfo.Get<Vector2>("AnchorMin") ); }

    if (xmlInfo.Contains("AnchorMax"))
    { SetAnchorMax( xmlInfo.Get<Vector2>("AnchorMax") ); }
}

void RectTransform::ExportXML(XMLNode *xmlInfo) const
{
    Transform::ExportXML(xmlInfo);

    xmlInfo->Set("MarginLeftBot",  GetMarginLeftBot() );
    xmlInfo->Set("MarginRightTop", GetMarginRightTop());

    xmlInfo->Set("PivotPosition",  GetPivotPosition());
    xmlInfo->Set("AnchorMin",      GetAnchorMin()    );
    xmlInfo->Set("AnchorMax",      GetAnchorMax()    );
}

void RectTransform::WarnWrongAnchorsIfNeeded()
{
    constexpr float Eps = 0.001f;
    if (GetAnchorMin().x > GetAnchorMax().x + Eps)
    {
        Debug_Warn("AnchorMin.x > AnchorMax.x!");
    }
    if (GetAnchorMin().y > GetAnchorMax().y + Eps)
    {
        Debug_Warn("AnchorMin.y > AnchorMax.y!");
    }
    if (GetAnchorMax().x < GetAnchorMin().x - Eps)
    {
        Debug_Warn("AnchorMax.x < AnchorMin.x!");
    }
    if (GetAnchorMax().y < GetAnchorMin().y - Eps)
    {
        Debug_Warn("AnchorMax.x < AnchorMin.y!");
    }
}

void RectTransform::OnEnabled()  { InvalidateTransform(); }
void RectTransform::OnDisabled() { InvalidateTransform(); }
