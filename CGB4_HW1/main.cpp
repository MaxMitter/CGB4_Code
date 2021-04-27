// Excersize 1: Maze game
// By:			Maximilian Mitter - S1910307074
// Time taken:	16h
#include "GL/glew.h"
#include "GL\freeglut.h"
#include "GLFW/glfw3.h"
#include <iostream>

#define MAZE_WIDTH 20 // how far a maze streches on one axis in World Coordinates
#define MAZE_SIZE 10 // how many sections a maze has on one axis
#define SEGMENT_SIZE (MAZE_WIDTH / MAZE_SIZE) // the size of one section in World Coordinates

using namespace std;

enum class jumpState { none, up, down };

enum class direction { forward, backward, left, right };

class vector2 {
	public:
		vector2(float xval = 0.0, float yval = 0.0)
			: x{ xval }
			, y{ yval } { }

		vector2(int xval, int yval)
			: x{ float(xval) }
			, y{ float(yval) } { }

	float distanceTo(vector2 v) {
		return sqrt(pow(this->x - v.x, 2) + pow(this->y - v.y, 2));
	}

	float x, y;
};

class vector3 : public vector2 {
	public:
	vector3(float xval = 0.0, float yval = 0.0, float zval = 0.0) 
		: vector2(xval, yval)
		, z{ zval } { }

	float z;
};

// Converts Maze Coordinates to World Coordinates
vector2 getWorldCoordsFromGbCoords(vector2 v) {
	vector2 temp{ -MAZE_WIDTH / 2 + v.x * SEGMENT_SIZE + SEGMENT_SIZE / 2,
				   MAZE_WIDTH / 2 - v.y * SEGMENT_SIZE - SEGMENT_SIZE / 2 };
	return temp;
}

// Printing Vectors for debugging
ostream& operator<< (ostream& lhs, const vector2 rhs) {
	lhs << "{" << rhs.x << ", " << rhs.y << "}";
	return lhs;
}
ostream& operator<< (ostream& lhs, const vector3 rhs) {
	lhs << "{" << rhs.x << ", " << rhs.y << ", " << rhs.z << "}";
	return lhs;
}

class camera {
	public:
		camera() = default;

		void move(vector3 delta) {
			pos.x += delta.x * moveSpeed;
			pos.y += delta.y * moveSpeed;
			pos.z += delta.z * moveSpeed;
			updateGbPos();
		}

		// keeps the camera GameBoard Position up to date
		void updateGbPos() {
			int gbX = 0;
			int gbZ = 0;
			vector2 closest{};
			//distance has to be smaller or equal to the distance of the corner of a segment
			float distClosest = sqrt(pow(SEGMENT_SIZE / 2, 2) + pow(SEGMENT_SIZE / 2, 2));
			for (int i = 0; i < MAZE_SIZE; i++) {
				for (int j = 0; j < MAZE_SIZE; j++) {
					auto worldCoord = getWorldCoordsFromGbCoords(vector2{ i, j });
					float dist = vector2{ pos.x, pos.z }.distanceTo(worldCoord);
					if (dist <= distClosest) {
						closest = worldCoord;
						distClosest = dist;
						gbX = i;
						gbZ = j;
						//loops can be escaped, there will be no smaller distance
						break; break;
					}
				}
			}

			gbPos = vector2{ gbX, gbZ };
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

			updateGbPos();
		}

		void look(vector3 delta) {
			los.x += delta.x * lookSpeed;
			los.y += delta.y * lookSpeed;
			los.z += delta.z * lookSpeed;
		}

