#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
#define YAW             -90.0f
#define PITCH           0.0f
#define SPEED           2.5f
#define SENSITIVITY     0.1f
#define ZOOM            45.0f

/** @brief euler Angles */
struct _EulerAngles {  
    float Yaw;
    float Pitch;
};

/** 
 * @def _EulerAnglesRotate 
 * @brief
 * Calculate new front
 * 
 * @param _this {const _EulerAngles*}
 * @param front {glm::vec3}
 * */
#define _EulerAnglesRotate(_this, front) \
(front)->x = cos(glm::radians((_this)->Yaw)) * cos(glm::radians((_this)->Pitch)); \
(front)->y = sin(glm::radians((_this)->Pitch)); \
(front)->z = sin(glm::radians((_this)->Yaw)) * cos(glm::radians((_this)->Pitch));
    

/** Camera, but without methods. */
struct _Camera {
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    /** @brief euler Angles */
    _EulerAngles EulerAngles;

    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
};



// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera : public _Camera
{
public:
    // constructor with vectors
    inline Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
    {
        Front = (glm::vec3(0.0f, 0.0f, -1.0f)); 
        MovementSpeed = (SPEED);
        MouseSensitivity = (SENSITIVITY);
        Zoom = (ZOOM);

        Position = position;
        WorldUp = up;
        EulerAngles.Yaw = yaw;
        EulerAngles.Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    inline Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    {
        Front = (glm::vec3(0.0f, 0.0f, -1.0f));
        MovementSpeed = (SPEED);
        MouseSensitivity = (SENSITIVITY);
        Zoom = (ZOOM);

        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        EulerAngles.Yaw = yaw;
        EulerAngles.Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    inline void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    /** @brief 
     * processes input received from a mouse input system. 
     * Expects the offset value in both the x and y direction. 
     * */
    inline void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        /** Sensitivities */
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;

        /** Euler Angle initiation with offset */
        this->EulerAngles.Yaw   += xoffset;
        this->EulerAngles.Pitch += yoffset;

        /** 
         * Clamping on condition
         *  */
        if (constrainPitch)
        {
            if (this->EulerAngles.Pitch > 89.0f)
                this->EulerAngles.Pitch = 89.0f;
            if (this->EulerAngles.Pitch < -89.0f)
                this->EulerAngles.Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    inline void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    inline void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        _EulerAnglesRotate(&this->EulerAngles, &front);
        Front = glm::normalize(front);

        // also re-calculate the Right and Up vector

        // normalize the vectors, 
        // because their length gets closer to 0 
        // the more you look up or down which results in slower movement.
        this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  
        this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
    }
};
#endif
