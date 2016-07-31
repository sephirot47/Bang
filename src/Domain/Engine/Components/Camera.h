#ifndef CAMERA_H
#define CAMERA_H

#include "Bang.h"

#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"

#include "Rect.h"
#include "Component.h"
#include "Transform.h"

class Canvas;
class Camera : public Component
{
public:
    /*
    enum ProjectionMode
    {
        Orthographic,
        Perspective
    };
    */

    NAMED_ENUM (ProjectionMode,
        Orthographic,
        Perspective
    );

private:

    Rect m_orthoRect;

    float m_fovDegrees = 60.0f;
    float m_zNear = 0.1f;
    float m_zFar = 100.0f;

    mutable float m_aspectRatio = 1.0f;

    ProjectionMode m_projMode = ProjectionMode::Perspective;

    bool m_autoUpdateAspectRatio = true;

public:
    Camera();

    void GetViewMatrix(Matrix4 *view) const;
    void GetProjectionMatrix(Matrix4 *proj) const;


    void SetOrthoRect(const Rect& rect);

    void SetFovDegrees(float m_fovDegrees);
    void SetZNear(float m_zNear);
    void SetZFar(float m_zFar);

    void SetProjectionMode(ProjectionMode m_projMode);

    void SetAspectRatio(float m_aspectRatio);

    void SetAutoUpdateAspectRatio(bool m_autoUpdateAspectRatio);


    float GetFovDegrees() const;
    float GetAspectRatio() const;
    float GetZNear() const;
    float GetZFar() const;
    ProjectionMode GetProjectionMode() const;
    bool  GetAutoUpdateAspectRatio() const;

    /**
     * @brief Converts a point from world coods to Screen NDC.
     *        NDC: Normalized Device Coordinates [(-1,-1) -> (1,1)]
    **/
    glm::vec2 WorldToScreenNDCPoint(const Vector3 &position);

    virtual const std::string ToString() const override;
    virtual std::string GetName() const override { return "Camera"; }

    virtual void CloneInto(ICloneable *clone) const override;
    virtual ICloneable *Clone() const override;

    #ifdef BANG_EDITOR
    virtual void OnInspectorXMLNeeded(XMLNode *xmlInfo) const override;
    virtual void OnInspectorXMLChanged(const XMLNode *xmlInfo) override;
    #endif

    virtual void ReadXMLInfo(const XMLNode *xmlInfo) override;
    virtual void FillXMLInfo(XMLNode *xmlInfo) const override;
};

#endif // CAMERA_H