		void look(float dx) {
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

		// Gets a vector of where the player is looking at
		vector3 getLookAt() {
			vector3 temp{ pos.x + los.x, pos.y, pos.z + los.z };
			return temp;
		}

		vector2 getGameBoardCoords() {
			return gbPos;
		}

		void setJumpState(jumpState state) {
			jumping = state;
		}

		jumpState getJumpState() {
			return jumping;
		}

		void jump() {
			if (jumping == jumpState::up) {
				move(vector3{ 0.0, jumpSpeed, 0.0 });
				if (pos.y >= 2.0)
					jumping = jumpState::down;
			}
			else if (jumping == jumpState::down) {
				move(vector3{ 0.0, -jumpSpeed, 0.0 });
				if (pos.y <= 1.0) {
					jumping = jumpState::none;
				}
			}
		}

	private:
		vector2 gbPos{ 5.0f, 4.0f };
		vector3 pos{1.0f, 1.0f, 1.0f};
		vector3 los{0.0f, 1.0f, -1.0f};
		float horizontalAngle{ 0.0 };
		jumpState jumping{ jumpState::none };
		
		//values can be changed for slower/faster walking and looking speed
		float moveSpeed{ 0.5 };
		float lookSpeed{ 0.01 };
		float jumpSpeed{ 0.005 };
};

// base class for items
class object {
	public:
		object() = default;

		virtual void draw() { draw(pos); }
		virtual void draw(vector3 position) = 0;


	protected:
		vector3 pos{ 0.0, 0.7, 0.0 };
		float angle{ 0.0 };
		float rotationStepSize{ 0.05 };
};

class ring : public object {
	public:

