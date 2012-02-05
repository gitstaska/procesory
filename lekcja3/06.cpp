// Triangle.cpp
// Our first OpenGL program that will just draw a triangle on the screen.
#include <Windows.h>
#include <GLTools.h>            // OpenGL toolkit
#include <GLFrame.h>
#include <GLFrustum.h>
#include <StopWatch.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#ifdef __APPLE__
#include <glut/glut.h>          // OS X version of GLUT
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>            // Windows FreeGlut equivalent
#endif

CStopWatch timer;

GLuint shader;
GLint MVPMatrixLocation,MVMatrixLocation,normalMatrixLocation;
GLFrame frame;
GLFrustum frustum;
GLint shaderPositionLocation;
GLint shaderColorLocation;
GLint shaderAngleLocation;
GLint shaderAttenuation0Location;
GLint shaderAttenuation1Location;
GLint shaderAttenuation2Location;
GLint ambientColor;
GLint diffuseColor;
GLint specularColor;
GLint specularExponent;
GLint ambientLight;

///////////////////////////////////////////////////////////////////////////////
// Window has changed size, or has just been created. In either case, we need
// to use the window dimensions to set the viewport and the projection matrix.

void ChangeSize(int w, int h) {
    glViewport(0, 0, w, h);
	frustum.SetPerspective(45.0f,w/h,25,100);
}


///////////////////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering context.
// This is the first opportunity to do any OpenGL related tasks.

