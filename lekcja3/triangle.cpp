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
GLint MVPMatrixLocation;
GLFrame frame;
GLFrustum frustum;

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
    shader = gltLoadShaderPairWithAttributes("pass_thru_shader.vp", "pass_thru_shader.fp",
            2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_COLOR, "vColor");
    fprintf(stdout, "GLT_ATTRIBUTE_VERTEX : %d\nGLT_ATTRIBUTE_COLOR : %d \n",
            GLT_ATTRIBUTE_VERTEX, GLT_ATTRIBUTE_COLOR);
	MVPMatrixLocation=glGetUniformLocation(shader,"MVPMatrix");
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
	glUseProgram(shader);
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
	modelView.Translate(10.0f,1.0f,0.0f);
	glUniformMatrix4fv(MVPMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
	glBegin(GL_QUADS);
	    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.5f, 0.5f, 0.5f);
		glVertex3f(-1.0f,-1.0f,0.0f);
		glVertex3f(-1.0f,1.0f,0.0f);
		glVertex3f(1.0f,1.0f,0.0f);
		glVertex3f(1.0f,-1.0f,0.0f);
	glEnd();
    glBegin(GL_TRIANGLES);
	    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
 		glVertex3f(0.0f, 0.0f, 2.0f);
 		glVertex3f(1.0f, 1.0f, 0.0f);
	    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0f, 0.0f, 0.8f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
 		glVertex3f(0.0f, 0.0f, 2.0f);
 		glVertex3f(-1.0f, 1.0f, 0.0f);
	    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
 		glVertex3f(0.0f, 0.0f, 2.0f);
 		glVertex3f(-1.0f, -1.0f, 0.0f);
	    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
 		glVertex3f(0.0f, 0.0f, 2.0f);
 		glVertex3f(1.0f, -1.0f, 0.0f);
    glEnd();
	modelView.PopMatrix();
	modelView.Rotate(45.0,0,0,1);
	modelView.Translate(0.0,0.0,1.0);
	glUniformMatrix4fv(MVPMatrixLocation,1,GL_FALSE,geometryPipeline.GetModelViewProjectionMatrix());
	glBegin(GL_QUADS);
	    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.5f, 0.5f, 0.5f);
		glVertex3f(-1.0f,-1.0f,0.0f);
		glVertex3f(-1.0f,1.0f,0.0f);
		glVertex3f(1.0f,1.0f,0.0f);
		glVertex3f(1.0f,-1.0f,0.0f);
	glEnd();
    glBegin(GL_TRIANGLES);
	    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
 		glVertex3f(0.0f, 0.0f, 2.0f);
 		glVertex3f(1.0f, 1.0f, 0.0f);
	    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0f, 0.0f, 0.8f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
 		glVertex3f(0.0f, 0.0f, 2.0f);
 		glVertex3f(-1.0f, 1.0f, 0.0f);
	    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
 		glVertex3f(0.0f, 0.0f, 2.0f);
 		glVertex3f(-1.0f, -1.0f, 0.0f);
	    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
 		glVertex3f(0.0f, 0.0f, 2.0f);
 		glVertex3f(1.0f, -1.0f, 0.0f);
    glEnd();

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
