#include "../Include/Common.h"
#include "Helper.h"
//#include "Mesh.hpp"
#include "UniformList.h"
#include "CubeMap.hpp"
//#include "Quad.hpp"
#include "ShadowFbo.hpp"
#include "Sobj.hpp"
#include "Snoobj.hpp"
#include "Sb.hpp"
#include "MaxRangeInt.h"
#include "model.h"
#include "Camera.hpp"
#include "Ssao.hpp"
#include "SsaoFbo.hpp"
#include "Gbuffer.hpp"
#include "Menu.h"
#include "BfshadingEffect.h"
#include "FBXLoader/fbximport.h"
#include "Bezier.h"
#include "Character.hpp"

GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;

using namespace glm;
using namespace std;

mat4 view_matrix;
mat4 proj_matrix;
mat4 inv_vp_matrix;

void My_Reshape(int width, int height);

using namespace glm;
using namespace std;

GLuint depth_prog;           // Program to get a depth map
GLuint bf_render_prog;          // Program to draw a model/a quad with env_map, shadow ahd BF lighting
GLuint skybox_prog;          // Program to draw a skybox
GLuint fbo2screen_prog;      // Program to draw a rbo to screen
GLuint depth_normal_prog;    // Program that should run before ssao_prog
GLuint ssao_prog;         // Program to draw ssao

GLuint tex_envmap;           // Skybox's loaded texture

int current_menu = MENU_ALL; // Selected menu
BfShadingEffect bfshading_effect; // Select shading effect
UniformList uniforms;        // A struct to save the location of uniform variables
MaxRangeInt* fb2screen_flag; // A flag to tell fb2screen_prog that what texture should be draw to screen.
ViewportSize viewport_size;  // A struct to save (width, height). This will be used in fbo.reshape(...)
Camera camera = Camera();
Camera camera_out = Camera();

CubeMap* cube_map;           // Skymap
//Quad* quad;                  // Quad
Model* mesh;                 // Scene
Fbximport zombie_1;            // Our SUPER CUTE zombie
Fbximport* zombie = nullptr;
Fbximport boy_idle;
Fbximport boy_run;
Character* char_boy;

ShadowFbo* shadow_fbo;       // Draw shadow to this fbo
Sobj* s_obj;                 // Draw quad+shadow-model to this fbo
Snoobj* s_noobj;             // Draw quad-model to this fbo
Sb* s_b;                     // Draw skymap+model to this fbo

Ssao ssao_c = Ssao();      // Ssao vao
Gbuffer* g_buffer;           // Frame buffer object to save depth and normal. This guy is being used to get ssao.
SsaoFbo* ssao_fbo;           // FBO to save ssao.

