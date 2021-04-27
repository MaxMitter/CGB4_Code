#include "GL/glew.h"
#include "GL/freeglut.h"
#include <iostream>
using namespace std;

// GLUT Window ID
int windowid;

// Materials to play with
GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat no_shininess = 0.0f;
GLfloat low_shininess = 5.0f;
GLfloat high_shininess = 100.0f;
GLfloat mat_emission[] = { 0.3f, 0.2f, 0.2f, 0.0f };
GLfloat mat_shininess[] = { 50.0 };
GLfloat mat_ambient_color[] = { 0.8f, 0.8f, 0.2f, 1.0f };
GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat mat_diffuse[] = { 0.1f, 0.5f, 0.8f, 1.0f };

GLfloat light_direction[] = { 1.0, 1.0, 1.0, 0.0 };

GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat light_diffuse[] = { 1.0, 0.5, 0.5, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

GLfloat attenuation = 0;

GLfloat angle = 0.0; // angle of rotation for the spotlight direction
GLfloat lx = 0.0f, lz = -1.0f; // actual vector components representing the spotlight direction


void foggy() {
	GLuint fogMode[] = { GL_EXP, GL_EXP2, GL_LINEAR }; // Storage For Three Types Of Fog
	GLuint fogfilter = 2;                              // Which Fog To Use
	GLfloat fogColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };  // Fog Color

	glFogi(GL_FOG_MODE, fogMode[fogfilter]);  // Fog Mode
	glFogfv(GL_FOG_COLOR, fogColor);          // Set Fog Color
	glFogf(GL_FOG_DENSITY, 0.35f);            // How Dense Will The Fog Be
	glHint(GL_FOG_HINT, GL_DONT_CARE);        // Fog Hint Value
	glFogf(GL_FOG_START, 2.0f);               // Fog Start Depth
	glFogf(GL_FOG_END, 4.0f);                 // Fog End Depth
	glEnable(GL_FOG);                         // Enables GL_FOG
};

void rotateSpot(void) {
	angle -= 0.005f;
	lx = sin(angle);
	lz = -cos(angle);
	GLfloat spot_direction[] = { lx, 0.0f, lz, 1.0f }; //rotate
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
	glutPostRedisplay();
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

		break;
	case 's': // lowercase character 's'
		cout << "You just pressed 's'" << endl;

		break;
	case '1': {
		cout << "You just pressed '1'" << endl;
		GLfloat ldr[] = { -1.0f, 0.0f, 0.0f, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, ldr);
		break;
	}
	case '2': {
		cout << "You just pressed '2'" << endl;
		GLfloat ldr[] = { 0.0f, 1.0f, 0.0f, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, ldr);
		break;
	}
	case '3': {
		cout << "You just pressed '3'" << endl;
		GLfloat ldr[] = { 0.0f, 0.0f, 1.0f, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, ldr);
		break;
	}
	case '+': {
		cout << "You just pressed '+'" << endl;
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, attenuation -= 0.1);
		break;
	}
	case '-': {
		cout << "You just pressed '-'" << endl;
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, attenuation += 0.1);
		break;
	}

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

	gluPerspective(40.0, (GLdouble)x / (GLdouble)y, 0.5, 40.0);
	glViewport(0, 0, x, y);  //Use the whole window for rendering
}

void idleFunc(void) {

}

void initLightSources(void)
{
	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat mat_direction[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	GLfloat light_direction[] = { 1.0f, 1.0f, 1.0f, 0.0f };

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
	glEnable(GL_LIGHT0);

	//add position light
	GLfloat light_pos[] = { 3.0, 0.0, 4.0, 1.0 };
	GLfloat light_color[] = { 0.5, 0.8, 0.5, 1.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_color);
	glEnable(GL_LIGHT1);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

void lightingSetup(void) {
	GLfloat lmodel_ambient[] = { 0.1f, 0.1f, 0.9f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
}

void renderCube(void) {
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glLoadIdentity();
	glTranslatef(0.0f, 0.1f, -1.5f);
	glRotatef(45, 1.0f, 0.0f, 0.0f);
	glRotatef(45, 0.0f, 1.0f, 0.0f);
	glColor4f(0.8f, 0.1f, 0.1f, 1.0f);
	glutSolidCube(0.5f);
	glutSwapBuffers();
}

void renderScene(void) {
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 0.0); // Original Black
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(1.0f, 0.1f, -3.5f);
	glRotatef(45, 1.0f, 0.0f, 0.0f);
	glRotatef(45, 0.0f, 1.0f, 0.0f);
	glutSolidCube(0.5f);
	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(-1.0f, 0.1f, -2.5f);
	glRotatef(45, 1.0f, 0.0f, 0.0f);
	glRotatef(45, 0.0f, 1.0f, 0.0f);
	glutSolidCube(0.5f);
	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.0f, 0.5f, -3.0f);
	glRotatef(45, 1.0f, 0.0f, 0.0f);
	glRotatef(45, 0.0f, 1.0f, 0.0f);
	glutSolidCube(0.5f);
	glPopMatrix();

	glutSwapBuffers();
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(500, 500); //determines the initial position of the window
	glutInitWindowSize(800, 600);	  //determines the size of the window
	windowid = glutCreateWindow("Our Third OpenGL Window"); // create and name window

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//lightingSetup();
	initLightSources();
	// register callbacks
	glutKeyboardFunc(keyboard);
	glutMouseFunc(onMouseClick);
	//glutDisplayFunc(renderCube);
	glutDisplayFunc(renderScene);
	glutReshapeFunc(reshapeFunc);
	glutIdleFunc(idleFunc);

	glutMainLoop(); // start the main loop of GLUT
	return 0;
}



