//
//  SsaoFbo.cpp
//  103033241_AS4
//
//  Created by Softcat on 1/7/19.
//  Copyright © 2019 NTHU. All rights reserved.
//

#include "SsaoFbo.hpp"

SsaoFbo::SsaoFbo(std::string name){
    _name = name;
    glGenFramebuffers(1, &fbo);
}
void SsaoFbo::reshape(int width, int height){
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
void SsaoFbo::beforeDraw(){
    /* This function should be called before drawing ssao_vao.*/
    static const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_BACK);
    glClearBufferfv(GL_COLOR, 0, white);
    glDisable(GL_DEPTH_TEST);
}
void SsaoFbo::afterDraw(){
    /* Dummy function, nothing in here
     *  1. Disable stencil test
     *  2. Bind to default framebuffer
     */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
}
void SsaoFbo::log(){
    printf("= = = = = =\n");
    printf("  name:%s\n", _name.c_str());
    printf("  fbo:%d\n", fbo);
    printf("  depth_stencil_map:%d\n", depth_stencil_map);
    printf("  texture_map:%d\n", texture_map);
    printf("= = = = = =\n");
}
void SsaoFbo::readme(){
    std::cout << std::string("[SsaoFbo]\n")+
    std::string(" > beforeDraw()\n")+
    std::string(" >   ???\n")+
    std::string(" >   ???\n")+
    std::string(" > afterDraw()\n")<< std::endl;
}
