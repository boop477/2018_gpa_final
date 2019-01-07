#include "Camera.hpp"
//#include "../Include/Include.h"

Camera::Camera(){
	view = glm::lookAt(eye_pos, eye_pos + eye_front, eye_up);
}
Camera::~Camera(){
}
void Camera::trackballUpdate(int mouse_x, int mouse_y, int height, int width) {
	/*
	Input:
	  mouse_x: position x of the cursur.
	  mouse_y: position y of the cursur
	  height, width: (h, w) of the window.
	*/

	mouse_x -= width / 2;
	mouse_y -= height / 2;

	// Map from [-width/2, width/2] to [-180.0, 180.0]
	float yaw = float(mouse_x) / (width / 2)*180.0;
	// Map from [-height/2, height/2] to [-89.0, 89.0]
	float pitch = float(mouse_y) / (height / 2)*-89.0;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	eye_front = glm::normalize(front);
}
void Camera::reshape(int width, int height) {
    glViewport(0, 0, width, height);
	float viewportAspect = (float)width / (float)height;
	projection = glm::perspective(glm::radians(60.0f), viewportAspect, 0.1f, 1000.0f);
}
glm::mat4 Camera::getView() {
	view = glm::lookAt(eye_pos, eye_pos + eye_front, eye_up);
	return view;
}
glm::mat4 Camera::getProjection() {
	return projection;
}
void Camera::moveFront() {
	eye_pos += eye_front / glm::vec3(5.0);
}
void Camera::moveBack() {
	eye_pos -= eye_front / glm::vec3(5.0);
}
void Camera::moveLeft() {
	glm::vec3 vec = eye_front / glm::vec3(5.0);

	glm::mat3 rot = glm::mat3();
	rot[0] = glm::vec3(cos(glm::radians(90.0f)), 0.0, sin(glm::radians(90.0f)));
	rot[1] = glm::vec3(0.0, 1.0, 0.0);
	rot[2] = glm::vec3(-sin(glm::radians(90.0f)), 0.0, cos(glm::radians(90.0f)));

	glm::vec3 move = rot * (eye_front / glm::vec3(5.0));
	eye_pos -= move;
}
void Camera::moveRight() { 
	glm::vec3 vec = eye_front / glm::vec3(5.0);

	glm::mat3 rot = glm::mat3();
	rot[0] = glm::vec3(cos(glm::radians(90.0f)), 0.0, sin(glm::radians(90.0f)));
	rot[1] = glm::vec3(0.0, 1.0, 0.0);
	rot[2] = glm::vec3(-sin(glm::radians(90.0f)), 0.0, cos(glm::radians(90.0f)));

	glm::vec3 move = rot * (eye_front / glm::vec3(5.0));
	eye_pos += move;
}
void Camera::moveUp() { 
	eye_pos += glm::vec3(0.0, 0.1, 0.0); 
}
void Camera::moveDown() { 
	eye_pos -= glm::vec3(0.0, 0.1, 0.0); 
}
