#pragma once

#ifndef camera_h
#define camera_h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Engine
{

    class Camera
    {

    public:
        enum class CameraMode
        {
            Perspective,
            Orthographic
        };
        Camera(CameraMode Mode, unsigned int *WindowWidth, unsigned int *WindowHeight);

        void SetPerspective(float FOV, float NearPlane, float FarPlane);
        void SetOrthographic(float Size, float NearPlane, float FarPlane);
        void SetPosition(const glm::vec3 &Pos);
        void SetRotation(const glm::quat &Rot);

        glm::vec3 GetPosition() const;
        glm::quat GetRotation() const;
        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const;

    private:
        CameraMode Mode;
        float FOV;
        float NearPlane;
        float FarPlane;
        float OrthoSize;

        glm::vec3 Position;
        glm::quat Rotation;

        unsigned int *WindowWidth;
        unsigned int *WindowHeight;
    };
};

#endif