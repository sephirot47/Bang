#ifndef EDITORCAMERA_H
#define EDITORCAMERA_H

#include <cmath>

#include "Bang.h"

#include "Vector3.h"

#include "Input.h"
#include "Canvas.h"
#include "Camera.h"
#include "EditorGameObject.h"
#include "IWindowEventManagerListener.h"


/**
 * Pre-built GameObject, with a Camera that you can control in the editor
 * This camera has a parent "Pitch-Node", which rotates in y
 * and a child "Yaw-Node", which rotates in x.
 * The Yaw-Node contains the camera Component itself
**/
class EditorCamera : public EditorGameObject
{
private:
    Transform *t = nullptr;
    Camera *cam = nullptr;
    Transform *camt = nullptr; //YawNode transform
    GameObject *yawNode = nullptr;

    GameObject *currentFocus = nullptr;
    bool doingLookAt = false;
    float lookAtRotSpeed = 5.0f;
    float lookAtMoveSpeed = 4.0f;

    float maxMoveSpeed = 1.0f;
    float minMoveSpeed = 0.1f;
    float moveAccel = 0.1f;
    float moveSpeed = 0.0f;

    /**
     * @brief How many units in world space do we have to move,
     * for every panned pixel?
     */
    glm::vec2 mousePanPerPixel = glm::vec2(100.0f); //Movement with middle button

    /**
     * @brief startingRotation changes after every lookAt.
     * It indicates the initial offset to sum to the user's rotation
     * of mouse right click.
     */
    Quaternion startingRotation;                         //starting offset
    glm::vec2 mouseRotDegreesAccum = glm::vec2(0.0f);    //User input
    glm::vec2 mouseRotDegreesPerPixel = glm::vec2(0.0f); //Parameter

    /**
     * @brief How many units in world space do we have to move
     * in camera's forward direction, for every user wheel delta?
     */
    float mouseZoomPerDeltaWheel = 24.0f;

public:
    EditorCamera();
    virtual ~EditorCamera();

    /**
     * @brief Adjust different camera parameters based
     * on the current camera position, focus, etc. For example,
     * it adjusts mousePanPerPixel. mouseRotDegreesPerPixel, etc.
     */
    void AdjustSpeeds();

    /**
     * @brief Called after every change on the camera rotation which is not
     * from user's input. This is to update all internal variables related with
     * rotation handling.
     */
    void UpdateRotationVariables();

    /**
     * @brief Handles camera 'zoom' with mouse wheel
     * If it has moved, hasMoved = true.
     */
    void HandleWheelZoom(Vector3 *moveStep, bool *hasMoved);

    /**
     * @brief Handles camera rotation with right mouse click
     * If it has rotated, hasMoved = true.
     * If it has rotated, unlockMouse = false
     * @return Returns if the cam has rotated or not
     */
    bool HandleMouseRotation(bool *hasMoved, bool *unwrapMouse);

    /**
     * @brief Handles camera panning (moving in the plane
     * perpendicular to cam's forward) with middle mouse click
     * If it has moved, hasMoved = true.
     * If it has moved, unlockMouse = false
     */
    void HandleMousePanning(bool *hasMoved, bool *unwrapMouse);

    /**
     * @brief Handles key movement with WASD
     */
    void HandleKeyMovement(Vector3 *moveStep, bool *hasMoved);

    /**
     * @brief Handles LookAt behaviour when focusing a GameObject.
     */
    void HandleLookAtFocus();

    void OnUpdate() override;

    /**
     * Gets the Camera Component inside the "Yaw-Node".
     **/
    Camera *GetCamera();

#ifdef BANG_EDITOR
    void OnTreeHierarchyGameObjectsSelected
                (std::list<GameObject*> &selectedGameObjects) override;
    void OnTreeHierarchyGameObjectDoubleClicked(GameObject *selected) override;
#endif
};

#endif // EDITORCAMERA_H
