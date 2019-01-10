//
//  Sb.cpp
//  103033241_AS4
//
//  Created by Softcat on 12/26/18.
//  Copyright © 2018 NTHU. All rights reserved.
//

#include "Sb.hpp"

Sb::Sb(std::string name){
    _name = name;
    glGenFramebuffers(1, &fbo);
}
void Sb::reshape(int width, int height){
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    // depth + stencil
    glDeleteRenderbuffers(1, &depth_stencil_map);
    glGenRenderbuffers(1, &depth_stencil_map);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_map);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_map);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_map);
    
    // texture map
    glDeleteTextures(1, &texture_map);
    glGenTextures(1, &texture_map);
    glBindTexture(GL_TEXTURE_2D, texture_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_map, 0);
    
    // bind to fbo
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Sb::beforeDrawSkyboxModel(){
    /* This function should be called before drawing the skybox and the model. Note that we do not want to
     * use stencil test.
     *  1. Disable stencil test and enable depth test
     *  2. Bind to fbo
     *  3. Clear color buffer and depth buffer
     */
    static const GLfloat red[] = { 0.2f, 0.0f, 0.0f, 1.0f };
    static const GLfloat ones[] = { 1.0f };
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glClearBufferfv(GL_COLOR, 0, red);
    glClearBufferfv(GL_DEPTH, 0, ones);
}
void Sb::afterDrawSkyboxModel(){
    /* Dummy function, nothing in here
     *  1. Disable stencil test
     *  2. Bind to default framebuffer
     */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Sb::log(){
    printf("= = = = = =\n");
    printf("  name:%s\n", _name.c_str());
    printf("  fbo:%d\n", fbo);
    printf("  depth_stencil_map:%d\n", depth_stencil_map);
    printf("  texture_map:%d\n", texture_map);
    printf("= = = = = =\n");
}
void Sb::readme(){
    std::cout << std::string("[Snoobj]\n")+
    std::string(" > beforeDrawSkyboxModel()\n")+
    std::string(" >   draw skybox\n")+
    std::string(" >   draw model\n")+
    std::string(" > afterDrawSkyboxModel()\n")<< std::endl;
}
