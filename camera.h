#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW			= -90.0f;
const float PITCH		=   0.0f;
const float SPEED		=   2.5f;
const float SENSITIVITY =   0.1f;
const float ZOOM		=  45.0f;

class Camera {
	public:
	glm::vec3 position;
	glm::vec3 worldUp  = glm::vec3(0.0f, 1.0f, 0.0f);

	// Calculated vectors
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	// Euler angles
	float yaw   = YAW;
	float pitch = PITCH;

	// Camera options
	float movementSpeed    = SPEED;
	float mouseSensitivity = SENSITIVITY;
	float zoom             = ZOOM;

	Camera(glm::vec3 position = glm::vec3(0.0, 0.0, 3.0), glm::vec3 up = glm::vec3(0.0, 1.0, 0.0), float yaw = YAW, float pitch = PITCH) {
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;

		// Calculate vectors
		updateCameraVectors();
	}

	glm::mat4 getViewMatrix() {
		return glm::lookAt(position, position + front, up);
	}

	void processKeyboard(CameraMovement direction, float deltaTime) {
		float velocity = movementSpeed * deltaTime;
		if (direction == FORWARD) {
			position += front * velocity;
		}
		if (direction == BACKWARD) {
			position -= front * velocity;
		}
		if (direction == LEFT) {
			position -= right * velocity;
		}
		if (direction == RIGHT) {
			position += right * velocity;
		}
	}

	void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true) {
		// Scale back by sensitivity
		xOffset *= mouseSensitivity;
		yOffset *= mouseSensitivity;

		// Update yaw and pitch
		yaw   += xOffset;
		pitch += yOffset;

		// Clamp pitch so camera doesn't flip
		if (constrainPitch) {
			pitch = std::clamp(pitch, -89.0f, 89.0f);
		}
	
		// Recalculate vectors
		updateCameraVectors();
	}

	void processMouseScroll(float yOffset) {
		zoom -= yOffset;
		zoom = std::clamp(zoom, 1.0f, 45.0f);
	}
	
	private:
	void updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		this->front = glm::normalize(front);

		this->right = glm::normalize(glm::cross(front, worldUp));
		this->up = glm::normalize(glm::cross(right, front));
	}
};