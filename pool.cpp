#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#include <GL/glew.h>
/* Using the GLUT library for the base windowing setup */
#include <GL/freeglut.h>
/* Use GLM mathematics library*/
#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
/* Use shader_utils.h for create and link shaders */
#include "shader_utils.h"
#include <iostream>
#include <string>
using namespace std;
#include <SOIL/SOIL.h>


// Global variables
int screen_width=700, screen_height=700;
GLuint ball_vbo, table_vbo;
GLuint ball_vao, table_vao;
GLuint select_ball_vao;

GLuint program;
GLuint select_program;
GLint attribute_v_coord;
GLint attribute_v_normal;
GLint attribute_v_texcoord;
GLint uniform_mv, uniform_pro, uniform_m, uniform_v;
GLint uniform_lightPos, uniform_ambient, uniform_diffuse, uniform_specular, uniform_shine;
GLint uniform_ballID;
GLint uniform_select_pro, uniform_select_mv, uniform_select_m, uniform_select_v;
GLint attribute_v_select_coord;

GLuint mytexture_id[17];
GLuint uniform_mytexture;

#define ORTHO
glm::mat4 model = glm::mat4(1.0f);

#ifdef PERS
glm::mat4 view = glm::lookAt(glm::vec3(0.0, 5.0, -5.0), glm::vec3(0.0, 0.0, -12.0), glm::vec3(0.0, 1.0, 0.0));
glm::mat4 project = glm::perspective(45.0f, 1.0f*screen_width/screen_height, 0.1f, 80.0f);
#endif

#ifdef ORTHO
glm::mat4 view = glm::lookAt(glm::vec3(0.0, 3.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0));
glm::mat4 project = glm::ortho(30.0f,-30.0f,15.0f,-15.0f,0.1f,6.0f);
#endif

glm::mat4 modelview = view*model;


struct filename {
    // const char* texture_filename;    
    const char* vshader_filename;
    const char* fshader_filename;
};
struct filename files = {"poolgame.v.glsl","poolgame.f.glsl"};
struct filename select_files = {"selection.v.glsl","selection.f.glsl"};

string texture_files[17];

ball myBall(30, 30);
table myTable(48.0, 24.0, 1.0);
ballPhysics myBallPhy;
// glm::vec3 myInitBallPos[16];
// glm::mat4 myInitTranslate[16];