void SetupRC() {
    // Blue background
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

//	timer.Reset();
    shader = gltLoadShaderPairWithAttributes("06.vp", "06.fp",
            2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");
    fprintf(stdout, "GLT_ATTRIBUTE_VERTEX : %d\nGLT_ATTRIBUTE_COLOR : %d \n",
            GLT_ATTRIBUTE_VERTEX, GLT_ATTRIBUTE_COLOR);
 shaderPositionLocation = glGetUniformLocation(shader, "light1.position");
 shaderColorLocation = glGetUniformLocation(shader, "light1.color");
 shaderAngleLocation = glGetUniformLocation(shader, "light1.angle");
 shaderAttenuation0Location = glGetUniformLocation(shader, "light1.attenuation0");
 shaderAttenuation1Location = glGetUniformLocation(shader, "light1.attenuation1");
 shaderAttenuation2Location = glGetUniformLocation(shader, "light1.attenuation2");
 ambientColor = glGetUniformLocation(shader, "material.ambientColor");
 diffuseColor = glGetUniformLocation(shader, "material.diffuseColor");
 specularColor = glGetUniformLocation(shader, "material.specularColor");
 specularExponent = glGetUniformLocation(shader, "material.specularExponent");
 


 	MVPMatrixLocation=glGetUniformLocation(shader,"MVPMatrix");
	MVMatrixLocation=glGetUniformLocation(shader,"MVMatrix");
	normalMatrixLocation=glGetUniformLocation(shader,"normalMatrix");
	if(MVPMatrixLocation==-1)
	{
		fprintf(stderr,"uniform MVPMatrix could not be found\n");
	}
	glEnable(GL_DEPTH_TEST);
}

void SetUpFrame(GLFrame &frame,const M3DVector3f origin,
				const M3DVector3f forward,
				const M3DVector3f up) {
					frame.SetOrigin(origin);
					frame.SetForwardVector(forward);
					M3DVector3f side,oUp;
					m3dCrossProduct3(side,forward,up);
					m3dCrossProduct3(oUp,side,forward);
					frame.SetUpVector(oUp);
					frame.Normalize();
}

void TriangleFace(M3DVector3f a, M3DVector3f b, M3DVector3f c) {
   M3DVector3f normal, bMa, cMa;
   m3dSubtractVectors3(bMa, b, a);
   m3dSubtractVectors3(cMa, c, a);
   m3dCrossProduct3(normal, bMa, cMa);
   m3dNormalizeVector3(normal);
   glVertexAttrib3fv(GLT_ATTRIBUTE_NORMAL, normal);
   glVertex3fv(a);
   glVertex3fv(b);
   glVertex3fv(c);
}

void myTriangleFace(float f11,float f12,float f13,float f21,float f22,float f23,float f31,float f32,float f33)
{
	M3DVector3f face1,face2,face3;
	face1[0]=f11;
	face1[1]=f12;
	face1[2]=f13;

	face2[0]=f21;
	face2[1]=f22;
	face2[2]=f23;

	face3[0]=f31;
	face3[1]=f32;
	face3[2]=f33;
	TriangleFace(face1,face2,face3);
}
float ico_vertices[3 * 12] = {
      0., 0., -0.9510565162951536,
      0., 0., 0.9510565162951536,
      -0.85065080835204, 0., -0.42532540417601994,
      0.85065080835204, 0., 0.42532540417601994,
      0.6881909602355868, -0.5, -0.42532540417601994,
      0.6881909602355868, 0.5, -0.42532540417601994,
      -0.6881909602355868, -0.5, 0.42532540417601994,
      -0.6881909602355868, 0.5, 0.42532540417601994,
      -0.2628655560595668, -0.8090169943749475, -0.42532540417601994,
      -0.2628655560595668, 0.8090169943749475, -0.42532540417601994,
      0.2628655560595668, -0.8090169943749475, 0.42532540417601994,
      0.2628655560595668, 0.8090169943749475, 0.42532540417601994
      };
int ico_faces[3*20]={
      1 ,			 11 ,			 7 ,
      1 ,			 7 ,			 6 ,
      1 ,			 6 ,			 10 ,
      1 ,			 10 ,			 3 ,
      1 ,			 3 ,			 11 ,
      4 ,			 8 ,			 0 ,
      5 ,			 4 ,			 0 ,
      9 ,			 5 ,			 0 ,
      2 ,			 9 ,			 0 ,
      8 ,			 2 ,			 0 ,
      11 ,			 9 ,			 7 ,
      7 ,			 2 ,			 6 ,
      6 ,			 8 ,			 10 ,
      10 ,			 4 ,			 3 ,
      3 ,			 5 ,			 11 ,
      4 ,			 10 ,			 8 ,
      5 ,			 3 ,			 4 ,
      9 ,			 11 ,			 5 ,
      2 ,			 7 ,			 9 ,
      8 ,			 6 ,			 2 };
 
void drawTriangles(int n_faces, float *vertices, int *faces) {
      for (int i = 0; i < n_faces; i++) {
      glBegin(GL_TRIANGLES);
      TriangleFace(vertices + 3 * faces[3 * i], vertices + 3 * faces[3 * i + 1], vertices + 3 * faces[3 * i + 2]);
      glEnd();
      }
      }
 
void drawSmoothTriangles(int n_faces, float *vertices, int *faces) {
      M3DVector3f normal;
      for (int i = 0; i < n_faces; i++) {
      glBegin(GL_TRIANGLES);
      for(int j=0;j<3;++j) {
      m3dCopyVector3(normal,vertices+3*faces[i*3+j]);
      m3dNormalizeVector3(normal);
      glVertexAttrib3fv(GLT_ATTRIBUTE_NORMAL, normal);
      glVertex3fv(vertices+3*faces[i*3+j]);
      
      }
      glEnd();
	  }}
void LookAt( GLFrame &frame,const M3DVector3f eye,
        const M3DVector3f at,
        const M3DVector3f up) {
			M3DVector3f forward;
			m3dSubtractVectors3(forward, at, eye);
			SetUpFrame(frame, eye, forward, up);
}
///////////////////////////////////////////////////////////////////////////////
// Called to draw scene

void RenderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
float ambient[3]={.0f, .0f, .0f};
 float diffuse[3]={1.0f, 0.0f, 1.0};
 float specular[3]={1.0f, 1.0f, 1.0f};
 float specularExp=9.f;
 float position[3]={-1.f,-10.f,10.f};
 float color[3]={1.f,1.f,1.0f};
 float angle_l=90.f;
 float attenuation0=0.01f;
 float attenuation1=0.01f;
 float attenuation2=0.01f;
	glUseProgram(shader);
	glUniform3fv(ambientColor, 1, ambient);
	glUniform3fv(ambientLight, 1, ambient);
	glUniform3fv(diffuseColor, 1, diffuse);
	glUniform3fv(specularColor, 1, specular);
	glUniform1f(specularExponent, specularExp);
	glUniform3fv(shaderPositionLocation, 1, position);
	glUniform3fv(shaderColorLocation, 1, color);
	glUniform1f(shaderAngleLocation, angle_l);
	glUniform1f(shaderAttenuation0Location, attenuation0);
	glUniform1f(shaderAttenuation1Location, attenuation1);
	glUniform1f(shaderAttenuation2Location, attenuation2);
	GLMatrixStack modelView;
	GLMatrixStack projection;
	GLGeometryTransform geometryPipeline;
	geometryPipeline.SetMatrixStacks(modelView,projection);
	M3DVector3f at={0,0,0};
	M3DVector3f up={0,0,1};
	M3DVector3f eye;
	float angle=timer.GetElapsedSeconds()*3.14f;
	eye[0]=40*cos(angle);
	eye[1]=40*sin(angle);
	eye[2]=30.0f; 
	LookAt(frame,eye,at,up);
	projection.LoadMatrix(frustum.GetProjectionMatrix());
	modelView.PushMatrix();
	M3DMatrix44f mCamera;
	frame.GetCameraMatrix(mCamera);
	modelView.LoadMatrix(mCamera);
	modelView.PushMatrix();
	glPolygonOffset(1.0f, 1.0f);
    glUniformMatrix4fv(MVMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewMatrix());
	glUniformMatrix3fv(normalMatrixLocation,1,GL_FALSE,geometryPipeline.GetNormalMatrix());
	glUniformMatrix4fv(MVPMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
		glBegin(GL_LINES);
	    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.5f, 0.5f, 0.5f);
		for (int i=-10; i<=10; i++)
		{
			glVertex3f(i,-10.0f,0);
			glVertex3f(i,10.0f,0);
			glVertex3f(-10.0f,i,0);
			glVertex3f(10.0f,i,0);
		}
	glEnd();
	glEnable(GL_POLYGON_OFFSET_FILL);
 float ambient2[3]={.0f, .1f, 0.5f};
	glUniform3fv(ambientColor, 1, ambient2);

	glBegin(GL_TRIANGLES);
		for (int i=-10; i<10; i++) for (int j=-10; j<10; j++)
		{
			myTriangleFace(i,j,0,i+1,j,0,i,j+1,0);
			myTriangleFace(i+1,j,0,i+1,j+1,0,i,j+1,0);
		}
	glEnd();
	glDisable(GL_POLYGON_OFFSET_FILL);
	modelView.Translate(10.0f,1.0f,0.0f);
	glUniformMatrix4fv(MVPMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
	glUniformMatrix3fv(normalMatrixLocation,1,GL_FALSE,geometryPipeline.GetNormalMatrix());
	glUniformMatrix4fv(MVPMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
    glBegin(GL_TRIANGLES);
			myTriangleFace(-1,1,0,0,0,2,1,1,0);
			myTriangleFace(-1,-1,0,0,0,2,-1,1,0);
			myTriangleFace(1,-1,0,0,0,2,-1,-1,0);
			myTriangleFace(1,1,0,0,0,2,1,-1,0);
    glEnd();
	modelView.PopMatrix();
	modelView.Rotate(45.0,0,0,1);
	modelView.Translate(0.0,0.0,1.0);
	glUniformMatrix4fv(MVPMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
	glUniformMatrix3fv(normalMatrixLocation,1,GL_FALSE,geometryPipeline.GetNormalMatrix());
	glUniformMatrix4fv(MVPMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
    glBegin(GL_TRIANGLES);
			myTriangleFace(-1,1,0,0,0,2,1,1,0);
			myTriangleFace(-1,-1,0,0,0,2,-1,1,0);
			myTriangleFace(1,-1,0,0,0,2,-1,-1,0);
			myTriangleFace(1,1,0,0,0,2,1,-1,0);
    glEnd();
	modelView.Translate(3.0f,3.0f,0.f);
	glUniformMatrix4fv(MVMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewMatrix());
	glUniformMatrix3fv(normalMatrixLocation,1,GL_FALSE,geometryPipeline.GetNormalMatrix());
	glUniformMatrix4fv(MVPMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
	drawTriangles(20, ico_vertices,ico_faces);
	modelView.Translate(3.0f,3.0f,0.f);
	glUniformMatrix4fv(MVMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewMatrix());
	glUniformMatrix3fv(normalMatrixLocation,1,GL_FALSE,geometryPipeline.GetNormalMatrix());
	glUniformMatrix4fv(MVPMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
	drawSmoothTriangles(20, ico_vertices,ico_faces);
	
	modelView.PopMatrix();
	modelView.PopMatrix();
    glutSwapBuffers();
	glutPostRedisplay();
}


///////////////////////////////////////////////////////////////////////////////
// Main entry point for GLUT based programs

int main(int argc, char* argv[]) {
  

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Triangle");
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }

    SetupRC();

    glutMainLoop();
    return 0;
}
