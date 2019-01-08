#ifndef model_hpp
#define model_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
/*
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#include <unistd.h>*/

#include "mesh.hpp"
#include "UniformList.h"

#include "../Include/GLM/glm/glm.hpp"
#include "../Include/GLM/glm/gtx/transform.hpp"
#include "../Include/GLM/glm/gtx/quaternion.hpp"
#include "../Include/assimp/Importer.hpp"
#include "../Include/assimp/cimport.h"
#include "../Include/assimp/scene.h"
#include "../Include/assimp/postprocess.h"


class Model{
public:
    Model(std::string filename, std::string prefix, glm::vec3 position, glm::quat quaternion, glm::vec3 scale);
    void draw(UniformList uniform_list,
                glm::mat4 view_matrix,
                glm::mat4 proj_matrix,
                glm::mat4 light_vp_matrix,
                bool is_shadow);
    void draw(UniformList uniform_list,
              glm::mat4 light_vp_matrix);
    // For ssao
    void draw(UniformList uniform_list,
              glm::mat4 view_matrix,
              glm::mat4 proj_matrix);
	void log();

	void addPosition(glm::vec3 position);
	void setPosition(glm::vec3 position);
	void addQuaternion(glm::quat quaternion);
	void setQuaternion(glm::quat quaternion);
	void addScale(glm::vec3 scale);
	void setScale(glm::vec3 scale);
	GLuint getTextureID(std::string path);
    
    GLuint tex_ID;
private:
	std::string _prefix;
    std::vector<Mesh> _meshes;
    std::vector<GLuint> _meshes_texture_ids;
    std::map<std::string, GLuint> _loaded_tex_path_id;
    glm::vec3 _position;
    glm::quat _quaternion;
    glm::vec3 _scale;
    GLint _um4mv;
    GLint _um4p;
    
    void parseTree(aiNode* node, const aiScene* scene);
    Mesh parseMesh(aiMesh* mesh_data, const aiScene* scenes);
	glm::mat4 getModelMatrix();
};

#endif /* model_hpp */