// Initial vbo and program
int init_resources()
{
	printf("init_resources: %s, %s\n", files.vshader_filename, files.fshader_filename);
  printf("init_resources: %s, %s\n", select_files.vshader_filename, select_files.fshader_filename);

  myBall.ballMesh();
	ball_vbo = setVBO(myBall.vertex, myBall.normal, myBall.texcoord, myBall.vertexNum);

  myTable.tableMesh();
	table_vbo = setVBO(myTable.vertex, myTable.normal, myTable.texcoord, myTable.vertexNum);

  myBallPhy.setInitBallPos();
  myBallPhy.setInitVelocity();
  myBallPhy.setPocketCenter();
  cout << "pocket[0]: " << myBallPhy.pocket[0][0] << ", " << myBallPhy.pocket[0][1] <<", "<<myBallPhy.pocket[0][2]<<endl;

  texture_files[0] = "BallCue.jpg";
  for(int i=1; i<16; i++)
  { 
    texture_files[i] = "Ball"+to_string(i)+".jpg";
  }
  texture_files[16] = "pool_table.png";

  cout<<"textures file[0]:"<<texture_files[0]<<endl;

    // Load texture image using SOIL//
  for (int i=0; i<17; i++)
  {
    mytexture_id[i] = SOIL_load_OGL_texture
      (
       texture_files[i].c_str(),
       SOIL_LOAD_AUTO,
       SOIL_CREATE_NEW_ID,
       SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS
       );
      if (mytexture_id[i] == 0)
        cerr << "SOIL loading error: '" << SOIL_last_result() << "' (" << texture_files[i] << ")" << endl;    
  }  


    // Create and link shader//
    GLint link_ok = GL_FALSE;
    GLuint vs, fs;
    if ((vs = create_shader(files.vshader_filename, GL_VERTEX_SHADER))   == 0) return 0;
    if ((fs = create_shader(files.fshader_filename, GL_FRAGMENT_SHADER)) == 0) return 0;

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
      fprintf(stderr, "glLinkProgram:");
      print_log(program);
      return 0;
    }

    if ((vs = create_shader(select_files.vshader_filename, GL_VERTEX_SHADER))   == 0) return 0;
    if ((fs = create_shader(select_files.fshader_filename, GL_FRAGMENT_SHADER)) == 0) return 0;

    select_program = glCreateProgram();
    glAttachShader(select_program, vs);
    glAttachShader(select_program, fs);
    glLinkProgram(select_program);
    glGetProgramiv(select_program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
      fprintf(stderr, "glLinkProgram:");
      print_log(select_program);
      return 0;
    }

    const char* attribute_name;
    attribute_name = "v_coord";
    attribute_v_coord = glGetAttribLocation(program, attribute_name);

    attribute_name = "v_normal";
    attribute_v_normal = glGetAttribLocation(program, attribute_name);

    attribute_name = "v_texcoord";
    attribute_v_texcoord = glGetAttribLocation(program, attribute_name);    
    
    const char* uniform_name;
    uniform_name = "mv";
    uniform_mv = glGetUniformLocation(program, uniform_name);

    uniform_name = "m";
    uniform_m = glGetUniformLocation(program, uniform_name);
    uniform_name = "v";
    uniform_v = glGetUniformLocation(program, uniform_name);

    uniform_name = "mytexture";
    uniform_mytexture = glGetUniformLocation(program, uniform_name);

    uniform_name = "pro";
    uniform_pro = glGetUniformLocation(program, uniform_name);

    uniform_name = "lightPos";
    uniform_lightPos = glGetUniformLocation(program, uniform_name);

    uniform_name = "ambient";
    uniform_ambient = glGetUniformLocation(program, uniform_name);

    uniform_name = "diffuse";
    uniform_diffuse = glGetUniformLocation(program, uniform_name);

    uniform_name = "specular";
    uniform_specular = glGetUniformLocation(program, uniform_name);

    uniform_name = "shine";
    uniform_shine = glGetUniformLocation(program, uniform_name);


    // uniform_select_mv = glGetUniformLocation(select_program,"select_mv");
    uniform_select_m = glGetUniformLocation(select_program,"select_m");
    uniform_select_v = glGetUniformLocation(select_program,"select_v");

    uniform_select_pro = glGetUniformLocation(select_program,"select_pro");
    uniform_ballID = glGetUniformLocation(select_program, "ballID");
    attribute_v_select_coord = glGetAttribLocation(select_program,"select_v_coord");

    return 1;
}

