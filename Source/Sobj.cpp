//
//  Sobj.cpp
//  103033241_AS4
//
//  Created by Softcat on 12/22/18.
//  Copyright © 2018 NTHU. All rights reserved.
//

#include "Sobj.hpp"

Sobj::Sobj(std::string name){
    _name = name;
    glGenFramebuffers(1, &fbo);
}
void Sobj::reshape(int width, int height){
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
void Sobj::beforeMakingStencilMap(int width, int height){
    /*
     * This function should to be called BEFORE getting a stencil map with pixels covering the object
     * equal to one.
     *   1. bind frame buffer
     *   2. clear color buffer, depth buffer and stencil buffer
     *   3. enable stencil test and depth test
     *   4. set stencil test
     *   -- Draw the model in draw() function --
     */
    static const GLfloat gray[] = { 1.0f, 0.5f, 0.5f, 1.0f };
    static const GLfloat ones[] = { 1.0f };
    
    // Enable stencil test and disable depth test
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    
    // Clear buffers
    glStencilMask(0xFF);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    glViewport(0, 0, width, height);
    glClearBufferfv(GL_COLOR, 0, gray);
    glClearBufferfv(GL_DEPTH, 0, ones);
    
    // Setting stencil test
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
}
void Sobj::afterMakingStencilMap(){
    /* This function should be called after getting a stencil map.
     *   1. Stop writing to the stencil map.
     */
    glStencilMask(0x00);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}
void Sobj::beforeDrawQuadShadow(){
    /* This function should be called before drawing the quad and shadow while masking out the model by
     * applying the stencil map.
     *  1. Set stencil function
     *  2. Clear color buffer
     *  3. enable depth test
     */
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // only the part with the value in the stencil map != 1 pass
    glStencilMask(0x00);
    
    static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, black);
    
    glEnable(GL_DEPTH_TEST);// without this, no shadow
}

void Sobj::afterDrawQuadShadow(){
    /* Dummy function, nothing in here
     *  1. Disable stencil test
     *  2. Bind to default framebuffer
     */
    glStencilMask(0x00);
    glDisable(GL_STENCIL_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Sobj::log(){
    printf("= = = = = =\n");
    printf("  name:%s\n", _name.c_str());
    printf("  fbo:%d\n", fbo);
    printf("  depth_stencil_map:%d\n", depth_stencil_map);
    printf("  texture_map:%d\n", texture_map);
    printf("= = = = = =\n");
}
void Sobj::readme(){
    std::cout << std::string("[Sobj]\n")+
                 std::string(" > beforeMakingStencilMap()\n")+
                 std::string(" >   draw model\n")+
                 std::string(" > afterMakingStencilMap()\n")+
                 std::string(" > beforeDrawQuadShadow()")+
                 std::string(" >   draw quad\n")+
                 std::string(" >   draw model\n")+
                 std::string(" > afterDrawQuadShadow()\n")<< std::endl;
}
