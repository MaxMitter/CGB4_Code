#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>
using namespace std;

// GLUT Window ID
int windowid;

// A rotatation matrix
GLfloat matR[][4] = {
	{ 0.707, 0.707, 0.0, 0.0},
	{ -0.707, 0.707, 0.0, 0.0},
	{ 0.0, 0.0, 1.0, 0.0},
	{ 0.0, 0.0, 0.0, 1.0}
};

// A translation matrix
GLfloat mat[][4] = {
	{ 1.0, 0.0, 0.0, 0.0},
	{ 0.0, 1.0, 0.0, 0.0},
	{ 0.0, 0.0, 1.0, 0.0},
	{ 0.5, 0.0, -1.5, 1.0}
};

// Angle for cube rotation
GLfloat angleCube = 0.0; //angle for cube1

GLfloat navX = 0.0f;
GLfloat navZ = 5.0f;

// TASK 5:
void drawObject(void) {

}

/*-[Keyboard Callback]-------------------------------------------------------*/
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a': // lowercase character 'a'
		cout << "You just pressed 'a'" << endl;

		break;
	case 'd': // lowercase character 'd'
		cout << "You just pressed 'd'" << endl;

		break;
	case 'w': // lowercase character 'w'
		cout << "You just pressed 'w'" << endl;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 's': // lowercase character 's'
		cout << "You just pressed 's'" << endl;
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 27: // Escape key
		glutDestroyWindow(windowid);
		exit(0);
		break;
	}
	glutPostRedisplay();
}

/*-[MouseClick Callback]-----------------------------------------------------*/
void onMouseClick(int button, int state, int x, int y) {
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		cout << "Middle button clicked at position "
			<< "x: " << x << " y: " << y << endl;
	}
}

/*-[Reshape Callback]--------------------------------------------------------*/
void reshapeFunc(int x, int y) {
	if (y == 0 || x == 0) return;  //Nothing is visible then, so return

	glMatrixMode(GL_PROJECTION); //Set a new projection matrix
	glLoadIdentity();
	//Angle of view: 40 degrees
	//Near clipping plane distance: 0.5
	//Far clipping plane distance: 20.0

	gluPerspective(40.0, (GLdouble)x / (GLdouble)y, 0.5, 20.0);
	glViewport(0, 0, x, y);  //Use the whole window for rendering

}

/*-[renderScene Callback]----------------------------------------------------*/
void renderPrimitives(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
	glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer (background)


	glColor3f(0.1, 0.2, 0.3);
	glBegin(GL_TRIANGLES);				// Drawing using triangles
	glVertex3f(0.0f, 1.0f, 0.0f);	// Top
	glVertex3f(-1.0f, -1.0f, 0.0f);	// Bottom left
	glVertex3f(1.0f, -1.0f, 0.0f);	// Bottom right
	glEnd();							// Finished drawing the triangle

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(-0.5f, -0.5f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.5f, -0.5f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(0.5f, 0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-0.5f, 0.5f);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex2f(-0.2f, -0.5f);
	glVertex2f(-0.5f, 0.0f);
	glVertex2f(-0.2f, 0.5f);
	glVertex2f(0.2f, 0.5f);
	glVertex2f(0.5f, 0.0f);
	glVertex2f(0.2f, -0.5f);
	glEnd();
	
	glutSwapBuffers();
}

/*-[render3D Callback]-------------------------------------------------------*/
void render3DScene(void) {
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	glColor3f(0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, -1.5f);
	glRotatef(45, 1.0f, 0.0f, 0.0f);
	glutSolidCube(0.5);
	glutSolidSphere(0.1f, 25, 25);
	
	glutSwapBuffers();
}

/*-[playWithTransforms Callback]---------------------------------------------*/
void renderCube(void) {
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	//glLoadMatrixf(&matR[0][0]);
	glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
	glScalef(2.0f, 2.0f, 1.0f);
	glTranslatef(1.0f, 0.0f, 0.0f);

	
	glTranslatef(0.0f, 0.0f, -5.0f);
	glutSolidCube(0.5f);
	glutSwapBuffers();
}

void idleFunc(void) {

}

void drawEyes(void) {
	glColor3d(0.1, 0.7, 0.0);
	glTranslated(0.0, 1.0, -5.0);
	glPushMatrix();
		glTranslated(-0.5, 0.0, 0.0);
		glutSolidSphere(0.1, 25, 25);
	glPopMatrix();
	glPushMatrix();
		glTranslated(0.5, 0.0, 0.0);
		glutSolidSphere(0.1, 25, 25);
	glPopMatrix();
}

void drawHead(void) {
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	
	glPushMatrix();
		drawEyes();
	glPopMatrix();

	glTranslated(0.0, 0.0, -1.0);
	glutSolidSphere(0.5, 25, 25);

	glutSwapBuffers();
}

int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(500, 500); //determines the initial position of the window
	glutInitWindowSize(800, 600); //determines the size of the window
	windowid = glutCreateWindow("Our Second OpenGL Window"); // create and name window

	// register callbacks
	glutKeyboardFunc(keyboard);
	glutMouseFunc(onMouseClick);

	// ** Part 1 - we simply render primitives **
	//glutDisplayFunc(renderPrimitives);

	// ** Part 2 - we start with 3D scene setup and projections **
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glutReshapeFunc(reshapeFunc);
	//glutDisplayFunc(render3DScene);

	// ** Part 3 - we play with transformations **
	//glutDisplayFunc(renderCube);
	glutDisplayFunc(drawHead);

	//glutIdleFunc(idleFunc);

	glutMainLoop(); // start the main loop of GLUT
	return 0;
}