void initVAO()
{
	glGenVertexArrays(1, &ball_vao);
	glBindVertexArray(ball_vao);
	glBindBuffer(GL_ARRAY_BUFFER, ball_vbo);
  glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(attribute_v_normal);
  glVertexAttribPointer(attribute_v_normal, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(glm::vec3)*myBall.vertexNum));	
  glEnableVertexAttribArray(attribute_v_texcoord);
  glVertexAttribPointer(attribute_v_texcoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(2*sizeof(glm::vec3)*myBall.vertexNum)); 
  glBindVertexArray(0);

	glGenVertexArrays(1, &table_vao);
	glBindVertexArray(table_vao);  
	glBindBuffer(GL_ARRAY_BUFFER, table_vbo);
  glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(attribute_v_normal);
  glVertexAttribPointer(attribute_v_normal, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(glm::vec3)*myTable.vertexNum));  
  glEnableVertexAttribArray(attribute_v_texcoord);
  glVertexAttribPointer(attribute_v_texcoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(glm::vec3)*myTable.vertexNum*2)); 
	glBindVertexArray(0);

  glGenVertexArrays(1, &select_ball_vao);
  glBindVertexArray(select_ball_vao);
  glBindBuffer(GL_ARRAY_BUFFER, ball_vbo);
  glEnableVertexAttribArray(attribute_v_select_coord);
  glVertexAttribPointer(attribute_v_select_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void initLight()
{
  glm::vec4 lightPos = glm::vec4(0.0, 10.0, 0.0, 1.0);
  glm::vec4 lightAmbient = glm::vec4(0.5, 0.5, 0.5, 1.0);
  glm::vec4 lightDiffuse = glm::vec4(1.0, 1.0, 1.0, 1.0);
  glm::vec4 lightSpecular = glm::vec4(1.0, 1.0, 1.0, 1.0);

  glm::vec4 materialAmbient = glm::vec4(1.0, 1.0, 1.0, 1.0);
  glm::vec4 materialDiffuse = glm::vec4( 1.0, 1.0, 1.0, 1.0 );
  glm::vec4 materialSpecular = glm::vec4( 1.0, 1.0, 1.0, 1.0 );
  float materialShine = 100;

  glm::vec4 ambient = lightAmbient * materialAmbient;
  glm::vec4 diffuse = lightDiffuse * materialDiffuse;
  glm::vec4 specular = lightSpecular * materialSpecular;

  glUniform4fv(uniform_ambient, 1, glm::value_ptr(ambient));
  glUniform4fv(uniform_diffuse, 1, glm::value_ptr(diffuse));
  glUniform4fv(uniform_specular, 1, glm::value_ptr(specular));
  glUniform4fv(uniform_lightPos, 1, glm::value_ptr(lightPos));
  glUniform1f(uniform_shine, materialShine);
}

quaternion myQuater(screen_width, screen_height);
void quaRotate()
{
  glm::mat4 rot = myQuater.quaRot();
  model = rot*model;
  glutPostRedisplay();
}

void ballSlide()
{
  for (int i = 0; i<16; i++)
  { 
    glm::vec3 p = myBallPhy.ballPos[i];

    if (abs(p[0]) >= myTable.width/2-1)
    {
      myBallPhy.wall = -1.0;
      myBallPhy.collisionWall(i);
    }
    else if (abs(p[2]) >= myTable.length/2-1)
    {
      myBallPhy.wall = 1.0;
      myBallPhy.collisionWall(i);
    }

    for (int j=0; j<16; j++)
    {
      float dist = glm::distance(myBallPhy.ballPos[j], p);
      if ((j != i) && (dist <= 1.85))
      {
        myBallPhy.collisionBall(i,j);
      }
    }

    myBallPhy.pocketTest(i);

    glm::vec3 v = myBallPhy.ballVel[i];
    glm::vec3 ds = v * myBallPhy.dt;
    glm::vec3 nextVel = v * (1+myBallPhy.dv);

    glm::vec3 w = myBallPhy.ballAngVel[i];
    glm::vec3 ddw = w * myBallPhy.dt;
    float ang = glm::length(ddw);
    glm::vec3 nw = glm::normalize(ddw);
    glm::vec3 nextAngVel = w * (1+myBallPhy.dw);

    if (glm::length(v) && ang)
    {
      myBallPhy.ballPos[i] += ds;
      myBallPhy.translate[i] = glm::translate(glm::mat4(1.0f), myBallPhy.ballPos[i]);
      myBallPhy.rotate[i] *= glm::rotate(glm::mat4(1.0f), ang, nw);
      if (glm::dot(nextVel,v) < 0.0)
      {
        myBallPhy.ballVel[i] = glm::vec3(0.0f);
        myBallPhy.ballAngVel[i] = glm::vec3(0.0f);
      }  
      else
      {
        myBallPhy.ballVel[i] = nextVel;
        myBallPhy.ballAngVel[i] = nextAngVel;
      }
    }

  }
  glutPostRedisplay();
}

void onDisplay()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
  glUniformMatrix4fv(uniform_pro, 1, GL_FALSE, glm::value_ptr(project));
  glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view));

  glActiveTexture(GL_TEXTURE0);
  glUniform1i(uniform_mytexture, /*GL_TEXTURE*/0);

  initLight();
//  quaRotate();
  ballSlide();

  glBindVertexArray(ball_vao);
  for (int i=0; i<16; i++)
  {
    glBindTexture(GL_TEXTURE_2D, mytexture_id[i]);
    model = myBallPhy.translate[i]*myBallPhy.rotate[i];
    glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(model));    
    // modelview = view*model*myBallPhy.translate[i]*myBallPhy.rotate[i];
    // glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(modelview));    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, myBall.vertexNum);
  }

  glBindVertexArray(table_vao);
  glBindTexture(GL_TEXTURE_2D, mytexture_id[16]);
  model = glm::scale(glm::mat4(1.0f), glm::vec3(myTable.width/2,myTable.height,myTable.length/2));
  glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(model));   

  // modelview = view*model*glm::scale(glm::mat4(1.0f), glm::vec3(myTable.width/2,myTable.height,myTable.length/2));
  // glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(modelview));   
  glDrawArrays(GL_TRIANGLES, 0, myTable.vertexNum);

	glutSwapBuffers();
}

