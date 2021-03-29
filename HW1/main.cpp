#include "GL/glew.h"
#include "GL\freeglut.h"
#include "GLFW/glfw3.h"
#include <iostream>

#define MAZE_SIZE 10
#define MAZE_WIDTH 20
#define SEGMENT_SIZE (MAZE_WIDTH / MAZE_SIZE)
#define JUMP_STEP_SIZE 0.005

using namespace std;

enum class jumpState { none, up, down };

enum class direction { forward, backward, left, right };

class vector2 {
	public:
	vector2(double xval = 0.0, double yval = 0.0)
		: x{ xval }
		, y{ yval } { }

	virtual void reverse() {
		x = -x;
		y = -y;
	}

	double x, y;
};

class vector3 : public vector2 {
	public:
	vector3(double xval = 0.0, double yval = 0.0, double zval = 0.0) 
		: vector2(xval, yval)
		, z{ zval } { }

	void reverse() override {
		x = -x;
		y = -y;
		z = -z;
	}

	double z;
};

class camera {
	public:
		camera() { }

		void move(vector3 delta) {
			pos.x += delta.x * moveSpeed;
			pos.y += delta.y * moveSpeed;
			pos.z += delta.z * moveSpeed;
		}

		void move(direction dir) {
			switch (dir) {
				case direction::forward:
					pos.x += los.x * moveSpeed;
					pos.z += los.z * moveSpeed;
					break;
				case direction::backward:
					pos.x -= los.x * moveSpeed;
					pos.z -= los.z * moveSpeed;
					break;
				case direction::left:
					pos.x += los.z * moveSpeed;
					pos.z -= los.x * moveSpeed;
					break;
				case direction::right:
					pos.x -= los.z * moveSpeed;
					pos.z += los.x * moveSpeed;
					break;
				default:
					break;
			}
		}

		void look(vector3 delta) {
			los.x += delta.x * lookSpeed;
			los.y += delta.y * lookSpeed;
			los.z += delta.z * lookSpeed;
		}

		void look(double dx) {
			horizontalAngle += dx * lookSpeed;
			if (horizontalAngle >= 360)  horizontalAngle = 0;
			if (horizontalAngle <= -360) horizontalAngle = 0;
			
			los.x = sin(horizontalAngle);
			los.z = -cos(horizontalAngle);
		}

		vector3 getPos() {
			return pos;
		}

		vector3 getLos() {
			return los;
		}

		vector3 getLookAt() {
			vector3 temp{ pos.x + los.x, pos.y, pos.z + los.z };
			return temp;
		}

		void setY(double y) {
			pos.y = y;
		}

		double getY() {
			return pos.y;
		}

	private:
		vector3 pos{0.0, 1.0, 5.0};
		vector3 los{0.0, 1.0, -1.0};
		float moveSpeed{ 0.5 };
		float lookSpeed{ 0.01 };
		float horizontalAngle{ 0.0 };
};

int windowid;
int maze[MAZE_SIZE][MAZE_SIZE] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
	{1, 0, 0, 1, 0, 0, 0, 1, 0, 1},
	{1, 0, 1, 1, 0, 1, 1, 1, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

camera player;
jumpState jumping = jumpState::none;

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a': // lowercase character 'a'
		player.move(direction::left);
		break;
	case 'd': // lowercase character 'd'
		player.move(direction::right);
		break;
	case 'w': // lowercase character 'w'
		player.move(direction::forward);
		break;
	case 's': // lowercase character 's'
		player.move(direction::backward);
		break;
	case ' ': // space
		if (jumping == jumpState::none) {
			jumping = jumpState::up;
		}
		break;
	case 27: // Escape key
		glutDestroyWindow(windowid);
		exit(0);
		break;
	}

	glutPostRedisplay();
}

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

void moveCamera(int x, int y) {
	int centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	int centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;

	int dx = x - centerX;

	player.look(dx);

	glutWarpPointer(centerX, centerY);
}

void drawSky(void) {
	glClearColor(0.55, 0.86, 0.91, 1.0);
}

void drawFloor(void) {
	glColor3d(0.48, 0.28, 0.11);
	glPushMatrix();
	glBegin(GL_POLYGON);
		glVertex3d(-MAZE_WIDTH / 2, 0.0, -MAZE_WIDTH / 2);
		glVertex3d( MAZE_WIDTH / 2, 0.0, -MAZE_WIDTH / 2);
		glVertex3d( MAZE_WIDTH / 2, 0.0,  MAZE_WIDTH / 2);
		glVertex3d(-MAZE_WIDTH / 2, 0.0,  MAZE_WIDTH / 2);
	glEnd();
	glPopMatrix();
}

void drawLabyrinth(void) {
	glColor3d(0.18, 0.55, 0.14);
	for (int i = 0; i < MAZE_SIZE; i++) {
		for (int j = 0; j < MAZE_SIZE; j++) {
			if (maze[i][j] == 1) {
				glPushMatrix();
				glTranslated(-MAZE_WIDTH / 2 + i * SEGMENT_SIZE + SEGMENT_SIZE / 2, SEGMENT_SIZE / 2, -MAZE_WIDTH / 2 + j * SEGMENT_SIZE + SEGMENT_SIZE / 2);
				glScaled(0.99, 0.99, 0.99);
				glutSolidCube(SEGMENT_SIZE + 0.1);
				glPopMatrix();
			}
		}
	}
}

void drawGame(void) {
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	if (jumping == jumpState::up) {
		player.move(vector3{ 0.0, JUMP_STEP_SIZE, 0.0 });
		if (player.getY() >= 2.0) {
			jumping = jumpState::down;
		}
	}
	if (jumping == jumpState::down) {
		player.move(vector3{ 0.0, -JUMP_STEP_SIZE, 0.0 });
		if (player.getY() <= 1.0) {
			player.setY(1.0);
			jumping = jumpState::none;
		}
	}

	gluLookAt(	player.getPos().x,	  player.getPos().y,	player.getPos().z,
				player.getLookAt().x, player.getLookAt().y,	player.getLookAt().z,
				0.0f,				  1.0f,					0.0f);

	drawSky();
	drawFloor();
	drawLabyrinth();

	glutSwapBuffers();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	windowid = glutCreateWindow("Labyrinth"); // create and name window

	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(moveCamera);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glutReshapeFunc(reshapeFunc);
	glutDisplayFunc(drawGame);
	glutIdleFunc(drawGame);
	
	glutFullScreen();
	glutMainLoop(); // start the main loop of GLUT
	return 0;
}