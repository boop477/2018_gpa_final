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
#include "model.hpp"
#include "Camera.hpp"

#define MENU_TIMER_START 1
#define MENU_TIMER_STOP 2
#define MENU_EXIT 3

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
GLuint render_prog;          // Program to draw a model/a quad with env_map, shadow ahd BF lighting
GLuint skybox_prog;          // Program to draw a skybox
GLuint fbo2screen_prog;      // Program to draw a rbo to screen

GLuint tex_envmap;           // Skybox's loaded texture

UniformList uniforms;        // A struct to save the location of uniform variables
MaxRangeInt* fb2screen_flag; // A flag to tell fb2screen_prog that what texture should be draw to screen.
ViewportSize viewport_size;  // A struct to save (width, height). This will be used in fbo.reshape(...)
Camera camera = Camera();

//Mesh* mesh;                  // Model
CubeMap* cube_map;           // Skymap
Quad* quad;                  // Quad
Model* mesh;

ShadowFbo* shadow_fbo;       // Draw shadow to this fbo
Sobj* s_obj;                 // Draw quad+shadow-model to this fbo
Snoobj* s_noobj;             // Draw quad-model to this fbo
Sb* s_b;                     // Draw skymap+model to this fbo

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
    
    // Program to draw fbo to screen
    fbo2screen_prog = createProgram("df_vertex.vs.glsl", "df_fragment.fs.glsl", "fbo2screen");
    glUseProgram(fbo2screen_prog);
    uniforms.fbo2screen.tex_sobj = glGetUniformLocation(fbo2screen_prog, "tex_sobj");
    uniforms.fbo2screen.tex_snoobj = glGetUniformLocation(fbo2screen_prog, "tex_snoobj");
    uniforms.fbo2screen.tex_sb = glGetUniformLocation(fbo2screen_prog, "tex_sb");
    uniforms.fbo2screen.is_using_df = glGetUniformLocation(fbo2screen_prog, "is_using_df");
    
    // Program to draw the depth map
    depth_prog = createProgram("depth_vertex.vs.glsl", "depth_fragment.fs.glsl", "depth");
    uniforms.depth.mvp = glGetUniformLocation(depth_prog, "mvp");
    
    // Program to draw the skybox
    skybox_prog = createProgram("skybox_vertex.vs.glsl", "skybox_fragment.fs.glsl", "skybox");
    glUseProgram(skybox_prog);
    
    uniforms.skybox.view_matrix = glGetUniformLocation(skybox_prog, "view_matrix");
    uniforms.skybox.inv_vp_matrix = glGetUniformLocation(skybox_prog, "inv_vp_matrix");
    uniforms.skybox.tex_cubemap = glGetUniformLocation(skybox_prog, "tex_cubemap");
    
    // Program to draw the model
    render_prog = createProgram("vertex.vs.glsl", "fragment.fs.glsl", "render");
    glUseProgram(render_prog);
    
    uniforms.render.mv_matrix = glGetUniformLocation(render_prog, "mv_matrix");
    uniforms.render.model_matrix = glGetUniformLocation(render_prog, "model_matrix");
    uniforms.render.view_matrix = glGetUniformLocation(render_prog, "view_matrix");
    uniforms.render.proj_matrix = glGetUniformLocation(render_prog, "proj_matrix");
    uniforms.render.is_quad = glGetUniformLocation(render_prog, "is_quad");
    uniforms.render.light_mvp_matrix = glGetUniformLocation(render_prog, "light_mvp_matrix");
    uniforms.render.tex_cubemap = glGetUniformLocation(render_prog, "tex_cubemap");
    uniforms.render.tex_shadow = glGetUniformLocation(render_prog, "tex_shadow");
    uniforms.render.is_shadow = glGetUniformLocation(render_prog, "is_shadow");
    
    // Fbos
    shadow_fbo = new ShadowFbo("Shadow buffer");
    s_obj = new Sobj("Sobj:quad+shadow-model");
    s_noobj = new Snoobj("Snoobj:quad-model");
    s_b = new Sb("Sb:model+skybox");
    
    // Flag required by UI
    fb2screen_flag = new MaxRangeInt(4);
    
    // Load Skybox
    cube_map = new CubeMap();
    tex_envmap = cube_map->loadTexture();    // We've hardcoded the texture location in this function.
    
    // Load Ladybug
    /*mesh = new Mesh("Nanosuit",
                    "nanosuit.obj",
                    vec3(-10.0, -13.0, -8.0),
                    vec3(0.5, 0.35, 0.5),
                    glm::quat(vec3(0.0)));*/
    
    // Load quad
    quad = new Quad(vec3(-20.0, -12.5, 0.0),
                    vec3(1.0, 1.0, 1.0),
                    glm::quat(vec3(0.0, 0.0, 0.0)));
    
    mesh = new Model("lost_empire/lost_empire.obj", "lost_empire/",
                      glm::vec3(0.0, 0.0, 0.0),
                      glm::quat(glm::vec3(radians(0.0), radians(90.0), radians(0.0))),
                      glm::vec3(0.15, 0.15, 0.15));
    mesh->log();
    
    
    My_Reshape(1440, 900);
    shadow_fbo->log();
    s_obj->log();
    s_noobj->log();
    s_b->log();
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
    mat4 light_proj_matrix = ortho(-shadow_range, shadow_range, -shadow_range, shadow_range, 0.0f, 170.0f); // far plane must be far enough.
    mat4 light_view_matrix = lookAt(vec3(-31.75, 26.05, -97.72), vec3(-20.0f, -23.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
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
    glUseProgram(render_prog);
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
    mesh->draw(uniforms, view_matrix, proj_matrix, shadow_sbpv_matrix, true);
    s_obj->afterMakingStencilMap();
    
    // == Quad + Shadow - Model == //
    s_obj->beforeDrawQuadShadow();
    mesh->draw(uniforms, view_matrix, proj_matrix, shadow_sbpv_matrix, true);
    quad->draw(uniforms, view_matrix, proj_matrix, shadow_sbpv_matrix, true);
    s_obj->afterDrawQuadShadow();
    
    // == Quad - Model == //
    s_noobj->bindStencilBuffer(s_obj->depth_stencil_map, viewport_size.width, viewport_size.height);
    s_noobj->beforeDrawQuad();
    mesh->draw(uniforms, view_matrix, proj_matrix, shadow_sbpv_matrix, false);
    quad->draw(uniforms, view_matrix, proj_matrix, shadow_sbpv_matrix, false);
    s_noobj->afterDrawQuad();
    
    // == Model + Skybox == //
    s_b->beforeDrawSkyboxModel();
    // - Draw skybox - //
    glUseProgram(skybox_prog);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);
    glUniform1i(uniforms.skybox.tex_cubemap, 2);
    
    cube_map->draw(uniforms, view_matrix, inv_vp_matrix);
    
    // - Draw the model - //
    glUseProgram(render_prog);
    
    // Bind textures
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadow_fbo->depth_map);
    glUniform1i(uniforms.render.tex_shadow, 1);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);
    glUniform1i(uniforms.render.tex_cubemap, 2);
    
    // draw mesh
    mesh->draw(uniforms, view_matrix, proj_matrix, shadow_sbpv_matrix, false);
    
    s_b->afterDrawSkyboxModel();
    
    // == FBO to screen == //
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glUseProgram(fbo2screen_prog);
    glBindVertexArray(shadow_fbo->window_vao);
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    
    switch(fb2screen_flag->get()){
        case 0:
            // differential rendering
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, s_b->texture_map);
            glUniform1i(uniforms.fbo2screen.tex_sb, 1);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, s_obj->texture_map);
            glUniform1i(uniforms.fbo2screen.tex_sobj, 2);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, s_noobj->texture_map);
            glUniform1i(uniforms.fbo2screen.tex_snoobj, 3);
            glUniform1i(uniforms.fbo2screen.is_using_df, 1);
            break;
        case 1:
            // Sobj
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, s_obj->texture_map);
            glUniform1i(uniforms.fbo2screen.tex, 4);
            glUniform1i(uniforms.fbo2screen.is_using_df, 0);
            break;
        case 2:
            // Snoobj
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, s_noobj->texture_map);
            glUniform1i(uniforms.fbo2screen.tex, 4);
            glUniform1i(uniforms.fbo2screen.is_using_df, 0);
            break;
        case 3:
            // Sb
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, s_b->texture_map);
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
        printf("w");
    }
    else if (key == 's') {
        camera.moveBack();
        printf("s");
    }
    else if (key == 'a') {
        camera.moveLeft();
        printf("a");
    }
    else if (key == 'd') {
        camera.moveRight();
        printf("d");
    }
    else if (key == 'z') {
        camera.moveUp();
        printf("Z");
    }
    else if (key == 'x') {
        camera.moveDown();
        printf("X");
    }
    else if(key == 'i'){
        fb2screen_flag->add(1);
    }
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
    glewInit();H
#endif
    dumpInfo();
    My_Init();
    
    // Create a menu and bind it to mouse right button.
    int menu_main = glutCreateMenu(My_Menu);
    int menu_timer = glutCreateMenu(My_Menu);
    
    glutSetMenu(menu_main);
    glutAddSubMenu("Timer", menu_timer);
    glutAddMenuEntry("Exit", MENU_EXIT);
    
    glutSetMenu(menu_timer);
    glutAddMenuEntry("Start", MENU_TIMER_START);
    glutAddMenuEntry("Stop", MENU_TIMER_STOP);
    
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

