// Triangle.cpp
// Our first OpenGL program that will just draw a triangle on the screen.
#include <Windows.h>
#include <GLTools.h>            // OpenGL toolkit
#include <GLFrame.h>
#include <GLFrustum.h>
#include <StopWatch.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <iostream>
#include <vector>

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
GLint textureLocation;
GLuint vertex_buffer;
GLuint faces_buffer;
GLuint textureID;
int n_faces;
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
	n_faces = 0;
    // Blue background
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

//	timer.Reset();
    shader = gltLoadShaderPairWithAttributes("07.vp", "07.fp",
            4, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_COLOR, "vColor",
            GLT_ATTRIBUTE_TEXTURE0, "texCoord0", GLT_ATTRIBUTE_NORMAL, "vNormal");
    fprintf(stdout, "GLT_ATTRIBUTE_VERTEX : %d\nGLT_ATTRIBUTE_COLOR : %d \n",
            GLT_ATTRIBUTE_VERTEX, GLT_ATTRIBUTE_COLOR);
 shaderPositionLocation = glGetUniformLocation(shader, "lightPosition");
 ambientColor = glGetUniformLocation(shader, "ambientColor");
 diffuseColor = glGetUniformLocation(shader, "diffuseColor");
 specularColor = glGetUniformLocation(shader, "specularColor");
 //specularExponent = glGetUniformLocation(shader, "material.specularExponent");
 textureLocation = glGetUniformLocation(shader, "texture0");


 	MVPMatrixLocation=glGetUniformLocation(shader,"MVPMatrix");
	MVMatrixLocation=glGetUniformLocation(shader,"MVMatrix");
	normalMatrixLocation=glGetUniformLocation(shader,"normalMatrix");
	if(MVPMatrixLocation==-1)
	{
		fprintf(stderr,"uniform MVPMatrix could not be found\n");
	}
	glEnable(GL_DEPTH_TEST);

	FILE *fvertices=fopen("geode_vertices.dat","r");
	int n_vertices=0;
   if(fvertices==NULL) {
   fprintf(stderr,"cannot open vertices file for reading\n");
   exit(-1);
   }
   char line[120];
   
   std::vector<float> vertices;
   while(fgets(line,120,fvertices)!=NULL) {
   float x,y,z;
   double norm;
   sscanf(line,"%f %f %f",&x,&y,&z);
  
   norm=x*x+y*y+z*z;
   norm=sqrt(norm);
   vertices.push_back(x);
   vertices.push_back(y);
   vertices.push_back(z);
   vertices.push_back(1.0f);
   vertices.push_back(x/norm);
   vertices.push_back(y/norm);
   vertices.push_back(z/norm);
   n_vertices++;
   }
   fclose(fvertices);

   	FILE *ffaces=fopen("geode_faces.dat","r");
   if(ffaces==NULL) {
   fprintf(stderr,"cannot open faces file for reading\n");
   exit(-1);
   }

   std::vector<GLuint> faces;
   while(fgets(line,120,ffaces)!=NULL) {
   GLuint  i,j,k;
   
   if(3!=sscanf(line,"%u %u %u",&i,&j,&k)){
   fprintf(stderr,"error reading faces\n"); 
   exit(-1);
   }
   faces.push_back(i-1);
   faces.push_back(j-1);
   faces.push_back(k-1);
   n_faces++;
   }
   fclose(ffaces);

    glGenBuffers(1,&vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER,n_vertices*sizeof(float)*7,&vertices[0],GL_STATIC_DRAW);
	if(glGetError()!=GL_NO_ERROR) 
   		fprintf(stderr,"error copying vertices\n");
	glVertexAttribPointer(GLT_ATTRIBUTE_VERTEX,4,GL_FLOAT,GL_FALSE,sizeof(float)*7,(const GLvoid *)0);
	glVertexAttribPointer(GLT_ATTRIBUTE_NORMAL,3,GL_FLOAT,GL_FALSE,sizeof(float)*7,(const GLvoid *)(4*sizeof(float)) );
	glEnableVertexAttribArray(GLT_ATTRIBUTE_VERTEX);
	glEnableVertexAttribArray(GLT_ATTRIBUTE_NORMAL);
	glGenBuffers(1,&faces_buffer);
	if(glGetError()!=GL_NO_ERROR) fprintf(stderr,"faces_buffer invalid\n");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,faces_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,n_faces*sizeof(GLuint)*3,&faces[0],GL_STATIC_DRAW);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;
 	pBits = gltReadTGABits("07.tga", &nWidth, &nHeight, &nComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBits);
	glUniform1i(textureLocation, 0);
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

