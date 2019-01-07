#ifndef mesh_hpp
#define mesh_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "vertexData.h"
#include "UniformList.h"

//#include <OpenGL/gl3.h>
//#include <GLUT/glut.h>
//#include <unistd.h>
#include "../Include/GLM/glm/glm.hpp"
#include "../Include/GLM/glm/gtc/matrix_transform.hpp"
#include "../Include/GLM/glm/gtc/type_ptr.hpp"


class Mesh {
public:
    /* std::vector<glm::vec3> cord;
     std::vector<glm::vec3> normal;
     std::vector<glm::vec2> tex_cord;
     std::vector<unsigned int> indices;
     std::vector<GLuint> tex_ids;*/
    Mesh(std::vector<Vertex> vertices,
         std::vector<unsigned int> indices,
		 GLuint tex_ids);
    /*
    void draw(GLuint program,
              glm::vec3 position,
              glm::quat quaternion,
              glm::vec3 scale,
              glm::mat4 view,
              glm::mat4 projection);*/
    void draw(UniformList,
              glm::mat4,
              glm::mat4,
              glm::mat4,
              bool);
    void draw(UniformList uniform_list,
              glm::mat4 light_vp_matrix,
              glm::vec3 position,
              glm::quat quaternion,
              glm::vec3 scale);
    void draw(UniformList uniform_list,
                glm::mat4 view_matrix,
                glm::mat4 proj_matrix,
                glm::mat4 light_vp_matrix,
                bool is_shadow,
                glm::vec3 position,
                glm::quat quaternion,
              glm::vec3 scale);
    void draw(UniformList uniform_list, // ssao
              glm::mat4 view_matrix,
              glm::mat4 proj_matrix,
              glm::vec3 position,
              glm::quat quaternion,
              glm::vec3 scale);

	void log();
	void logTexId();
private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    //std::vector<GLuint> tex_ids;
	GLuint tex_ids;
    
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    
    GLint _um4mv;
    GLint _um4p;
    // vertex
    // intices to draw vertex
    // texture
    void setMesh(); // Build vao , glVertexAttributePointer
    glm::mat4 getModelMatrix(glm::vec3 position, glm::quat quaternion, glm::vec3 scale);
};

#endif /* mesh_hpp */
