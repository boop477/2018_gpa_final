#include "../Include/Common.h"
#include "Helper.h"
//#include "Mesh.hpp"
#include "UniformList.h"
#include "CubeMap.hpp"
#include "Quad.hpp"
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

CubeMap* cube_map;           // Skymap
Quad* quad;                  // Quad
Model* mesh;                 // Scene
Fbximport zombie;            // Our SUPER CUTE zombie

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
    uniforms.fbo2screen.tex_sobj = glGetUniformLocation(fbo2screen_prog, "tex_sobj");
    uniforms.fbo2screen.tex_snoobj = glGetUniformLocation(fbo2screen_prog, "tex_snoobj");
    uniforms.fbo2screen.tex_sb = glGetUniformLocation(fbo2screen_prog, "tex_sb");
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
    
    quad = new Quad(vec3(-20.0, -12.5, 0.0),
                    vec3(1.0, 1.0, 1.0),
                    glm::quat(vec3(0.0, 0.0, 0.0)));
    
    mesh = new Model("Space/Space Station Scene.obj",
                     glm::vec3(0.0, 0.0, 0.0),
                     glm::quat(glm::vec3(radians(0.0), radians(90.0), radians(0.0))),
                     glm::vec3(0.15, 0.15, 0.15));
    
    zombie.loadmodel("zombie_walk.FBX",
                     glm::vec3(0, 0.9, 0),
                     glm::quat(glm::vec3(radians(0.0), radians(90.0), radians(0.0))),
                     vec3(0.05f, 0.05f, 0.05f));
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
    static const GLfloat gray[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    static const GLfloat pink[] = { 1.0f, 0.5f, 0.5f, 1.0f };
    static const GLfloat ones[] = { 1.0f };
    const float shadow_range = 25.0f;
    mat4 scale_bias_matrix = translate(mat4(), vec3(0.5f, 0.5f, 0.5f)) * scale(mat4(), vec3(0.5f, 0.5f, 0.5f));
    
    // == View and projection matrix for light space == //
    mat4 light_proj_matrix = ortho(-shadow_range, shadow_range, -shadow_range, shadow_range, 0.0f, 20.0f); // far plane must be far enough.
    //mat4 light_view_matrix = lookAt(vec3(-31.75, 26.05, -97.72), vec3(-20.0f, -23.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 light_view_matrix = lookAt(vec3(2.50, 3.00, 5.00), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 light_vp_matrix = light_proj_matrix * light_view_matrix;
    mat4 shadow_sbpv_matrix = scale_bias_matrix * light_vp_matrix;
    
    // == View and projection matrix == //
    /*view_matrix = lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(-1.0f, -1.0f, 0.0f), vec3(0.0, 1.0, 0.0));
     inv_vp_matrix = inverse(proj_matrix * view_matrix);*/
    view_matrix = camera.getView();
    inv_vp_matrix = inverse(proj_matrix * view_matrix);
    
    // == Draw the depth map == //
    glUseProgram(depth_prog);
    shadow_fbo->beforeDraw();
    mesh->draw(uniforms, light_vp_matrix);
    quad->draw(uniforms, light_vp_matrix);
    shadow_fbo->afterDraw();
    
    // == Get the stencil map == //
    glUseProgram(bf_render_prog);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    // Bind textures
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadow_fbo->depth_map);
    glUniform1i(uniforms.render.tex_shadow, 1);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);
    glUniform1i(uniforms.render.tex_cubemap, 2);
    
    // Get a stencil map
    s_obj->beforeMakingStencilMap(viewport_size.width, viewport_size.height);
    mesh->draw(uniforms, view_matrix, proj_matrix, shadow_sbpv_matrix, bfshading_effect);//T
    s_obj->afterMakingStencilMap();
    
    // == Quad + Shadow - Model == //
    s_obj->beforeDrawQuadShadow();
    mesh->draw(uniforms, view_matrix, proj_matrix, shadow_sbpv_matrix, bfshading_effect);//T
    //quad->draw(uniforms, view_matrix, proj_matrix, shadow_sbpv_matrix, bfshading_effect);//T
    s_obj->afterDrawQuadShadow();
    
    // == Quad - Model == //
    s_noobj->bindStencilBuffer(s_obj->depth_stencil_map, viewport_size.width, viewport_size.height);
    s_noobj->beforeDrawQuad();
    mesh->draw(uniforms, view_matrix, proj_matrix, shadow_sbpv_matrix, bfshading_effect);// false
    //quad->draw(uniforms, view_matrix, proj_matrix, shadow_sbpv_matrix, false);
    s_noobj->afterDrawQuad();
    
    // == SSAO == //
    // depth-normal path
    g_buffer->beforeDraw();
    glUseProgram(depth_normal_prog);
    mesh->draw(uniforms, camera.getView(), camera.getProjection());
    g_buffer->afterDraw();
    
    // SSao
    ssao_fbo->beforeDraw();
    glUseProgram(ssao_prog);
    
    glm::mat4 proj_matrix = camera.getProjection();
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
    
    //cube_map->draw(uniforms, view_matrix, inv_vp_matrix);
    
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
    //zombie.draw(uniforms, view_matrix, proj_matrix, shadow_sbpv_matrix, bfshading_effect, timer_cnt);
    
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
            // differential rendering
            /*glActiveTexture(GL_TEXTURE1);
             glBindTexture(GL_TEXTURE_2D, s_b->texture_map);
             glUniform1i(uniforms.fbo2screen.tex_sb, 1);
             glActiveTexture(GL_TEXTURE2);
             glBindTexture(GL_TEXTURE_2D, s_obj->texture_map);
             glUniform1i(uniforms.fbo2screen.tex_sobj, 2);
             glActiveTexture(GL_TEXTURE3);
             glBindTexture(GL_TEXTURE_2D, s_noobj->texture_map);
             glUniform1i(uniforms.fbo2screen.tex_snoobj, 3);
             glUniform1i(uniforms.fbo2screen.is_using_df, 1);*/
            
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
    proj_matrix = camera.getProjection();
}
void My_Timer(int val){
    timer_cnt++;
    glutPostRedisplay();
    if (timer_enabled)
    {
        glutTimerFunc(timer_speed, My_Timer, val);
    }
}
void My_PassiveMousePosition(int x, int y) {
    //glUniform2f(u_mouse_pos, float(x)/screenView.x, (screenView.y - float(y))/screenView.y);
    camera.trackballUpdate(x, y, viewport_size.width, viewport_size.height);
}
void My_Keyboard(unsigned char key, int x, int y)
{
    /*
     glm::vec3 cam_eye = glm::vec3(-10.0f, 5.0f, 0.0f);
     glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);*/
    float unit = 0.5;
    if (key == 'w') {
        camera.moveFront();
    }
    else if (key == 's') {
        camera.moveBack();
    }
    else if (key == 'a') {
        camera.moveLeft();
    }
    else if (key == 'd') {
        camera.moveRight();
    }
    else if (key == 'z') {
        camera.moveUp();
    }
    else if (key == 'x') {
        camera.moveDown();
    }
    else if(key == 'i'){
        fb2screen_flag->add(1);
    }
    printf("%f %f %f\n", camera.eye_pos.x, camera.eye_pos.y, camera.eye_pos.z);
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
        default:
            break;
    }
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
    glutCreateWindow("Assignment 4"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
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
    
    glutSetMenu(menu_main);
    glutAddSubMenu("Timer", menu_timer);
    glutAddSubMenu("Normal map", menu_normal_map);
    glutAddSubMenu("Shadow", menu_shadow);
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
    
    // Enter main event loop.
    glutMainLoop();
    
    return 0;
}
