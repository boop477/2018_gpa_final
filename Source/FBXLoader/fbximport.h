#pragma once
#ifndef FBXIMPORT_H
#define FBXIMPORT_H

//#include <glew.h>
//#include <freeglut.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <cstdlib>
#include <tiny_obj_loader.h>
#include "fbxloader.h"
#include <IL/il.h>
#include <vector>
#include <ctime>

#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#include <unistd.h>
#include "../../Include/GLM/glm/glm.hpp"
#include "../../Include/GLM/glm/gtc/matrix_transform.hpp"
#include "../../Include/GLM/glm/gtc/quaternion.hpp"
#include "../../Include/GLM/glm/gtx/transform.hpp"

using namespace std;


typedef struct
{
	GLuint vao;
	GLuint vbo;
	GLuint vboTex;
	GLuint ebo;
	int materialId;
	int indexCount;
} Shape;

typedef struct
{
	GLuint texId;
} Material;

class Fbximport {
public:
	/*  Mesh Data  */
	typedef struct
	{
		GLuint vao;
		GLuint vbo;
		GLuint vboTex;
		GLuint ebo;
		int materialId;
		int indexCount;
	} Shape;

	typedef struct
	{
		GLuint texId;
	} Material;

	vector<Shape> characterShapes;
	vector<Material> characterMaterials;
	fbx_handles characterFbx;
    
    glm::vec3 _position;
    glm::vec3 _position_add; // This is for moving in(?) bezier curve. REMEMBER TO add this var to _position everytime before getModelMatrix()
    glm::quat _quaternion;
    glm::vec3 _scale;

	
	/*  Functions  */
	// constructor
	void loadmodel(string const &path, glm::vec3 position, glm::quat quaternion, glm::vec3 scale)
	{
		setupMesh(path);
        this->_position = position;
        this->_quaternion = quaternion;
        this->_scale = scale;
	}
    
    void loadmodel(string const &path, bool gamma = false)
    {
        setupMesh(path);
    }

    // render the mesh
    void Draw(GLubyte timer_cnt)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        std::vector<tinyobj::shape_t> new_shapes;
        GetFbxAnimation(characterFbx, new_shapes, timer_cnt / 255.0f);
        
