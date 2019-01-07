//
//  Fbo.h
//  103033241_AS4
//
//  Created by Softcat on 12/19/18.
//  Copyright © 2018 NTHU. All rights reserved.
//

#ifndef Fbo_h
#define Fbo_h

#define SHADOW_MAP_SIZE 4096

static const GLfloat window_positions[] =
{
    1.0f,-1.0f,1.0f,0.0f,
    -1.0f,-1.0f,0.0f,0.0f,
    -1.0f,1.0f,0.0f,1.0f,
    1.0f,1.0f,1.0f,1.0f
};

struct ShadowBuffer
{
    GLuint fbo;
    GLuint depth_map;
    GLuint texture_map;
};

void fbo_reshape(ShadowBuffer* shadow_buffer, int width, int height){
    glDeleteRenderbuffers(1, &(shadow_buffer->depth_map));
    glDeleteTextures(1, &(shadow_buffer->texture_map));
    glGenRenderbuffers(1, &(shadow_buffer->depth_map));
    glBindRenderbuffer(GL_RENDERBUFFER, shadow_buffer->depth_map);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
    //FBODataTexture;
    glGenTextures(1, &(shadow_buffer->texture_map));
    glBindTexture(GL_TEXTURE_2D, shadow_buffer->texture_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadow_buffer->fbo);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, shadow_buffer->depth_map);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadow_buffer->texture_map, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void fboShadowReshape(ShadowBuffer* shadow_buffer){
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_buffer->fbo);
    
    glGenTextures(1, &(shadow_buffer->depth_map));   // use GL_DEPTH_ATTACHMENT to attach depth map
    glBindTexture(GL_TEXTURE_2D, shadow_buffer->depth_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0,  GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_buffer->depth_map, 0);
}

void shaderProgLog(ShadowBuffer s){
    printf("= = = = = =\n");
    printf("  fbo:%d\n", s.fbo);
    printf("  depth_map:%d\n", s.depth_map);
    printf("  texture_map:%d\n", s.texture_map);
    printf("= = = = = =\n");
}

#endif /* Fbo_h */
