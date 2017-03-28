#include "Bang/Vector3.h"

#include "Bang/Math.h"
#include "Bang/String.h"
#include "Bang/Vector2.h"
#include "Bang/Quaternion.h"

const Vector3 Vector3::Up      = Vector3( 0, 1,  0);
const Vector3 Vector3::Down    = Vector3( 0,-1,  0);
const Vector3 Vector3::Right   = Vector3( 1, 0,  0);
const Vector3 Vector3::Left    = Vector3(-1, 0,  0);
const Vector3 Vector3::Forward = Vector3( 0, 0, -1);
const Vector3 Vector3::Back    = Vector3( 0, 0,  1);
const Vector3 Vector3::Zero    = Vector3( 0, 0,  0);
const Vector3 Vector3::One     = Vector3( 1, 1,  1);
//*/

Vector3::Vector3() : glm::vec3(0.0f)
{
}

Vector3::Vector3(const glm::vec3 &v) : glm::vec3(v)
{
}

Vector3::Vector3(float a) : glm::vec3(a)
{
}

Vector3::Vector3(float x, float y, float z) : glm::vec3(x,y,z)
{
}

Vector3::Vector3(const Vector2 &v, float z) : Vector3(v.x, v.y, z)
{
}

float Vector3::Length() const
{
    return float( Math::Sqrt(LengthSquared()) );
}

float Vector3::LengthSquared() const
{
    return x*x + y*y + z*z;
}

void Vector3::Normalize()
{
    float l = Length();
    x /= l;
    y /= l;
    z /= l;
}

Vector3 Vector3::Normalized() const
{
    Vector3 v(*this);
    v.Normalize();
    return v;
}

Vector3 Vector3::ToDegrees() const
{
    return Vector3( Math::Rad2Deg(x), Math::Rad2Deg(y), Math::Rad2Deg(z));
}

Vector3 Vector3::ToRadians() const
{
    return Vector3( Math::Deg2Rad(x), Math::Deg2Rad(y), Math::Deg2Rad(z));
}

glm::vec3 Vector3::ToGlmVec3() const
{
    return glm::vec3(x,y,z);
}

String Vector3::ToString() const
{
    return "(" + String::ToString(x) + ", " +
                 String::ToString(y) + ", " +
                 String::ToString(z) + ")";
}

float Vector3::Distance(const Vector3 &p) const
{
    return Vector3::Distance(*this, p);
}

float Vector3::DistanceSquared(const Vector3 &p) const
{
    return Vector3::DistanceSquared(*this, p);
}

void Vector3::OrthoNormalize(Vector3 &v1, Vector3 &v2)
{
    //Not sure if this is working...
    v1.Normalize();
    v2.Normalize();
    Vector3 aux = Vector3::Cross(v1, v2).Normalized();
    v2 = Vector3::Cross(aux, v1).Normalized();
}

Vector3 Vector3::Lerp(const Vector3 &v1,
                      const Vector3 &v2,
                      float progression)
{
    return Vector3(glm::mix(glm::vec3(v1), glm::vec3(v2), progression));
}

Vector3 Vector3::Abs() const
{
    return Vector3(Math::Abs(x), Math::Abs(y), Math::Abs(z));
}

float* Vector3::Values() const
{
    return (float*) &x;
}

Vector3 Vector3::Abs(const Vector3 &v)
{
    return v.Abs();
}

Vector3 Vector3::Cross(const Vector3 &v1, const Vector3 &v2)
{
    return Vector3(glm::cross(glm::vec3(v1), glm::vec3(v2)));
}

float Vector3::Dot(const Vector3 &v1, const Vector3 &v2)
{
    return glm::dot(glm::vec3(v1), glm::vec3(v2));
}

float Vector3::Distance(const Vector3 &v1, const Vector3 &v2)
{
    return (v1 - v2).Length();
}

float Vector3::DistanceSquared(const Vector3 &v1, const Vector3 &v2)
{
    return (v1 - v2).LengthSquared();
}

Vector3 Vector3::Reflect(const Vector3 &incident, const Vector3 &normal)
{
    Vector3 n = normal.Normalized();
    return incident - 2 * ( Vector3::Dot(incident, n) ) * n;
}

Vector2 Vector3::xy() const
{
    return Vector2(x, y);
}






Vector3 operator*(Quaternion q, const Vector3 &rhs)
{
    return Vector3(q * glm::vec3(rhs.x, rhs.y, rhs.z));
}

Vector3 operator*(const Vector3 &lhs, Quaternion q)
{
    return Vector3(glm::vec3(lhs.x, lhs.y, lhs.z) * q);
}


Vector3 operator+(const Vector3 & v1, const Vector3 &v2)
{
    return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}


Vector3 operator*(const Vector3 &v1, const Vector3 &v2)
{
    return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}


Vector3 operator-(const Vector3 &v)
{
    return Vector3(-v.x, -v.y, -v.z);
}


Vector3 operator*(float a, const Vector3 &v)
{
    return Vector3(a * v.x, a * v.y, a * v.z);
}


Vector3 operator*(const Vector3 &v, float a)
{
    return a * v;
}


Vector3 operator/(float a, const Vector3 &v)
{
    return Vector3(a / v.x, a / v.y, a / v.z);
}


Vector3 operator/(const Vector3 &v, float a)
{
    return Vector3(v.x / a, v.y / a, v.z / a);
}


Vector3 operator/(const Vector3 &v1, const Vector3 &v2)
{
    return Vector3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}


Vector3 &operator+=(Vector3 &lhs, const Vector3 &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}


Vector3 &operator-=(Vector3 &lhs, const Vector3 &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    return lhs;
}

Vector3 &operator*=(Vector3 &lhs, const Vector3 &rhs)
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    lhs.z *= rhs.z;
    return lhs;
}


Vector3 &operator/=(Vector3 &lhs, const Vector3 &rhs)
{
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    lhs.z /= rhs.z;
    return lhs;
}


Vector3 operator+(float a, const Vector3 &v)
{
    return Vector3(a + v.x, a + v.y, a + v.z);
}


Vector3 operator+(const Vector3 &v, float a)
{
    return a + v;
}


Vector3 operator-(const Vector3 &v1, const Vector3 &v2)
{
    return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}


Vector3 operator-(const Vector3 &v, float a)
{
    return Vector3(v.x - a, v.y - a, v.z - a);
}


Vector3 operator-(float a, const Vector3 &v)
{
    return Vector3(a - v.x, a - v.y, a - v.z);
}


Vector3 &operator+=(Vector3 &lhs, float a)
{
    lhs.x += a;
    lhs.y += a;
    lhs.z += a;
    return lhs;
}


Vector3 &operator-=(Vector3 &lhs, float a)
{
    lhs.x -= a;
    lhs.y -= a;
    lhs.z -= a;
    return lhs;
}


Vector3 &operator*=(Vector3 &lhs, float a)
{
    lhs.x *= a;
    lhs.y *= a;
    lhs.z *= a;
    return lhs;
}


Vector3 &operator/=(Vector3 &lhs, float a)
{
    lhs.x /= a;
    lhs.y /= a;
    lhs.z /= a;
    return lhs;
}

