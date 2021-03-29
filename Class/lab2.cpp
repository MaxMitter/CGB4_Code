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

// Camera motion
GLfloat angle = 0.0; // angle of rotation for the camera direction
GLfloat lx = 0.0f, lz = -1.0f; // actual vector representing the camera's direction
GLfloat x = 0.0f, z = 5.0f;// XZ position of the camera

GLfloat navX = 0.0f;
GLfloat navZ =5.0f;

/*-[Special Keys Callback]---------------------------------------------------*/
//Taken from http://www.lighthouse3d.com/tutorials/glut-tutorial/keyboard-example-moving-around-the-world/
void processSpecialKeys(int key, int xx, int yy) {

	float fraction = 0.1f;

	switch (key) {
	case GLUT_KEY_LEFT:
		angle -= 0.01f;
		lx = sin(angle);
		lz = -cos(angle);
		break;
	case GLUT_KEY_RIGHT:
		angle += 0.01f;
		lx = sin(angle);
		lz = -cos(angle);
		break;
	case GLUT_KEY_UP:
		x += lx * fraction;
		z += lz * fraction;
		break;
	case GLUT_KEY_DOWN:
		x -= lx * fraction;
		z -= lz * fraction;
		break;
	}
}

// TASK 5:
void drawObject(void) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPushMatrix(); //set where to start the current object transformations	
		glRotatef(angleCube, 0.0, 1.0, 0.0);
		glColor3f(0.0, 0.0, 0.0); //change cube1 to black
		glScalef(0.99, 0.99, 0.99);
		glutSolidCube(0.5);
		glTranslatef(0, 0.5, 0); //move cube1 to the right
		glScalef(0.99, 0.99, 0.99);
		glutSolidSphere(0.25f, 20, 20);
	glPopMatrix();
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPushMatrix(); //set where to start the current object transformations	
		glRotatef(angleCube, 0.0, 1.0, 0.0);
		glColor3f(0.0, 1.0, 0.0); //change cube1 to Green
		glutSolidCube(0.5);
		glTranslatef(0, 0.5, 0); //move cube1 to the right
		glutSolidSphere(0.25f, 20, 20);
	glPopMatrix();	
}

/*-[Keyboard Callback]-------------------------------------------------------*/
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a': // lowercase character 'a'
		cout << "You just pressed 'a'" << endl;
		navX -= 0.1;
		break;
	case 'd': // lowercase character 'd'
		cout << "You just pressed 'd'" << endl;
		navX += 0.1;
		break;
	case 'w': // lowercase character 'w'
		cout << "You just pressed 'w'" << endl;
		navZ -= 0.1;
		break;
	case 's': // lowercase character 's'
		cout << "You just pressed 's'" << endl;
		navZ += 0.1;
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

	//glOrtho(-1.0*(GLdouble)x/(GLdouble)y,(GLdouble)x/(GLdouble)y,-1.0,1.0,0.5,20.0);
	gluPerspective(40.0, (GLdouble)x / (GLdouble)y, 0.5, 20.0);
	glViewport(0, 0, x, y);  //Use the whole window for rendering

	// TASK 2:
	//glViewport(0, 0, x/2, y/2);  //Use the whole window for rendering
}

/*-[renderScene Callback]----------------------------------------------------*/
void renderPrimitives(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
	glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer (background)

	/*-Example 1-*/
	glColor3f(0.1, 0.2, 0.3);
	glBegin(GL_TRIANGLES);				// Drawing using triangles
		glVertex3f(0.0f, 1.0f, 0.0f);	// Top
		glVertex3f(-1.0f, -1.0f, 0.0f);	// Bottom left
		glVertex3f(1.0f, -1.0f, 0.0f);	// Bottom right
	glEnd();							// Finished drawing the triangle

	/*-Example 2-*/
	glColor3f(1.0f, 0.0f, 0.0f);		// Red
	glBegin(GL_QUADS);					// Drawing using quads
		glVertex2f(-0.5f, -0.5f);		// Bottom left
		glVertex2f(0.5f, -0.5f);		// Bottom right
		glVertex2f(0.5f, 0.5f);			// Top right
		glVertex2f(-0.5f, 0.5f);		// Top left
	glEnd();

	/*-Example 3-*/
	glBegin(GL_QUADS);					// Drawing using quads
		glColor3f(1.0f, 0.0f, 0.0f);	// Red
		glVertex2f(-0.5f, -0.5f);		// Bottom left
		glColor3f(0.0f, 1.0f, 0.0f);	// Green
		glVertex2f(0.5f, -0.5f);		// Bottom right
		glColor3f(0.0f, 0.0f, 1.0f);	// Blue
		glVertex2f(0.5f, 0.5f);			// Top right
		glColor3f(1.0f, 1.0f, 0.0f);	// Yellow
		glVertex2f(-0.5f, 0.5f);		// Top left
	glEnd();

	// TASK 1:
	glBegin(GL_POLYGON);				// These vertices form a closed polygon
		glColor3f(1.0f, 1.0f, 0.0f);	// Yellow
		glVertex2f(0.4f, 0.2f);
		glVertex2f(0.6f, 0.2f);
		glVertex2f(0.7f, 0.4f);
		glVertex2f(0.6f, 0.6f);
		glVertex2f(0.4f, 0.6f);
		glVertex2f(0.3f, 0.4f);
	glEnd();

	glutSwapBuffers();
}