		void draw(vector3 position) override {
			angle = angle >= 360 ? 0 : angle + rotationStepSize;
			glTranslatef(position.x, position.y + pos.y, position.z);
			glRotatef(angle, 0, 1, 0);
			glPushMatrix();
				glPushMatrix();
				glColor3f(0.42, 0.84, 0.9);
				glTranslatef(0.0, 0.38, 0.0);
				glScalef(0.25, 0.25, 0.25);
				glutSolidSphere(0.5, 10, 20);
				glPopMatrix();

				glPushMatrix();
				glColor3f(0.99, 0.87, 0.13);
				glScalef(0.5, 0.5, 0.5);
				glutSolidTorus(0.1, 0.5, 25, 25);
				glPopMatrix();
			glPopMatrix();
		}
};

class pyramid : public object {
	public:
		void draw(vector3 position) override {
			glColor3f(0, 0, 0);
			angle = angle >= 360 ? 0 : angle + rotationStepSize;
			glTranslatef(position.x, position.y + pos.y, position.z);
			glRotatef(angle, 0, 1, 0);
			glPushMatrix();
				glBegin(GL_QUADS);
				glVertex3f( 0.25, -0.5,  0.25);
				glVertex3f(-0.25, -0.5,  0.25);
				glVertex3f(-0.25, -0.5, -0.25);
				glVertex3f( 0.25, -0.5, -0.25);
				glEnd();

				glBegin(GL_TRIANGLES);
				glVertex3f( 0.25, -0.5, 0.25);
				glVertex3f(-0.25, -0.5, 0.25);
				glVertex3f( 0.0,   0.0, 0.0);
				glEnd();

				glBegin(GL_TRIANGLES);
				glVertex3f(-0.25, -0.5,  0.25);
				glVertex3f(-0.25, -0.5, -0.25);
				glVertex3f( 0.0,   0.0,  0.0);
				glEnd();

				glBegin(GL_TRIANGLES);
				glVertex3f(-0.25, -0.5, -0.25);
				glVertex3f( 0.25, -0.5, -0.25);
				glVertex3f( 0.0,   0.0,  0.0);
				glEnd();

				glBegin(GL_TRIANGLES);
				glVertex3f(0.25, -0.5, -0.25);
				glVertex3f(0.25, -0.5,  0.25);
				glVertex3f(0.0,   0.0,  0.0);
				glEnd();
			glPopMatrix();
		}
};

class cup : public object {
	public:
		void draw(vector3 position) override {
			glTranslatef(position.x, position.y + pos.y, position.z);
			glPushMatrix();
				glRotatef(90, 1, 0, 0);

				glPushMatrix();
					glColor3f(0.79, 0.16, 0.79);
					glRotatef(90, 1, 0, 0);
					glTranslatef(0.25, 0.25, 0);
					glutSolidTorus(0.05, 0.1, 25, 25);
				glPopMatrix();	
				
				glColor3f(0.99, 0.16, 0.89);
				glutSolidCylinder(0.25, 0.5, 25, 25);
			glPopMatrix();

		}
};

int windowid;
// Saves the maze layout.
// 0 == Air
// 1 == Wall
// 2 == Ring
// 3 == Pyramid
// 4 == Cup
int maze[MAZE_SIZE][MAZE_SIZE] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 1, 0, 0, 0, 1, 0, 1, 1},
	{1, 0, 1, 0, 1, 0, 0, 0, 0, 1},
	{1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
	{1, 0, 1, 2, 0, 0, 3, 1, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
	{1, 0, 1, 0, 0, 0, 4, 1, 0, 1},
	{1, 0, 1, 1, 0, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

camera player;
ring r;
pyramid p;
cup jg;

// checks if the cameras current GameBoard Section has a wall
bool cameraCollides() {
	vector2 gbCoord = player.getGameBoardCoords();
	return (maze[int(gbCoord.x)][int(gbCoord.y)] == 1);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a': // lowercase character 'a'
		player.move(direction::left);
		if (cameraCollides()) player.move(direction::right);
		break;
	case 'd': // lowercase character 'd'
		player.move(direction::right);
		if (cameraCollides()) player.move(direction::left);
		break;
	case 'w': // lowercase character 'w'
		player.move(direction::forward);
		if (cameraCollides()) player.move(direction::backward);
		break;
	case 's': // lowercase character 's'
		player.move(direction::backward);
		if (cameraCollides()) player.move(direction::forward);
		break;
	case ' ': // space
		if (player.getJumpState() == jumpState::none) {
			player.setJumpState(jumpState::up);
		}
		break;
	case 27: // Escape key
		glutDestroyWindow(windowid);
		exit(0);
		break;
	}

	glutPostRedisplay();
}

// copied from the Example file
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

// gets mouse movement and keeps the cursor in the center
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
	for (int i = 0; i < MAZE_SIZE; i++) {
		for (int j = 0; j < MAZE_SIZE; j++) {
			if (maze[i][j] == 1) {
				glPushMatrix();
				glColor3d(0.18, 0.55, 0.14);
				auto tmp = getWorldCoordsFromGbCoords(vector2{ i, j });
				glTranslated(tmp.x, SEGMENT_SIZE / 2, tmp.y);
				//cube is slightly bigger than a segment to avoid lines
				//between segments
				glutSolidCube(SEGMENT_SIZE + 0.01);
				glPopMatrix();
			}
		}
	}
}

void drawItems(void) {
	for (int i = 0; i < MAZE_SIZE; i++) {
		for (int j = 0; j < MAZE_SIZE; j++) {
			if (maze[i][j] == 2) {
				glPushMatrix();
				auto tmp = getWorldCoordsFromGbCoords(vector2{ i, j });
				r.draw(vector3{ tmp.x, 0.0, tmp.y });
				glPopMatrix();
			} else if (maze[i][j] == 3) {
				glPushMatrix();
				auto tmp = getWorldCoordsFromGbCoords(vector2{ i, j });
				p.draw(vector3{ tmp.x, 0.0, tmp.y });
				glPopMatrix();
			} else if (maze[i][j] == 4) {
				glPushMatrix();
				auto tmp = getWorldCoordsFromGbCoords(vector2{ i, j });
				jg.draw(vector3{ tmp.x, 0.0, tmp.y });
				glPopMatrix();
			}
		}
	}
}

void drawGame(void) {
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	player.jump();

	gluLookAt(	player.getPos().x,	  player.getPos().y,	player.getPos().z,
				player.getLookAt().x, player.getLookAt().y,	player.getLookAt().z,
				0.0f,				  1.0f,					0.0f);

	drawSky();
	drawFloor();
	drawItems();
	drawLabyrinth();

	glutSwapBuffers();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	windowid = glutCreateWindow("Labyrinth"); // create and name window

	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(moveCamera);
	glutSetCursor(GLUT_CURSOR_NONE);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glutReshapeFunc(reshapeFunc);
	glutDisplayFunc(drawGame);
	glutIdleFunc(drawGame);
	
	glutFullScreen();
	glutMainLoop(); // start the main loop of GLUT
	return 0;
}