void My_Init(){
    /*
     * 1. Load programs and get uniform vars index.
     * 2. Load Fbos
     * 3. Load a flag
     * 4. Load models
     * 5. log
     */
    printf("My_Init() start\n");
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    // == Program to draw fbo to screen == //
    fbo2screen_prog = createProgram("df_vertex.vs.glsl", "df_fragment.fs.glsl", "fbo2screen");
    glUseProgram(fbo2screen_prog);
    /*uniforms.fbo2screen.tex_sobj = glGetUniformLocation(fbo2screen_prog, "tex_sobj");
    uniforms.fbo2screen.tex_snoobj = glGetUniformLocation(fbo2screen_prog, "tex_snoobj");
    uniforms.fbo2screen.tex_sb = glGetUniformLocation(fbo2screen_prog, "tex_sb");*/
    uniforms.fbo2screen.is_using_df = glGetUniformLocation(fbo2screen_prog, "is_using_df");
    uniforms.fbo2screen.tex = glGetUniformLocation(fbo2screen_prog, "tex");
    // __ END __ //
    
    // == Program to draw ssao->depth+normal map == //
    depth_normal_prog = createProgram("depth_normal_vertex.vs.glsl", "depth_normal_fragment.fs.glsl", "depth_normal");
    glUseProgram(fbo2screen_prog);
    uniforms.depth_normal.mv_matrix = glGetUniformLocation(depth_normal_prog, "mv_matrix");
    uniforms.depth_normal.proj_matrix = glGetUniformLocation(depth_normal_prog, "proj_matrix");
    // __ END __ //
    
    // == Program to draw ssao == //
    ssao_prog = createProgram("ssao_vertex.vs.glsl", "ssao_fragment.fs.glsl", "ssao program");
    glUseProgram(ssao_prog);
    uniforms.ssao.normal_map = glGetUniformLocation(ssao_prog, "normal_map");
    uniforms.ssao.depth_map = glGetUniformLocation(ssao_prog, "depth_map");
    uniforms.ssao.proj = glGetUniformLocation(ssao_prog, "proj");
    uniforms.ssao.noise_map = glGetUniformLocation(ssao_prog, "noise_map");
    uniforms.ssao.noise_scale = glGetUniformLocation(ssao_prog, "noise_scale");
    // __ END __ //
    
    // == Program to draw the depth map == //
    depth_prog = createProgram("depth_vertex.vs.glsl", "depth_fragment.fs.glsl", "depth");
    uniforms.depth.mvp = glGetUniformLocation(depth_prog, "mvp");
    // __ END __ //
    
    // == Program to draw the skybox == //
    skybox_prog = createProgram("skybox_vertex.vs.glsl", "skybox_fragment.fs.glsl", "skybox");
    glUseProgram(skybox_prog);
    
    uniforms.skybox.view_matrix = glGetUniformLocation(skybox_prog, "view_matrix");
    uniforms.skybox.inv_vp_matrix = glGetUniformLocation(skybox_prog, "inv_vp_matrix");
    uniforms.skybox.tex_cubemap = glGetUniformLocation(skybox_prog, "tex_cubemap");
    // __ END __ //
    
    // == Program to draw the model with metal texture + BF shading == //
    bf_render_prog = createProgram("bf_vertex.vs.glsl", "bf_fragment.fs.glsl", "bf_render");
    glUseProgram(bf_render_prog);
    
    uniforms.render.program_id = bf_render_prog;
    uniforms.render.mv_matrix = glGetUniformLocation(bf_render_prog, "mv_matrix");
    uniforms.render.model_matrix = glGetUniformLocation(bf_render_prog, "model_matrix");
    uniforms.render.view_matrix = glGetUniformLocation(bf_render_prog, "view_matrix");
    uniforms.render.proj_matrix = glGetUniformLocation(bf_render_prog, "proj_matrix");
    uniforms.render.is_quad = glGetUniformLocation(bf_render_prog, "is_quad");
    uniforms.render.light_mvp_matrix = glGetUniformLocation(bf_render_prog, "light_mvp_matrix");
    uniforms.render.tex_cubemap = glGetUniformLocation(bf_render_prog, "tex_cubemap");
    uniforms.render.tex_shadow = glGetUniformLocation(bf_render_prog, "tex_shadow");
    uniforms.render.tex = glGetUniformLocation(bf_render_prog, "tex");
    uniforms.render.tex_ssao = glGetUniformLocation(bf_render_prog, "tex_ssao");
    uniforms.render.tex_normal_map = glGetUniformLocation(bf_render_prog, "tex_normal_map");
    uniforms.render.texture_diffuse1 = glGetUniformLocation(bf_render_prog, "texture_diffuse1");
    uniforms.render.is_shadow = glGetUniformLocation(bf_render_prog, "is_shadow");
    uniforms.render.is_ssao = glGetUniformLocation(bf_render_prog, "is_ssao");
    uniforms.render.is_normal_map = glGetUniformLocation(bf_render_prog, "is_normal_map");
    // __ END __ //
    
    // == Fbos == //
    shadow_fbo = new ShadowFbo("Shadow buffer");
    s_obj = new Sobj("Sobj:quad+shadow-model");
    s_noobj = new Snoobj("Snoobj:quad-model");
    s_b = new Sb("Sb:model+skybox");
    g_buffer = new Gbuffer("G buffer for ssao");
    ssao_fbo = new SsaoFbo("Ssao fbo");
    // __ END __ //
    
    // == Flag required by UI == //
    fb2screen_flag = new MaxRangeInt(4);
    // __ END __ //
    
    // == Load ssao == //
    ssao_c.loadVao(ssao_prog);
    ssao_c.loadKernalUbo();
    ssao_c.loadNoiseMap();
    // __ END __ //
    
    // == Load all the drawable stuff == //
    cube_map = new CubeMap();
    tex_envmap = cube_map->loadTexture();    // We've hardcoded the texture location in this function.
    
    mesh = new Model("Space/Space Station Scene.obj",
                     glm::vec3(0.0, 0.0, 0.0),
                     glm::quat(glm::vec3(radians(0.0), radians(90.0), radians(0.0))),
                     glm::vec3(0.15, 0.15, 0.15));
    /*mesh = new Model("Space/Space Station Scene.obj",
                     glm::vec3(0.0, 0.0, 0.0),
                     glm::quat(glm::vec3(radians(0.0), radians(0.0), radians(0.0))),
                     glm::vec3(0.0, 0.0, 0.0));*/

    zombie_1.loadmodel("zombie_walk_1.fbx",
                       glm::vec3(4.866417, -0.023559, -0.214670),
                       glm::quat(glm::vec3(-0.087267, -1.745329, 0.000000)),
                       glm::vec3(0.01, 0.01, 0.01),
                       glm::vec3(-0.087267, -1.745329, 0.000000),
                       "zombie_police", 250, 100);
    boy_idle.loadmodel("Idle.fbx",
                       glm::vec3(-7.133575, 0.016441, -0.014676), //-3.833578 0.076441 -0.014676
                       glm::quat(glm::vec3(0.087266, 1.570796, 0.087267)), // 0.174533 1.483530 0.349066
                       glm::vec3(0.009, 0.009, 0.009),
                       glm::vec3(0.087266, 1.570796, 0.087267),
                       "boy_idle", 47, 55);
    boy_run.loadmodel("Running.fbx",
                       glm::vec3(-7.133575, 0.016441, -0.014676), //-3.833578 0.076441 -0.014676
                       glm::quat(glm::vec3(0.087266, 1.570796, 0.087267)), // 0.174533 1.483530 0.349066
                       glm::vec3(0.009, 0.009, 0.009),
                       glm::vec3(0.087266, 1.570796, 0.087267),
                       "boy_run", 25, 55);
    char_boy = new Character(&boy_idle, &boy_run);
    // __ END __ //
    
    // == Turn on all the effects == //
    bfshading_effect.normal_map = 1;
    bfshading_effect.shadow = 1;
    bfshading_effect.ssao = 1;
    // __ END __ //
    
    // == Sampler for FBC loader == //
    GLuint sampler;
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindSampler(0, sampler);
    // __ END __ //
    
    // == Set ficed camera
    camera_out.fix(true);
    camera_out.initialize(glm::vec3(46.0f,30.0f,46.0f),
                          glm::vec3(-30.0f, -30.0f, -30.0f));
    
    // == LOG == //
    My_Reshape(1440, 900);
    shadow_fbo->log();
    s_obj->log();
    s_noobj->log();
    s_b->log();
    g_buffer->log();
    ssao_fbo->log();
    printUniformList(uniforms);
    printf("My_Init() finished\n");
}
void My_Display(){
    /*
     * 1. Get mvp matrix of the camera and the light source\
     * 2. Get depth map
     * 3. Get stencil map
     * 4. Get Sobj, Snoobj and Sb
     * 4. Fbo to screen
     */
    char_boy->mouse_update();
    
    static const GLfloat gray[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    static const GLfloat pink[] = { 1.0f, 0.5f, 0.5f, 1.0f };
    static const GLfloat ones[] = { 1.0f };
    const float shadow_range = 25.0f;
    mat4 scale_bias_matrix = translate(mat4(), vec3(0.5f, 0.5f, 0.5f)) * scale(mat4(), vec3(0.5f, 0.5f, 0.5f));
    
    // == View and projection matrix for light space == //
    mat4 light_proj_matrix = ortho(-shadow_range, shadow_range, -shadow_range, shadow_range, 0.0f, 20.0f); // far plane must be far enough.
    mat4 light_view_matrix = lookAt(vec3(2.50, 3.00, 5.00), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 light_vp_matrix = light_proj_matrix * light_view_matrix;
    mat4 shadow_sbpv_matrix = scale_bias_matrix * light_vp_matrix;
    
    // == View and projection matrix == //
    if(camera.is_activated){
        view_matrix = camera.getView();
        inv_vp_matrix = inverse(proj_matrix * view_matrix);
    }
    else if(camera_out.is_activated){
        view_matrix = camera_out.getView();
        inv_vp_matrix = inverse(proj_matrix * view_matrix);
    }
    else{
        view_matrix = char_boy->current_camera->getView();
        inv_vp_matrix = inverse(proj_matrix * view_matrix);
    }
    
    
    // == Draw the depth map == //
    glUseProgram(depth_prog);
    shadow_fbo->beforeDraw();
    mesh->draw(uniforms, light_vp_matrix);
    zombie_1.draw(uniforms, light_vp_matrix);
    char_boy->draw(uniforms, light_vp_matrix);
    shadow_fbo->afterDraw(viewport_size.width, viewport_size.height);
    
    // == SSAO == //
    // depth-normal path
    g_buffer->beforeDraw();
    glUseProgram(depth_normal_prog);
    mesh->draw(uniforms, view_matrix, proj_matrix);
    zombie_1.draw(uniforms, view_matrix, proj_matrix);
    char_boy->draw(uniforms, view_matrix, proj_matrix);
    g_buffer->afterDraw();
    
    // SSao
    ssao_fbo->beforeDraw();
    glUseProgram(ssao_prog);
    
    //glm::mat4 proj_matrix = camera.getProjection();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_buffer->normal_map);
    glUniform1i(uniforms.ssao.normal_map, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_buffer->depth_map);
    glUniform1i(uniforms.ssao.depth_map, 1);
    glUniformMatrix4fv(uniforms.ssao.proj, 1, GL_FALSE, &proj_matrix[0][0]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ssao_c.noise_map);
    glUniform1i(uniforms.ssao.noise_map, 2);
    glUniform2f(uniforms.ssao.noise_scale, viewport_size.width / 4.0f, viewport_size.height / 4.0f);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ssao_c.kernal_ubo);
    
    glBindVertexArray(ssao_c.ssao_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ssao_fbo->afterDraw();
    
    // == Model + Skybox == //
    s_b->beforeDrawSkyboxModel();
    // - Draw skybox - //
    glUseProgram(skybox_prog);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);
    glUniform1i(uniforms.skybox.tex_cubemap, 2);
    
    cube_map->draw(uniforms, view_matrix, inv_vp_matrix);
    
    // - Draw the model - //
    
    glUseProgram(bf_render_prog);
    
    // Bind textures
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadow_fbo->depth_map);
    glUniform1i(uniforms.render.tex_shadow, 1);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);
    glUniform1i(uniforms.render.tex_cubemap, 2);
    
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, ssao_fbo->texture_map);
    glUniform1i(uniforms.render.tex_ssao, 3);
    
    // draw mesh
    mesh->draw(uniforms, view_matrix, proj_matrix, shadow_sbpv_matrix, bfshading_effect);
    zombie_1.draw(uniforms, view_matrix, proj_matrix, shadow_sbpv_matrix, bfshading_effect);
    char_boy->draw(uniforms, view_matrix, proj_matrix, shadow_sbpv_matrix, bfshading_effect);
    s_b->afterDrawSkyboxModel();
    
    // == FBO to screen == //
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glUseProgram(fbo2screen_prog);
    glBindVertexArray(shadow_fbo->window_vao);
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    
    //switch(fb2screen_flag->get()){
    switch(current_menu){
        case MENU_ALL:
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, s_b->texture_map);
            glUniform1i(uniforms.fbo2screen.tex, 4);
            glUniform1i(uniforms.fbo2screen.is_using_df, 0);
            break;
        case MENU_DEPTH_LIGHT:
            // depth map from light
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, shadow_fbo->depth_map);
            glUniform1i(uniforms.fbo2screen.tex, 4);
            glUniform1i(uniforms.fbo2screen.is_using_df, 0);
            break;
        case MENU_DEPTH_EYES:
            // depth map from eyes
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, g_buffer->depth_map);
            glUniform1i(uniforms.fbo2screen.tex, 4);
            glUniform1i(uniforms.fbo2screen.is_using_df, 0);
            break;
        case MENU_SSAO:
            // SSAO
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, ssao_fbo->texture_map);
            glUniform1i(uniforms.fbo2screen.tex, 4);
            glUniform1i(uniforms.fbo2screen.is_using_df, 0);
            break;
        default:
            break;
    }
    
    glDrawArrays(GL_TRIANGLE_FAN,0,4 );
    
    glutSwapBuffers();
    glActiveTexture(GL_TEXTURE0);
}
void My_Reshape(int width, int height){
    viewport_size.width = width;
    viewport_size.height = height;
    
    /*
     glViewport(0, 0, width, height);
     float viewportAspect = (float)width / (float)height;
     proj_matrix = perspective(deg2rad(60.0f), viewportAspect, 0.1f, 1000.0f);*/
    
    shadow_fbo->reshape();
    s_obj->reshape(width, height);
    s_noobj->reshape(width, height);
    s_b->reshape(width, height);
    g_buffer->reshape(width, height);
    ssao_fbo->reshape(width, height);
    
    camera.reshape(width, height);
    camera_out.reshape(width, height);
    char_boy->reshape(width, height);
    
    if(camera.is_activated){
        proj_matrix = camera.getProjection();
    }
    else{
        proj_matrix = char_boy->current_camera->getProjection();
    }
}