/*-[render3D Callback]-------------------------------------------------------*/
void render3DScene(void) {
	glMatrixMode(GL_MODELVIEW);		// Set the ModelView Matrix for scene setup
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	glColor3f(0.0f, 1.0f, 0.0f);	// Green
	glTranslatef(0.0f, 0.0f, -1.5f);
	glRotatef(45, 1.0f, 0.0f, 0.0f); 
	glutSolidCube(0.5f);

	// TASK 3:
	glutSolidSphere(0.1f, 20, 20);
	glutSolidTorus(0.6, 1.4, 20, 20);

	glutSwapBuffers();
}

void usingMatrices(void) {
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glLoadIdentity();
	/*	gluLookAt(0.0f, 0.0f, -1.5f, 
		0.0f, 0.0, 0.0f, 
		0.0f, 1.0f, 0.0f);
	*/
	/*
	glMultMatrixf(&mat[0][0]);
	*/
	/*
	glMultMatrixf(&matR[0][0]);
	glTranslatef(0, 0, -1.5);
	*/
	/*
	glMultMatrixf(&mat[0][0]);
	glMultMatrixf(&matR[0][0]);
	glTranslatef(0, 0, -1.5);
	*/
	/*
	glMultMatrixf(&matR[0][0]);
	glMultMatrixf(&mat[0][0]);
	glTranslatef(0, 0, -1.5);
	*/
	glutSolidCube(0.5f);
	glutSwapBuffers();
}
  
/*-[playWithTransforms Callback]---------------------------------------------*/
void renderCube(void) {
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// TASK 6:
/*	gluLookAt(navX, 0.0f, navZ, navX, 0.0, 0, 0.0f, 1.0f, 0.0f);
	*/
	gluLookAt(x, 1.0f, z,
		x + lx, 1.0f, z + lz,
		0.0f, 1.0f, 0.0f);
		
	// TASK 5:
	glTranslatef(0.0f, 0.0f, -5.0f);
	glutSolidCube(0.5f);
	glTranslatef(-1.0f, 0, 0);
	drawObject();
	glTranslatef(-1.0f, 0, 0);
	drawObject();
	glTranslatef(0, 1, 0);
	drawObject();
	// TASK 4:
	/*
	glScalef(2.0f, 2.0f, 1.0f);
	glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(1.0f, 0.0f, 0.0f);
	//To set the object in front of the camera
	glTranslatef(0.0f, 0.0f, -5.0f);
	glutSolidCube(0.5f);
	*/
/*
	glTranslatef(1.0f, 0.0f, 0.0f);
	glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
	glScalef(2.0f, 2.0f, 1.0f);
	//To set the object in front of the camera
	glTranslatef(0.0f, 0.0f, -5.0f);
	glutSolidCube(0.5f);
*/
	glutSwapBuffers();
}

void idleFunc(void) {
	angleCube += 0.1;
	glutPostRedisplay();
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
	//glutDisplayFunc(usingMatrices);

	// ** Part 3 - we play with transformations **
	glEnable(GL_DEPTH_TEST);
	glutDisplayFunc(renderCube);	
	
	glutIdleFunc(idleFunc);

	glutSpecialFunc(processSpecialKeys);

	glutMainLoop(); // start the main loop of GLUT
	return 0;
}