#include <stdio.h>
#include "poolgame.h"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <math.h>
#include <iostream>
using namespace std;

ball::ball(int slc, int stk){
	slices = slc;
	stacks = stk;
}

void ball::ballMesh(){
	vertexNum = 2 * (slices + 1) * stacks;
	vertex = (glm::vec3*)malloc(sizeof(glm::vec3)*vertexNum);
	normal = (glm::vec3*)malloc(sizeof(glm::vec3)*vertexNum);
	texcoord = (glm::vec2*)malloc(sizeof(glm::vec2)*vertexNum);
	float stepsk = M_PI/stacks;
	float stepsl = 2*M_PI/slices;
	float theta;
	float phi;

	for (int i = 0; i < stacks; i++) {
		theta = stepsk*i;
		for (int j = 0; j < slices; j++) {
			phi = stepsl*j;		    
		    glm::vec3 v1 = glm::vec3(sin(theta) * sin(phi), cos(theta), sin(theta) * cos(phi));
		    glm::vec3 v2 = glm::vec3(sin(theta + stepsk) * sin(phi), cos(theta + stepsk), sin(theta + stepsk) * cos(phi));
		    *(vertex++) = v1;
		    *(vertex++) = v2;
		    *(normal++) = v1;
		    *(normal++) = v2;
		    *(texcoord++) = glm::vec2(-(atan2(v1[0],v1[1])/M_PI+1.0)/2, -acos(v1[2])/M_PI);
		    *(texcoord++) = glm::vec2(-(atan2(v2[0],v2[1])/M_PI+1.0)/2, -acos(v2[2])/M_PI);
	  }
	}
	vertex = vertex - vertexNum;
	normal = normal - vertexNum;
	texcoord = texcoord - vertexNum;
}

table::table(float l, float w, float h){
	length = l;
	width = w;
	height = h;
}

void table::quad(int a, int b, int c, int d){
	glm::vec3 u = points[b] - points[a];
	glm::vec3 v = points[c] - points[b];
	glm::vec3 n = glm::normalize(glm::cross(u,v));

	*(normal+id) = n; *(vertex+id) = points[a]; *(texcoord+id) = tex[0]; id++; 
	*(normal+id) = n; *(vertex+id) = points[b]; *(texcoord+id) = tex[1]; id++; 
	*(normal+id) = n; *(vertex+id) = points[c]; *(texcoord+id) = tex[2]; id++; 
	*(normal+id) = n; *(vertex+id) = points[a]; *(texcoord+id) = tex[0]; id++; 
	*(normal+id) = n; *(vertex+id) = points[c]; *(texcoord+id) = tex[2]; id++; 
	*(normal+id) = n; *(vertex+id) = points[d]; *(texcoord+id) = tex[3]; id++; 
}

void table::tableMesh(){
	vertexNum = 6*5;
	id = 0;
	vertex = (glm::vec3*)malloc(sizeof(glm::vec3)*vertexNum);
	normal = (glm::vec3*)malloc(sizeof(glm::vec3)*vertexNum);
	texcoord = (glm::vec2*)malloc(sizeof(glm::vec2)*vertexNum);
	
	points = (glm::vec3*)malloc(sizeof(glm::vec3)*8);
	*(points++) = glm::vec3(-1.0,-1.0,1.0);
	*(points++) = glm::vec3(1.0,-1.0,1.0);
	*(points++) = glm::vec3(1.0,1.0,1.0);
	*(points++) = glm::vec3(-1.0,1.0,1.0);
	*(points++) = glm::vec3(1.0,-1.0,-1.0);
	*(points++) = glm::vec3(-1.0,-1.0,-1.0);
	*(points++) = glm::vec3(-1.0,1.0,-1.0);
	*(points++) = glm::vec3(1.0,1.0,-1.0);
	points = points - 8;

	cout<<"hi, i am ok"<<endl;
	tex[0] = glm::vec2(0.0,0.0);
	tex[1] = glm::vec2(1.0,0.0);
	tex[2] = glm::vec2(1.0,1.0);
	tex[3] = glm::vec2(0.0,1.0);
	cout<<"hi, i am ok"<<endl;

	quad(1,0,3,2);
	quad(0,5,6,3);
	quad(5,4,7,6);
	quad(4,1,2,7);
	quad(0,1,4,5);

	// quad(0,1,2,3);
	// quad(1,4,7,2);
	// quad(4,5,6,7);
	// quad(5,0,3,6);
	// quad(0,5,4,1);

}

GLuint setVBO(glm::vec3 *points, glm::vec3 *normals, glm::vec2 *texcoord, int vertexNum){
	GLuint vbo;
	int sizePoints = sizeof(glm::vec3)*vertexNum;
	int sizeTexcoord = sizeof(glm::vec2)*vertexNum;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizePoints*2+sizeTexcoord, NULL, GL_STATIC_DRAW);
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizePoints, points );
	glBufferSubData( GL_ARRAY_BUFFER, sizePoints, sizePoints, normals);
	glBufferSubData( GL_ARRAY_BUFFER, 2*sizePoints, sizeTexcoord, texcoord);
	return vbo;
}

