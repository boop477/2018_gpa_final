//
//  CubeMap.cpp
//  103033241_AS4
//
//  Created by Softcat on 12/19/18.
//  Copyright © 2018 NTHU. All rights reserved.
//

#include "CubeMap.hpp"
#include "UniformList.h"
#include "../Include/STB/stb_image.h"

CubeMap::CubeMap(){
    _vao = 999;
}
GLuint CubeMap::loadTexture(){
    glGenTextures(1, &tex_envmap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);
    std::vector<std::string> textures_name;
    textures_name.push_back("cubemaps/face-r.png");
    textures_name.push_back("cubemaps/face-l.png");
    textures_name.push_back("cubemaps/face-t.png");
    textures_name.push_back("cubemaps/face-d.png");
    textures_name.push_back("cubemaps/face-b.png");
    textures_name.push_back("cubemaps/face-f.png");
    int width, height, nrChannels;
    unsigned char *data = NULL;
    for(int i = 0; i < 6; ++i)
    {
        data = stbi_load(textures_name[i].c_str(), &width, &height, &nrChannels, 0);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glGenVertexArrays(1, &_vao);
    return tex_envmap;
}
void CubeMap::draw(UniformList uniform_list,
                   glm::mat4 view_matrix,
                   glm::mat4 inv_vp_matrix
                   ){
    /*
     * Should invoke glUseProgram before calling this function.
     */
    glBindVertexArray(_vao);
    
    glUniformMatrix4fv(uniform_list.skybox.view_matrix, 1, GL_FALSE, &view_matrix[0][0]);
    glUniformMatrix4fv(uniform_list.skybox.inv_vp_matrix, 1, GL_FALSE, &inv_vp_matrix[0][0]);
    
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glEnable(GL_DEPTH_TEST);
}
