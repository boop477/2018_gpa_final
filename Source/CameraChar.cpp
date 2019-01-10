//
//  CameraChar.cpp
//  103033241_AS4
//
//  Created by Softcat on 1/10/19.
//  Copyright © 2019 NTHU. All rights reserved.
//

#include "CameraChar.hpp"

CameraChar::CameraChar(glm::vec3 position, glm::vec3 char_position, glm::vec3 eye_front){
    _position = position;
    _position_base = position;
    moveWithChar(char_position, eye_front);
    view = glm::lookAt(eye_pos, eye_pos + eye_front, eye_up);
}
// Call this function when resizing the window
void CameraChar::reshape(int width, int height) {
    glViewport(0, 0, width, height);
    float viewportAspect = (float)width / (float)height;
    projection = glm::perspective(glm::radians(60.0f), viewportAspect, 0.1f, 1000.0f);
}
// Update eye_pos and eye_front, you should invoke this function when the character is moving or rotating.
void CameraChar::moveWithChar(glm::vec3 char_position, glm::vec3 eye_front){
    eye_pos = char_position + _position; // remember to rotate _position.
    this->eye_front = eye_front;
}
void CameraChar::rotateEyePos(glm::vec3 char_position,float yaw){
    // Rotate _position relative to _position_base
    yaw = -yaw;
    glm::vec3 new_position;
    new_position.x = cos(glm::radians(yaw))*(_position_base.x) +
                     sin(glm::radians(yaw))*(_position_base.z);
    new_position.y = _position_base.y;
    new_position.z = -sin(glm::radians(yaw))*(_position_base.x) +
                     cos(glm::radians(yaw))*(_position_base.z);
    _position = new_position;
    eye_pos = char_position + _position;
}
void CameraChar::moveEyePos(glm::vec3 char_position){
    eye_pos = char_position + _position;
}
void CameraChar::rotateEyeFront(float yaw, float pitch){
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    eye_front = glm::normalize(front);
}
glm::mat4 CameraChar::getView() {
    view = glm::lookAt(eye_pos, eye_pos + eye_front, eye_up);
    //view = lookAt(glm::vec3(-2.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    return view;
}
glm::mat4 CameraChar::getProjection() {
    return projection;
}