        for (unsigned int i = 0; i < characterShapes.size(); i++)
        {
            glBindVertexArray(characterShapes[i].vao);
            glBindBuffer(GL_ARRAY_BUFFER, characterShapes[i].vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, new_shapes[i].mesh.positions.size() * sizeof(float), new_shapes[i].mesh.positions.data());
            glBindTexture(GL_TEXTURE_2D, characterMaterials[characterShapes[i].materialId].texId);
            glDrawElements(GL_TRIANGLES, characterShapes[i].indexCount, GL_UNSIGNED_INT, 0);
        }
    }

	// render the mesh
    void draw(UniformList uniform_list,
              glm::mat4 view_matrix,
              glm::mat4 proj_matrix,
              glm::mat4 light_vp_matrix,
              BfShadingEffect bfshading_effect,
              GLubyte timer_cnt){
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 model = getModelMatrix();
        glm::mat4 light_mvp_matrix = light_vp_matrix * model;
        glUniformMatrix4fv(uniform_list.render.model_matrix, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(uniform_list.render.view_matrix, 1, GL_FALSE, &view_matrix[0][0]);
        glUniformMatrix4fv(uniform_list.render.proj_matrix, 1, GL_FALSE, &proj_matrix[0][0]);
        glUniformMatrix4fv(uniform_list.render.light_mvp_matrix, 1, GL_FALSE, &light_mvp_matrix[0][0]);
        glUniform1i(uniform_list.render.is_shadow, bfshading_effect.shadow);
        glUniform1i(uniform_list.render.is_normal_map, 0); // Lack of normal map in fbx
        glUniform1i(uniform_list.render.is_ssao, bfshading_effect.ssao);

		std::vector<tinyobj::shape_t> new_shapes;
		GetFbxAnimation(characterFbx, new_shapes, timer_cnt / 255.0f);

        glActiveTexture(GL_TEXTURE4);
        glUniform1i(uniform_list.render.texture_diffuse1, 4);
		for (unsigned int i = 0; i < characterShapes.size(); i++)
		{
			glBindVertexArray(characterShapes[i].vao);
			glBindBuffer(GL_ARRAY_BUFFER, characterShapes[i].vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, new_shapes[i].mesh.positions.size() * sizeof(float), new_shapes[i].mesh.positions.data());
			glBindTexture(GL_TEXTURE_2D, characterMaterials[characterShapes[i].materialId].texId);
			glDrawElements(GL_TRIANGLES, characterShapes[i].indexCount, GL_UNSIGNED_INT, 0);
		}
        glActiveTexture(GL_TEXTURE0);
	}

    void addPosition(glm::vec3 position){
        this->_position += position;
    }
    void setPosition(glm::vec3 position){
        this->_position = position;
    }
    void addQuaternion(glm::quat quaternion){
        this->_quaternion = this->_quaternion * quaternion;
    }
    void setQuaternion(glm::quat quaternion){
        this->_quaternion = quaternion;
    }
    void addScale(glm::vec3 scale){
        this->_scale += scale;
    }
    void setScale(glm::vec3 scale){
        this->_scale = scale;
    }
    glm::mat4 getModelMatrix() {
        // Translation matrix
        glm::mat4 translation_matrix = glm::translate(glm::mat4(), _position);
        // Rotation matrix
        glm::mat4 rotation_matrix = glm::toMat4(_quaternion);
        // Scale matrix
        glm::mat4 scale_matrix = glm::scale(_scale);
        
        return translation_matrix * rotation_matrix * scale_matrix;
    }
    
private:
	/*  Render data  */
	unsigned int VBO, EBO;

	/*  Functions    */
	// initializes all the buffer objects/arrays
	void setupMesh(string const &path, bool gamma = false)
	{
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string err;
		const char *path_char = path.c_str();

		bool ret = LoadFbx(characterFbx, shapes, materials, err, path_char);
		//bool ret = LoadFbx(characterFbx, shapes, materials, err, "fbx/Space Station Scene.FBX");
		//bool ret = LoadFbx(characterFbx, shapes, materials, err, "zombie_dead2.FBX");

		if (ret)
		{
            printf ("load:%s\n", path_char);
			// For Each Material
			for (int i = 0; i < materials.size(); i++)
			{
				ILuint ilTexName;
				ilGenImages(1, &ilTexName);
				ilBindImage(ilTexName);
				Material mat;
				if (ilLoadImage(materials[i].diffuse_texname.c_str()))
				{
                    printf ("load texture:%s\n", materials[i].diffuse_texname.c_str());
					int width = ilGetInteger(IL_IMAGE_WIDTH);
					int height = ilGetInteger(IL_IMAGE_HEIGHT);
					unsigned char *data = new unsigned char[width * height * 4];
					ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, data);

					glGenTextures(1, &mat.texId);
					glBindTexture(GL_TEXTURE_2D, mat.texId);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					glGenerateMipmap(GL_TEXTURE_2D);

					delete[] data;
					ilDeleteImages(1, &ilTexName);
				}
				characterMaterials.push_back(mat);
			}

			// For Each Shape (or Mesh, Object)
			for (int i = 0; i < shapes.size(); i++)
			{
				Shape shape;
				glGenVertexArrays(1, &shape.vao);
				glBindVertexArray(shape.vao);

				glGenBuffers(3, &shape.vbo);
				glBindBuffer(GL_ARRAY_BUFFER, shape.vbo);
				glBufferData(GL_ARRAY_BUFFER, shapes[i].mesh.positions.size() * sizeof(float), shapes[i].mesh.positions.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glBindBuffer(GL_ARRAY_BUFFER, shape.vboTex);
				glBufferData(GL_ARRAY_BUFFER, shapes[i].mesh.texcoords.size() * sizeof(float), shapes[i].mesh.texcoords.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.ebo);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[i].mesh.indices.size() * sizeof(unsigned int), shapes[i].mesh.indices.data(), GL_STATIC_DRAW);
				shape.materialId = shapes[i].mesh.material_ids[0];
				shape.indexCount = shapes[i].mesh.indices.size();
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(2);
				characterShapes.push_back(shape);
			}
		}
        else{
            printf ("FAILED load:%s\n", path_char);
        }
	}
};

#endif
