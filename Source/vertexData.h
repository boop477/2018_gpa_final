#ifndef vertexdata_h
#define vertexdata_h

/*#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#include <unistd.h>*/
#include "../Include/GLM/glm/glm.hpp"
#include "../Include/GLM/glm/gtc/matrix_transform.hpp"
#include "../Include/GLM/glm/gtc/type_ptr.hpp"

struct Vertex{
    glm::vec3 cord;
    glm::vec2 tex_cord;
    glm::vec3 normal;
};

#endif
