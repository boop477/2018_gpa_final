#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#include <unistd.h>
#include "../Include/GLM/glm/glm.hpp"
#include "../Include/GLM/glm/gtc/matrix_transform.hpp"
#include "../Include/GLM/glm/gtc/type_ptr.hpp"
//#include "../Externals/Include/Include.h"

class Camera{
public:
	Camera();
	~Camera();
    void initialize(glm::vec3 eye_pos, glm::vec3 eye_front);
	void reshape(int, int);
	void trackballUpdate(int, int, int, int);
    void fix(bool fix){is_fixed = fix;}

	glm::mat4 getView();
	glm::mat4 getProjection();

	void moveFront();
	void moveBack();
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
    void key_update(unsigned char key);
    

	glm::vec3 eye_pos = glm::vec3(-6.462197, 1.718549, 0.039003); // demo
    //glm::vec3 eye_pos = glm::vec3(-6.462197, 4.918546, 0.039003); // for debug usage in list_empire.obj
	glm::vec3 eye_front = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 eye_up = glm::vec3(0.0f, 1.0f, 0.0f);
    
    bool is_activated = false;
    bool is_fixed = false;

	float yaw = 90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
	float pitch = 0.0f;

private:
	glm::mat4 view;
	glm::mat4 projection;
};

