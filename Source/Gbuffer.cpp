//
//  Gbuffer.cpp
//  103033241_AS4
//
//  Created by Softcat on 1/7/19.
//  Copyright © 2019 NTHU. All rights reserved.
//

#include "Gbuffer.hpp"

void Gbuffer::reshape(int width, int height){
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    //glDeleteTextures(1, &normal_map);
    //glGenTextures(1, &normal_map);
    glBindTexture(GL_TEXTURE_2D, normal_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    
    //glDeleteTextures(1, &depth_map);
    //glGenTextures(1, &(depth_map));
    glBindTexture(GL_TEXTURE_2D, depth_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    //glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normal_map, 0);
    //glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
Gbuffer::Gbuffer(std::string name){
    _name = name;
    
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    
    glGenTextures(2, &normal_map);
    glBindTexture(GL_TEXTURE_2D, normal_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 256, 256, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, depth_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 256, 256, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normal_map, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
void Gbuffer::beforeDraw(){
    static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    static const GLfloat ones[] = { 1.0f };
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClearBufferfv(GL_COLOR, 0, black);
    glClearBufferfv(GL_DEPTH, 0, ones);
    
    glEnable(GL_DEPTH_TEST);
}
void Gbuffer::afterDraw(){
    // Do nothing
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Gbuffer::log(){
    printf("= = = = = =\n");
    printf("  name:%s\n", _name.c_str());
    printf("  fbo:%d\n", fbo);
    printf("  depth_map:%d\n", depth_map);
    printf("  normal_map:%d\n", normal_map);
    printf("= = = = = =\n");
}
