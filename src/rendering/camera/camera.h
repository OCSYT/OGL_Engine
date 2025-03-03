#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

enum class CameraMode {
    Perspective,
    Orthographic
};

class Camera {
private:
    CameraMode Mode;
    float FOV;
    float NearPlane;
    float FarPlane;
    float OrthoSize;

    glm::vec3 Position;
    glm::quat Rotation;

public:
    float AspectRatio;
    Camera(CameraMode Mode, float AspectRatio);

    void SetPerspective(float FOV, float NearPlane, float FarPlane);
    void SetOrthographic(float Size, float NearPlane, float FarPlane);
    void SetPosition(const glm::vec3& Pos);
    void SetRotation(const glm::quat& Rot);

    glm::vec3 GetPosition() const;
    glm::quat GetRotation() const;
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
};
