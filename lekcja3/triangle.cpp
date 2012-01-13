// Triangle.cpp
// Our first OpenGL program that will just draw a triangle on the screen.
#include <Windows.h>
#include <GLTools.h>            // OpenGL toolkit
#include <GLFrame.h>
#include <GLFrustum.h>

#ifdef __APPLE__
#include <glut/glut.h>          // OS X version of GLUT
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>            // Windows FreeGlut equivalent
#endif


GLuint shader;
GLint MVPMatrixLocation;
GLFrame frame;
GLFrustum frustum;

///////////////////////////////////////////////////////////////////////////////
// Window has changed size, or has just been created. In either case, we need
// to use the window dimensions to set the viewport and the projection matrix.

void ChangeSize(int w, int h) {
    glViewport(0, 0, w, h);
	frustum.SetPerspective(45.0f,w/h,5,20);
}


///////////////////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering context.
// This is the first opportunity to do any OpenGL related tasks.

void SetupRC() {
    // Blue background
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    shader = gltLoadShaderPairWithAttributes("pass_thru_shader.vp", "pass_thru_shader.fp",
            2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_COLOR, "vColor");
    fprintf(stdout, "GLT_ATTRIBUTE_VERTEX : %d\nGLT_ATTRIBUTE_COLOR : %d \n",
            GLT_ATTRIBUTE_VERTEX, GLT_ATTRIBUTE_COLOR);
	MVPMatrixLocation=glGetUniformLocation(shader,"MVPMatrix");
	if(MVPMatrixLocation==-1)
	{
		fprintf(stderr,"uniform MVPMatrix could not be found\n");
	}
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
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	M3DMatrix44f mCamera;
	const float* mProjection = frustum.GetProjectionMatrix();
	M3DVector3f at = {0.0f,0.0f,0.0f};
	M3DVector3f eye = {0.0f,-6.0f,9.0f};
	M3DVector3f up = {0.0f,1.0f,0.0f};
	LookAt(frame,eye,at,up);	
	frame.GetCameraMatrix(mCamera);
	M3DMatrix44f ViewProjectionMatrix;
	m3dMatrixMultiply44(ViewProjectionMatrix,mProjection,mCamera);
    glUseProgram(shader);
	glUniformMatrix4fv(MVPMatrixLocation,1,GL_FALSE,ViewProjectionMatrix);
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

    // Perform the buffer swap to display back buffer
    glutSwapBuffers();
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
