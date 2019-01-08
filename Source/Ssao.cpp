//
//  Ssao.cpp
//  103033241_AS4
//
//  Created by Softcat on 1/8/19.
//  Copyright © 2019 NTHU. All rights reserved.
//

#include "Ssao.hpp"

Ssao::Ssao(){
    ssao_vao = 99;
    kernal_ubo = 99;
    noise_map = 99;
}
void Ssao::loadVao(GLuint ssao_prog){
    GLint blockIdx = glGetUniformBlockIndex(ssao_prog, "Kernals");
    glUniformBlockBinding(ssao_prog, blockIdx, 0);
    glGenVertexArrays(1, &ssao_vao);
}
void Ssao::loadKernalUbo(){
    glBindVertexArray(ssao_vao);
    glGenBuffers(1, &kernal_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, kernal_ubo);
    glm::vec4 kernals[32];
    srand(time(NULL));
    for(int i = 0; i < 32; ++i)
    {
        float scale = i / 32.0;
        scale = 0.1f + 0.9f * scale * scale;
        kernals[i] = glm::vec4(normalize(glm::vec3(
                                                   rand() / (float)RAND_MAX * 2.0f - 1.0f,
                                                   rand() / (float)RAND_MAX * 2.0f - 1.0f,
                                                   rand() / (float)RAND_MAX * 0.85f + 0.15f)) * scale,
                               0.0f
                               );
    }
    glBufferData(GL_UNIFORM_BUFFER, 32 * sizeof(glm::vec4), &kernals[0][0], GL_STATIC_DRAW);
    glBindVertexArray(0);
}
void Ssao::loadNoiseMap(){
    glGenTextures(1, &noise_map);
    glBindTexture(GL_TEXTURE_2D, noise_map);
    glm::vec3 noiseData[16];
    for (int i = 0; i < 16; ++i)
    {
        noiseData[i] = normalize(glm::vec3(
                                           rand() / (float)RAND_MAX, // 0.0 ~ 1.0
                                           rand() / (float)RAND_MAX, // 0.0 ~ 1.0
                                           0.0f
                                           ));
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 4, 4, 0, GL_RGB, GL_FLOAT, &noiseData[0][0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
