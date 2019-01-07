#include "mesh.hpp"

#include "../Include/GLM/glm/glm.hpp"
#include "../Include/GLM/glm/gtx/transform.hpp"
#include "../Include/GLM/glm/gtx/quaternion.hpp"

#include "UniformList.h"

Mesh::Mesh(std::vector<Vertex> vertices,
           std::vector<unsigned int> indices,
           GLuint tex_ids){
    this->vertices = vertices;
    this->indices = indices;
    this->tex_ids = tex_ids;
	setMesh();
}
void Mesh::draw(UniformList uniform_list,
                glm::mat4 light_vp_matrix,
                glm::vec3 position,
                glm::quat quaternion,
                glm::vec3 scale){
    /*
     * Should invoke glUseProgram before calling this function.
     * Invoke this function to get a depth map.
     */

    glBindVertexArray(VAO);
    glm::mat4 model = getModelMatrix(position, quaternion, scale);   // Assign uniforms
    glUniformMatrix4fv(uniform_list.depth.mvp, 1, GL_FALSE, value_ptr(light_vp_matrix * model));
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void Mesh::draw(UniformList uniform_list,
                glm::mat4 view_matrix,
                glm::mat4 proj_matrix,
                glm::mat4 light_vp_matrix,
                bool is_shadow,
                glm::vec3 position,
                glm::quat quaternion,
                glm::vec3 scale){
    /*
     * Should invoke glUseProgram before calling this function.
     * Use this function to draw a model with environment mapping, shadow and BF lighting.
     */

    glBindVertexArray(VAO);  //Bind vao
    
    glm::mat4 model = getModelMatrix(position, quaternion, scale);   // Assign uniforms
    glm::mat4 light_mvp_matrix = light_vp_matrix * model;
    glUniformMatrix4fv(uniform_list.render.model_matrix, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(uniform_list.render.view_matrix, 1, GL_FALSE, &view_matrix[0][0]);
    glUniformMatrix4fv(uniform_list.render.proj_matrix, 1, GL_FALSE, &proj_matrix[0][0]);
    glUniformMatrix4fv(uniform_list.render.light_mvp_matrix, 1, GL_FALSE, &light_mvp_matrix[0][0]);
    //glUniform1i(uniform_list.render.is_quad, 0);
    if(is_shadow){
        glUniform1i(uniform_list.render.is_shadow, 1);
    }
    else{
        glUniform1i(uniform_list.render.is_shadow, 0);
    }
    
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    
    glUniform1i(uniform_list.render.is_quad, 0);
}
// Build vao , glVertexAttributePointer
void Mesh::setMesh(){
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(Vertex),
                 &vertices[0], GL_STATIC_DRAW);
    /*
     struct Vertex{
        glm::vec3 cord;
        glm::vec2 tex_cord;
        glm::vec3 normal;
    } */
    // coordinate
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    // texture coordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*3));
    // normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*5));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

glm::mat4 Mesh::getModelMatrix(glm::vec3 position, glm::quat quaternion, glm::vec3 scale){
    // Translation matrix
    glm::mat4 translation_matrix = glm::translate(glm::mat4(), position);
    // Rotation matrix
    glm::mat4 rotation_matrix = glm::toMat4(quaternion);
    // Scale matrix
    glm::mat4 scale_matrix = glm::scale(scale);
    
    return translation_matrix * rotation_matrix * scale_matrix;
}

void Mesh::log() {
	std::cout << "mesh.vertices.size: " << this->vertices.size() << std::endl;
	std::cout << "mesh.indices.size: " << this->indices.size() << std::endl;
	std::cout << "VAO: " << VAO << std::endl;
	std::cout << "VBO: " << VBO << std::endl;
	std::cout << "EBO: " << EBO << std::endl;
}

void Mesh::logTexId() {
	std::cout << this->tex_ids << std::endl;
}