void TriangleFace(M3DVector3f a, float s1, float t1, M3DVector3f b, float s2, float t2, M3DVector3f c, float s3, float t3) {
   M3DVector3f normal, bMa, cMa;
   m3dSubtractVectors3(bMa, b, a);
   m3dSubtractVectors3(cMa, c, a);
   m3dCrossProduct3(normal, bMa, cMa);
   m3dNormalizeVector3(normal);
   glVertexAttrib3fv(GLT_ATTRIBUTE_NORMAL, normal);
	glVertexAttrib2f(GLT_ATTRIBUTE_TEXTURE0, s1, t1);
	glVertex3fv(a);
glVertexAttrib2f(GLT_ATTRIBUTE_TEXTURE0, s2, t2);
glVertex3fv(b);
glVertexAttrib2f(GLT_ATTRIBUTE_TEXTURE0, s3, t3);
   glVertex3fv(c);
}

void myTriangleFace(float f11,float f12,float f13,float s1, float t1,
	float f21,float f22,float f23,float s2,float t2,
	float f31,float f32,float f33,float s3,float t3)
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
	TriangleFace(face1,s1,t1,face2,s2,t2,face3,s3,t3);
}
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
float ambient[4]={.0f, .0f, .0f,1.0f};
 float diffuse[4]={1.0f, 0.0f, 1.0f, 1.0f};
 float specular[4]={1.0f, 1.0f, 1.0f, 1.0f};
 float specularExp=9.f;
 float position[3]={-1.f,-10.f,10.f};
 float color[3]={1.f,1.f,1.0f};
 float angle_l=90.f;
 float attenuation0=0.01f;
 float attenuation1=0.01f;
 float attenuation2=0.01f;
	glUseProgram(shader);
	glUniform4fv(ambientColor, 1, ambient);
//	glUniform3fv(ambientLight, 1, ambient);
	glUniform4fv(diffuseColor, 1, diffuse);
	glUniform4fv(specularColor, 1, specular);
//	glUniform1f(specularExponent, specularExp);
	glUniform3fv(shaderPositionLocation, 1, position);
//	glUniform3fv(shaderColorLocation, 1, color);
//	glUniform1f(shaderAngleLocation, angle_l);
//	glUniform1f(shaderAttenuation0Location, attenuation0);
//	glUniform1f(shaderAttenuation1Location, attenuation1);
//	glUniform1f(shaderAttenuation2Location, attenuation2);
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
			myTriangleFace(i,j,0,(i+10)/20.0,(j+10)/20.0,i+1,j,.0,(i+11)/20.0,(j+10)/20.0,i,j+1,0,(i+10)/20.0,(j+11)/20.0);
			myTriangleFace(i+1,j,0,(i+11)/20.0,(j+10)/20.0,i+1,j+1,0,(i+11)/20.0,(j+11)/20.0,i,j+1,0,(i+10)/20.0,(j+11)/20.0);
		}
	glEnd();
	glDisable(GL_POLYGON_OFFSET_FILL);
	modelView.Translate(10.0f,1.0f,0.0f);
	glUniformMatrix4fv(MVPMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
	glUniformMatrix3fv(normalMatrixLocation,1,GL_FALSE,geometryPipeline.GetNormalMatrix());
	glUniformMatrix4fv(MVPMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
	glDrawElements(GL_TRIANGLES,3*n_faces,GL_UNSIGNED_INT,0);
 /*   glBegin(GL_TRIANGLES);
			myTriangleFace(-1,1,0,0,0,2,1,1,0);
			myTriangleFace(-1,-1,0,0,0,2,-1,1,0);
			myTriangleFace(1,-1,0,0,0,2,-1,-1,0);
			myTriangleFace(1,1,0,0,0,2,1,-1,0);
    glEnd();

*/	modelView.PopMatrix();
	modelView.Rotate(45.0,0,0,1);
	modelView.Translate(0.0,0.0,1.0);
	glUniformMatrix4fv(MVPMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
	glUniformMatrix3fv(normalMatrixLocation,1,GL_FALSE,geometryPipeline.GetNormalMatrix());
	glUniformMatrix4fv(MVPMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
	glDrawElements(GL_TRIANGLES,3*n_faces,GL_UNSIGNED_INT,0);
/*    glBegin(GL_TRIANGLES);
			myTriangleFace(-1,1,0,0,0,2,1,1,0);
			myTriangleFace(-1,-1,0,0,0,2,-1,1,0);
			myTriangleFace(1,-1,0,0,0,2,-1,-1,0);
			myTriangleFace(1,1,0,0,0,2,1,-1,0);
    glEnd();
*///	modelView.Translate(3.0f,3.0f,0.f);
//	glUniformMatrix4fv(MVMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewMatrix());
//	glUniformMatrix3fv(normalMatrixLocation,1,GL_FALSE,geometryPipeline.GetNormalMatrix());
//	glUniformMatrix4fv(MVPMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
//	drawTriangles(20, ico_vertices,ico_faces);
//	modelView.Translate(3.0f,3.0f,0.f);
//	glUniformMatrix4fv(MVMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewMatrix());
//	glUniformMatrix3fv(normalMatrixLocation,1,GL_FALSE,geometryPipeline.GetNormalMatrix());
//	glUniformMatrix4fv(MVPMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
//	drawSmoothTriangles(20, ico_vertices,ico_faces);
	
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
