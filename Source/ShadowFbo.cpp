//
//  ShadowFbo.cpp
//  103033241_AS4
//
//  Created by Softcat on 12/22/18.
//  Copyright © 2018 NTHU. All rights reserved.
//

#include "ShadowFbo.hpp"

void ShadowFbo::reshape(){
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glDeleteTextures(1, &depth_map);
    glGenTextures(1, &(depth_map));   // use GL_DEPTH_ATTACHMENT to attach depth map
    glBindTexture(GL_TEXTURE_2D, depth_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0,  GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_map, 0);
}
ShadowFbo::ShadowFbo(std::string name){
    _name = name;
    
    const GLfloat _window_positions[] =
    {
        1.0f,-1.0f,1.0f,0.0f,
        -1.0f,-1.0f,0.0f,0.0f,
        -1.0f,1.0f,0.0f,1.0f,
        1.0f,1.0f,1.0f,1.0f
    };
    
    glGenVertexArrays(1, &window_vao);
    glBindVertexArray(window_vao);
    
    GLuint window_buffer;
    glGenBuffers(1, &window_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, window_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_window_positions), _window_positions, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (const GLvoid*)(sizeof(GL_FLOAT) * 2));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    // create fbo(shader buffer object)
    glGenFramebuffers(1, &fbo);
}
void ShadowFbo::beforeDraw(){
    static const GLfloat gray[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    static const GLfloat ones[] = { 1.0f };
    
    glBindFramebuffer(GL_FRAMEBUFFER, fbo); // Draw to frame buffer object
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE); // Specify SHADOW_MAP_SIZE
    
    glClearBufferfv(GL_COLOR, 0, gray);
    glClearBufferfv(GL_DEPTH, 0, ones);
    
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(4.0f, 4.0f);
}
void ShadowFbo::afterDraw(){
    glDisable(GL_POLYGON_OFFSET_FILL);
}
void ShadowFbo::log(){
    printf("= = = = = =\n");
    printf("  name:%s\n", _name.c_str());
    printf("  fbo:%d\n", fbo);
    printf("  depth_map:%d\n", depth_map);
    printf("  texture_map:%d\n", texture_map);
    printf("= = = = = =\n");
}