setShotVelocity myShot;
void renderSelection()
{
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glUseProgram(select_program);
  glUniformMatrix4fv(uniform_select_pro, 1, GL_FALSE, glm::value_ptr(project));
  glUniformMatrix4fv(uniform_select_v, 1, GL_FALSE, glm::value_ptr(view));

  glBindVertexArray(select_ball_vao);
  for (int i=0; i<16; i++)
  {
    // printf("ball: %d\n", i+1);
    model = myBallPhy.translate[i]*myBallPhy.rotate[i];
    glUniformMatrix4fv(uniform_select_m, 1, GL_FALSE, glm::value_ptr(model));   
    // modelview = view*model*myBallPhy.translate[i]*myBallPhy.rotate[i];
    // glUniformMatrix4fv(uniform_select_mv, 1, GL_FALSE, glm::value_ptr(modelview));   

    glUniform1i(uniform_ballID, i+1); 
    glDrawArrays(GL_TRIANGLE_STRIP, 0, myBall.vertexNum);
  }

  glClearColor(0.0, 0.0, 0.0, 1.0);
}


void onReshape(int width, int height)
{
	screen_width = width;
	screen_height = height;
	glViewport(0, 0, screen_width, screen_height);
}


void keyfunc( unsigned char key, int x, int y)
{
  switch( key ) {
    case 27:  //escape
      exit ( 0 );
      break;
    // case 'k':

  }
}

void processSelection(int x, int y)
{
  unsigned char res[4];
  renderSelection();
  glReadPixels(x, screen_height-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);
  myShot.ballID = res[0]-1;
  myShot.setVelocity();
  if (myShot.ballID != -1)
  {
    myBallPhy.ballVel[myShot.ballID] = myShot.velocity; 
    myBallPhy.ballAngVel[myShot.ballID] = myShot.angVel; 
  }
  cout << "myBallPhy.ballVel[myShot.ballID]: " << myShot.velocity[0] << ", " << myShot.velocity[1] << endl;
  printf("Ball: %d\n", myShot.ballID);
}


void onMouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    myShot.last_mx = x;
    myShot.last_my = y;
    myShot.tracking = true;
  }
  else myShot.tracking = false;

  // if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  // {
  //   myQuater.last_mx = myQuater.cur_mx = x;
  //   myQuater.last_my = myQuater.cur_my = y;
  //   myQuater.trackball_on = true; 
  // }
  // else myQuater.trackball_on = false;
}


void onMotion(int x, int y)
{
  if (myShot.tracking)
  {
    myShot.cur_mx = x;
    myShot.cur_my = y;
    processSelection(x,y);
    printf("cur_mx: %d, cur_my: %d\n", myShot.cur_mx, myShot.cur_my);
  }

  // if (myQuater.trackball_on)
  // {
  //   myQuater.cur_mx = x;
  //   myQuater.cur_my = y;
  // }

}


void free_resources()
{
	glDeleteProgram(program);
  glDeleteBuffers(1, &ball_vbo);
  glDeleteBuffers(1, &table_vbo);
}

int main(int argc, char* argv[]) 
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(screen_width, screen_height);
  glutCreateWindow("Pool Game");


  GLenum glew_status = glewInit();
  if (glew_status != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
    return EXIT_FAILURE;
  }

  if (!GLEW_VERSION_2_0) {
    fprintf(stderr, "Error: your graphic card does not support OpenGL 2.0\n");
    return EXIT_FAILURE;
  }

  if (init_resources()) {
    initVAO();
    glutDisplayFunc(onDisplay);
    // glutDisplayFunc(renderSelection);
    glutReshapeFunc(onReshape);
    glutMouseFunc(onMouse);
    glutMotionFunc(onMotion);
    glutKeyboardFunc (keyfunc);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutMainLoop();
  }

  free_resources();
  return EXIT_SUCCESS;
}