void My_Timer(int val)
{
    static point p_old;
    static float n=0.0f;
    static bool turn_switch=false;
    static bool animation = false;
    static float rot_deg = 0.0;
    static float rot_scale = 2.5;
    
    timer_cnt++;
    char_boy->updateTimer();
    zombie_1.updateTimer();
    
    glutPostRedisplay();
    
    //bezier curve
    /*point a = { 0, 0 };
    point b = { -5.499997, -5.7 };
    point c = { -3.499997, -8 };
    point d = { -1.899995, -10.1 };*/
    point a = { 0, 0 };
    point b = { -2.499997, -8.199998 };
    point c = { -8.499997, -4.799998 };
    point d = { -9.899995, -1.1 };
    
    point p;
    
    if (animation){
        if (turn_switch){
            // +
            rot_deg += rot_scale;
            zombie_1.addQuaternion(glm::quat(glm::vec3(glm::radians(0.0), glm::radians(rot_scale), glm::radians(0.0))));
            if (rot_deg == 180)
                animation = false;
        }else{
            // -
            rot_deg -= rot_scale;
            zombie_1.addQuaternion(glm::quat(glm::vec3(glm::radians(0.0), glm::radians(-rot_scale), glm::radians(0.0))));
            if (rot_deg == 0)
                animation = false;
        }
        glutTimerFunc(timer_speed, My_Timer, val);
        return;
    }
    
    if(!animation &&!turn_switch){
        n+=1.0f;
        if(n>=1000.0f){
            turn_switch=!turn_switch;
            animation = true;
        }
    }
    if(!animation && turn_switch){
        n-=1.0f;
        if(n<=0.0f){
            turn_switch=!turn_switch;
            animation = true;
        }
    }
    float t = static_cast<float>(n)/999.0;
    bezier(p,a,b,c,d,t);
    zombie_1._position_add = glm::vec3(p.x-p_old.x, 0, p.y-p_old.y);
    zombie_1.addPosition(zombie_1._position_add); // Update model matrix
    p_old.x = p.x;
    p_old.y = p.y;
    //bezier curve over
    
    glutTimerFunc(timer_speed, My_Timer, val);
}
void My_PassiveMousePosition(int x, int y) {
    //glUniform2f(u_mouse_pos, float(x)/screenView.x, (screenView.y - float(y))/screenView.y);
    if(camera.is_activated) // Update debug camera
        camera.trackballUpdate(x, y, viewport_size.width, viewport_size.height);
    else if (!camera_out.is_activated && !camera.is_activated)
        char_boy->trackballFlag(x, y, viewport_size.width, viewport_size.height);
}
void My_Keyboard(unsigned char key, int x, int y)
{
    glm::vec3 add_pos = glm::vec3(0.0);
    glm::vec3 add_rot = glm::vec3(0.0);
    
    zombie = &zombie_1; // User can control pos/rot of the variable assigned to zombie
    
    float pos_unit = 0.01;
    float rot_unit = 1.0;
    
    // == For debugging == //
    if(key == 'i'){
        fb2screen_flag->add(1);
    }  // Position
    else if(key == 'f'){
        add_pos = glm::vec3(pos_unit, 0.0, 0.0);
    }
    else if(key == 'v'){
        add_pos = glm::vec3(-pos_unit, 0.0, 0.0);
    }
    else if(key == 'g'){
        add_pos = glm::vec3(0.0, pos_unit, 0.0);
    }
    else if(key == 'b'){
        add_pos = glm::vec3(0.0, -pos_unit, 0.0);
    }
    else if(key == 'h'){
        add_pos = glm::vec3(0.0, 0.0, pos_unit);
    }
    else if(key == 'n'){
        add_pos = glm::vec3(0.0, 0.0, -pos_unit);
    } // Rotation
    else if(key == 'j'){
        add_rot = glm::vec3(glm::radians(rot_unit), 0.0, 0.0);
    }
    else if(key == 'm'){
        add_rot = glm::vec3(glm::radians(-rot_unit), 0.0, 0.0);
    }
    else if(key == 'k'){
        add_rot = glm::vec3(0.0, glm::radians(rot_unit), 0.0);
    }
    else if(key == ','){
        add_rot = glm::vec3(0.0, glm::radians(-rot_unit), 0.0);
    }
    else if(key == 'l'){
        add_rot = glm::vec3(0.0, 0.0, glm::radians(rot_unit));
    }
    else if(key == '.'){
        add_rot = glm::vec3(0.0, 0.0, glm::radians(-rot_unit));
    }
    
    if (zombie != nullptr){
        zombie->addPosition(add_pos);
        zombie->addQuaternion(glm::quat(add_rot));
        zombie->addRotation(add_rot);
        zombie->log();
    }
    
    
    if (camera.is_activated){
        camera.key_update(key);
        printf("cam:%f %f %f\n", camera.eye_pos.x, camera.eye_pos.y, camera.eye_pos.z);
    }
    if (!camera.is_activated && !camera_out.is_activated)
        char_boy->key_update(key);
}
void My_SpecialKeys(int key, int x, int y){
    switch (key)
    {
        case GLUT_KEY_F1:
            printf("F1 is pressed at (%d, %d)\n", x, y);
            break;
        case GLUT_KEY_PAGE_UP:
            printf("Page up is pressed at (%d, %d)\n", x, y);
            break;
        case GLUT_KEY_LEFT:
            printf("Left arrow is pressed at (%d, %d)\n", x, y);
            break;
        default:
            printf("Other special key is pressed at (%d, %d)\n", x, y);
            break;
    }
}
void My_Menu(int id){
    switch (id)
    {
        case MENU_TIMER_START:
            if (!timer_enabled)
            {
                timer_enabled = true;
                glutTimerFunc(timer_speed, My_Timer, 0);
            }
            break;
        case MENU_TIMER_STOP:
            timer_enabled = false;
            break;
        case MENU_EXIT:
            exit(0);
            break;
        case MENU_DEPTH_LIGHT:
            current_menu = MENU_DEPTH_LIGHT;;
            break;
        case MENU_DEPTH_EYES:
            current_menu = MENU_DEPTH_EYES;
            break;
        case MENU_SSAO:
            current_menu = MENU_SSAO;
            break;
        case MENU_ALL:
            current_menu = MENU_ALL;
            break;
        case MENU_NORMAL_MAP_ON :
            bfshading_effect.normal_map = 1;
            break;
        case MENU_NORMAL_MAP_OFF:
            bfshading_effect.normal_map = 0;
            break;
        case MENU_SHADOW_ON :
            bfshading_effect.shadow = 1;
            break;
        case MENU_SHADOW_OFF:
            bfshading_effect.shadow = 0;
            break;
        case MENU_SSAO_ON :
            bfshading_effect.ssao = 1;
            break;
        case MENU_SSAO_OFF:
            bfshading_effect.ssao = 0;
            break;
        case MENU_CAM_DEBUG:
            camera.is_activated = true;
            camera_out.is_activated = false;
            break;
        case MENU_CAM_OUT:
            camera.is_activated = false;
            camera_out.is_activated = true;
            break;
        case MENU_CAM_THIRD:
            camera.is_activated = false;
            camera_out.is_activated = false;
            char_boy->selectThird();
            break;
        case MENU_CAM_FIRST:
            camera.is_activated = false;
            camera_out.is_activated = false;
            char_boy->selectFirst();
            break;
        default:
            break;
    }
}

