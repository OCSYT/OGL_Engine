#include "Camera.h"

Camera::Camera(CameraMode Mode, float AspectRatio)
    : Mode(Mode), AspectRatio(AspectRatio), FOV(45.0f), NearPlane(0.1f), FarPlane(100.0f), OrthoSize(10.0f),
      Position(glm::vec3(0.0f)), Rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)) {}

void Camera::SetPerspective(float FOV, float NearPlane, float FarPlane) {
    this->Mode = CameraMode::Perspective;
    this->FOV = FOV;
    this->NearPlane = NearPlane;
    this->FarPlane = FarPlane;
}

void Camera::SetOrthographic(float Size, float NearPlane, float FarPlane) {
    this->Mode = CameraMode::Orthographic;
    this->OrthoSize = Size;
    this->NearPlane = NearPlane;
    this->FarPlane = FarPlane;
}

void Camera::SetPosition(const glm::vec3& Pos) {
    this->Position = Pos;
}

void Camera::SetRotation(const glm::quat& Rot) {
    this->Rotation = Rot;
}

glm::vec3 Camera::GetPosition() const {
    return Position;
}

glm::quat Camera::GetRotation() const {
    return Rotation;
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::translate(glm::mat4(1.0f), -Position) * glm::mat4_cast(Rotation);
}

glm::mat4 Camera::GetProjectionMatrix() const {
    if (Mode == CameraMode::Perspective) {
        return glm::perspective(glm::radians(FOV), AspectRatio, NearPlane, FarPlane);
    } else {
        float HalfSize = OrthoSize * 0.5f;
        return glm::ortho(-HalfSize * AspectRatio, HalfSize * AspectRatio, -HalfSize, HalfSize, NearPlane, FarPlane);
    }
}
