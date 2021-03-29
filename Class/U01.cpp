#include "GL/glew.h"
#include "GL/freeglut.h"
#include <iostream>

using namespace std;

namespace U01 {
	int windowid;
	void renderScene(void) {
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(1.0, 1.0, 0.0, 1.0);

		glutSwapBuffers();
	}

	void keyboard(unsigned char key, int x, int y) {
		switch (key) {
		case 27:
			glutDestroyWindow(windowid);
			exit(0);
			break;
		case 'g':
			glClearColor(0.0, 1.0, 1.0, 1.0);
			glutPostRedisplay();
			break;
		case ' ':
			glClearColor(0.0, 1.0, 0.0, 1.0);
			glutPostRedisplay();
			break;
		default:
			cout << "Key " << key << " at: {" << x << ", " << y << "}" << endl;
			break;
		}
	}

	void onMouseClick(int button, int state, int x, int y) {
		if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
			cout << "Middle Mouse Button clicked at: {" << x << ", " << y << "}" << endl;
		}
	}

	int U01_main(int argc, char** argv) {
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
		glutInitWindowPosition(500, 500);
		glutInitWindowSize(800, 600);

		windowid = glutCreateWindow("OpenGl First Window");
		glutDisplayFunc(renderScene);
		glutKeyboardFunc(keyboard);
		glutMouseFunc(onMouseClick);
		glutMainLoop();
		return 0;
	}
}