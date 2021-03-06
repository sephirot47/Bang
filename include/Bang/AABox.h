#ifndef BOX_H
#define BOX_H

#include <cmath>

#include "Bang/Quad.h"
#include "Bang/AARect.h"
#include "Bang/Vector3.h"
#include "Bang/Matrix4.h"
#include "Bang/IToString.h"

NAMESPACE_BANG_BEGIN

FORWARD class Camera;

class AABox
{
private:
    Vector3 m_minv, m_maxv;
    bool m_initialized = false;

public:
    static AABox Empty;

    AABox();
    AABox(float minx, float maxx,
          float miny, float maxy,
          float minz, float maxz);
    AABox(const Vector3 &p);
    AABox(const Vector3 &p1, const Vector3 &p2);
    AABox(const AABox& b);

    void SetMin(const Vector3& bMin);
    void SetMax(const Vector3& bMax);

    const Vector3& GetMin() const;
    const Vector3& GetMax() const;
    Vector3 GetDiagonal() const;
    float GetWidth() const;
    float GetHeight() const;
    float GetDepth() const;
    Vector3 GetCenter() const;
    Vector3 GetSize() const;
    float GetArea() const;
    float GetVolume() const;
    Vector3 GetExtents() const;
    AARect GetAABoundingViewportRect(Camera *cam) const;
    Vector3 GetClosestPointInAABB(const Vector3 &point) const;
    Array<Vector3> GetPoints() const;

    Quad GetQuad(Axis3D axis, bool sign) const;
    Quad GetRightQuad() const;
    Quad GetLeftQuad()  const;
    Quad GetTopQuad()   const;
    Quad GetBotQuad()   const;
    Quad GetFrontQuad() const;
    Quad GetBackQuad()  const;
    std::array<Quad, 6> GetQuads() const;

    bool CheckCollision(const Sphere &sphere,
                        Vector3 *point  = nullptr,
                        Vector3 *normal = nullptr) const;
    bool CheckCollision(const AABox &aabox) const;
    bool Contains(const Vector3 &point) const;

    void AddPoint(const Vector3& point);
    static AABox Union(const AABox &b1, const AABox &b2);
    void CreateFromPositions(const Array<Vector3> &positions);

    static AABox FromPointAndSize(const Vector3 &point, const Vector3 &size);
    static AABox FromSphere(const Sphere &sphere);
};

AABox operator*(const Matrix4 &m, const AABox &b);
bool operator==(const AABox &b1, const AABox &b2);
bool operator!=(const AABox &b1, const AABox &b2);

NAMESPACE_BANG_END

#endif // BOX_H