// GLuint setIBO(GLushort *element, int indexNum){
// 	GLuint ibo;
// 	glGenBuffers(1, &ibo);
// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
// 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::vec3)*indexNum, element, GL_STATIC_DRAW);
// 	return ibo;	
// }

quaternion::quaternion(int w, int h)
{
	screen_width = w;
	screen_height = h;
}

// Compute virtual trackball vector.
glm::vec3 quaternion::get_trackball_vec(int x, int y)
{
	glm::vec3 pvec = glm::vec3(1.0*x/screen_width*2 -1.0, 1.0*y/screen_height*2 -1.0, 0);
	pvec.y = -pvec.y;
	float plength = pvec.x*pvec.x + pvec.y*pvec.y;
	if (plength <= 1.0)
	{
		pvec.z = sqrt(1-plength);
	}				
	else pvec = glm::normalize(pvec);
	return pvec;
}


// Quaternions multiplication.
glm::vec4 quaternion::quaMul(glm::vec4 a, glm::vec4 b)
{
  float angA = a.x;
  float angB = b.x;
  glm::vec3 axisA = glm::vec3(a.y, a.z, a.w);
  glm::vec3 axisB = glm::vec3(b.y, b.z, b.w);
  return glm::vec4(angA*angB-glm::dot(axisA,axisB), glm::vec3(angA*axisB+angB*axisA+glm::cross(axisA, axisB)));
}

glm::mat4 quaternion::quaRot()
{
  float rotAng = 0;
  glm::vec3 rotAxis = glm::vec3(1.0f);

	if (cur_mx != last_mx || last_my != cur_my)
	{
		glm::vec3 va = get_trackball_vec(last_mx, last_my);
		glm::vec3 vb = get_trackball_vec(cur_mx, cur_my);
		rotAng = acos(glm::min(1.0f, glm::dot(va, vb)));
		rotAxis = glm::normalize(glm::cross(va, vb));
		last_mx = cur_mx;
		last_my = cur_my;
	}
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotAng, rotAxis);
	return rot;
}

void setShotVelocity::setVelocity()
{
	if (ballID >= 0)
	{
		velocity[0] = cur_my - last_my;
		velocity[1] = 0.0;
		velocity[2] = -(cur_mx - last_mx);
		angVel = glm::cross(glm::vec3(0.0,1.0,0.0),velocity);
	}	
	else 
	{
		velocity = glm::vec3(0.0f);
		angVel = glm::vec3(0.0f);
	}
}


void ballPhysics::setInitBallPos()
{
	float d = 2;
	ballPos[0] = glm::vec3(0.0, 0.0, 16.0);
	translate[0] = glm::translate(glm::mat4(1.0f), ballPos[0]);
	rotate[0] = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0,0.0,0.0));

	int k = 1;
	for (int i=0; i<5; i++)
	{
		for (int j=0; j<5-i; j++)
		{
			ballPos[k][0] = j*d*cos(M_PI/3)-2*d+i*d;
			ballPos[k][1] = 0.0;
			ballPos[k][2] = j*d*sin(M_PI/3)-16.0;
			translate[k] = glm::translate(glm::mat4(1.0f), ballPos[k]);
			rotate[k] = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0,0.0,0.0));
			k++;
		}
	}
}

void ballPhysics::setInitVelocity()
{
	for (int i=0; i<16; i++)
	{
		ballVel[i] = glm::vec3(0.0f);
		ballAngVel[i] = glm::vec3(0.0f);
	}
}

void ballPhysics::collisionWall(int i)
{
	ballVel[i] *= glm::vec3(wall, 0.0, -wall);
	ballAngVel[i] = 2.5f*glm::cross(glm::vec3(0.0,1.0,0.0),ballVel[i]);
}

void ballPhysics::collisionBall(int i, int j)
{
	glm::vec3 n = glm::normalize(ballPos[j] - ballPos[i]);
	glm::vec3 niVel = glm::dot(ballVel[i],n)*n;
	glm::vec3 njVel = glm::dot(ballVel[j],-n)*(-n);
	glm::vec3 tiVel = ballVel[i] - niVel;
	glm::vec3 tjVel = ballVel[j] - njVel;
	ballVel[i] = njVel + tiVel;
	ballVel[j] = niVel + tjVel;
	ballAngVel[i] = 2.5f*glm::cross(glm::vec3(0.0,1.0,0.0),ballVel[i]);
}

void ballPhysics::setPocketCenter()
{
	float l = 24;
	float h = 24;
	int k=0;
	for (int i=0; i<2; i++)
	{
		for (int j=0; j<3; j++)
		{
			pocket[k][0] = i*h - h/2;
			pocket[k][1] = 0.0;
			pocket[k][2] = j*l - l;
			k++;
		}
	}
}

void ballPhysics::pocketTest(int i)
{
	for (int j=0; j<6; j++)
	{	
		float dist = glm::distance(ballPos[i],pocket[j]);
		if (dist <= 1.85)
		{
			ballPos[i] = glm::vec3(0.0, -i*5.0, 0.0);
			// ballVel[i] = glm::vec3(0.0f);
		}
	}
}