void My_KeyboardUp(unsigned char key, int x, int y){
    printf ("%c up\n", key);
    char_boy->key_update_up(key);
}

int main(int argc, char *argv[]){
#ifdef __APPLE__
    // Change working directory to source code path
    chdir(__FILEPATH__("/../Assets/"));
#endif
    // Initialize GLUT and GLEW, then create a window.
    ////////////////////
    glutInit(&argc, argv);
#ifdef _MSC_VER
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
#endif
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1440, 900);
    glutCreateWindow("GPA_FINAL"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
    glewInit();
#endif
    ilInit();
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    dumpInfo();
    My_Init();
    
    // Create a menu and bind it to mouse right button.
    int menu_main = glutCreateMenu(My_Menu);
    int menu_timer = glutCreateMenu(My_Menu);
    int menu_normal_map = glutCreateMenu(My_Menu);
    int menu_shadow = glutCreateMenu(My_Menu);
    int menu_ssao = glutCreateMenu(My_Menu);
    int menu_cam = glutCreateMenu(My_Menu);
    
    glutSetMenu(menu_main);
    glutAddSubMenu("Timer", menu_timer);
    glutAddSubMenu("Normal map", menu_normal_map);
    glutAddSubMenu("Shadow", menu_shadow);
    glutAddSubMenu("Ssao", menu_ssao);
    glutAddSubMenu("Cameras", menu_cam);
    glutAddMenuEntry("depth map(light)", MENU_DEPTH_LIGHT);
    glutAddMenuEntry("depth map(eye)", MENU_DEPTH_EYES);
    glutAddMenuEntry("ssao", MENU_SSAO);
    glutAddMenuEntry("all", MENU_ALL);
    glutAddMenuEntry("Exit", MENU_EXIT);
    
    glutSetMenu(menu_timer);
    glutAddMenuEntry("Start", MENU_TIMER_START);
    glutAddMenuEntry("Stop", MENU_TIMER_STOP);
    
    glutSetMenu(menu_normal_map);
    glutAddMenuEntry("Normal map? YES :)", MENU_NORMAL_MAP_ON);
    glutAddMenuEntry("Normal map? NOPE :(", MENU_NORMAL_MAP_OFF);
    
    glutSetMenu(menu_shadow);
    glutAddMenuEntry("Shadow? YES :)", MENU_SHADOW_ON);
    glutAddMenuEntry("Shadow? NOPE :(", MENU_SHADOW_OFF);
    
    glutSetMenu(menu_ssao);
    glutAddMenuEntry("Ssao? YES :)", MENU_SSAO_ON);
    glutAddMenuEntry("Ssao? NOPE :(", MENU_SSAO_OFF);
    
    glutSetMenu(menu_cam);
    glutAddMenuEntry("Debug camera", MENU_CAM_DEBUG);
    glutAddMenuEntry("Skybox camera", MENU_CAM_OUT);
    glutAddMenuEntry("Third camera", MENU_CAM_THIRD);
    glutAddMenuEntry("First camera", MENU_CAM_FIRST);
    
    glutSetMenu(menu_main);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    // Register GLUT callback functions.
    glutDisplayFunc(My_Display);
    glutReshapeFunc(My_Reshape);
    //glutMouseFunc(My_Mouse);
    glutKeyboardFunc(My_Keyboard);
    glutSpecialFunc(My_SpecialKeys);
    glutTimerFunc(timer_speed, My_Timer, 0);
    glutPassiveMotionFunc(My_PassiveMousePosition);
    glutKeyboardUpFunc(My_KeyboardUp);
    
    // Enter main event loop.
    glutMainLoop();
    
    return 0;
}
