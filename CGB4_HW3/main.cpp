#include <algorithm>

#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>
#include <random>
#include <vector>

using namespace std;

// HAS TO BE EVEN
#define MAX_CARDS 6

struct vector3 {
	float x;
	float y;
	float z;
};

struct card {
	vector3 position{ 0, 0, 0 };
	int id{0};
};

vector3 position{ 0, 0, -11 };

// Data read from the header of the BMP file
unsigned char header[54]; // Each BMP file begins by a 54-bytes header
unsigned int dataPos;     // Position in the file where the actual data begins
unsigned int imageWidth, imageHeight;
unsigned int imageSize;   // = width*height*3
// Actual RGB data
unsigned char* imageData;

vector<card> cards;

	// image file Strings
	std::string cardBack = "img/cardback.bmp";
std::string background = "img/background.bmp";

// converted to C-Strings
char* cardBackPath = &cardBack[0];
char* backgroundPath = &background[0];

// texture names
static GLuint cardBackName;
static GLuint backgroundName;

// GLUT Window ID
int windowid;

void moveCamera(float x, float y, float z) {
	position.x += x;
	position.y += y;
	position.z += z;
}

int loadBMP_custom(char* imagepath) {
	FILE* file;
	fopen_s(&file, imagepath, "rb"); // Open the file
	if (!file) {
		cout << "Image could not be opened" << endl;
		return 0;
	}

	if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
		cout << "Not a correct BMP file" << endl;
		return 0;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		cout << "Not a correct BMP file" << endl;
		return 0;
	}

	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	imageWidth = *(int*)&(header[0x12]);
	imageHeight = *(int*)&(header[0x16]);
	// Some BMP files are misformatted, guess missing information
	// 3 : one byte for each Red, Green and Blue component
	if (imageSize == 0)    imageSize = imageWidth * imageHeight * 3;
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way
	cout << "imageSize:" << imageSize << endl;
	cout << "dataPos:" << dataPos << endl;
	imageData = new unsigned char[imageSize]; // Create a buffer
	fread(imageData, 1, imageSize, file); // Read the actual data from the file into the buffer

	fclose(file); //Everything is in memory now, the file can be closed
}

void initBackgroundTexture (void) {
	loadBMP_custom(backgroundPath);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &backgroundName);
	glBindTexture(GL_TEXTURE_2D, backgroundName);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, imageData);
}

void initCardBackTexture (void) {
	loadBMP_custom(cardBackPath);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &cardBackName);
	glBindTexture(GL_TEXTURE_2D, cardBackName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, imageData);
}

void initTextures(void) {
	initBackgroundTexture();
	initCardBackTexture();
}

void displayBackground(void) {
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, backgroundName);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(-7.5, -5.0, 0.0);
		glTexCoord2f(0.0, 1.0); glVertex3f(-7.5,  5.0, 0.0);
		glTexCoord2f(1.0, 1.0); glVertex3f( 7.5,  5.0, 0.0);
		glTexCoord2f(1.0, 0.0); glVertex3f( 7.5, -5.0, 0.0);
	glEnd();
	glFlush();
	glDisable(GL_TEXTURE_2D);
}

void displayCard(card c) {
	vector3 botLeft = c.position;
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, cardBackName);
	switch(c.id) {
		case 1:
			glColor3f(1.0, 0.0, 0.0);
			break;
		case 2:
			glColor3f(0.0, 1.0, 0.0);
			break;
		case 3:
			glColor3f(0.0, 0.0, 1.0);
			break;
		default:
			break;
	}
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(botLeft.x, botLeft.y, botLeft.z);
		glTexCoord2f(0.0, 1.0); glVertex3f(botLeft.x, botLeft.y + 2.25, botLeft.z);
		glTexCoord2f(1.0, 1.0); glVertex3f(botLeft.x + 1.5, botLeft.y + 2.25, botLeft.z);
		glTexCoord2f(1.0, 0.0); glVertex3f(botLeft.x + 1.5, botLeft.y, botLeft.z);
	glEnd();
	glFlush();
	glDisable(GL_TEXTURE_2D);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for (auto& c : cards) {
		displayCard(c);
	}
	
	displayBackground();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(position.x, position.y, position.z);
	glutSwapBuffers();
}

vector<int> generateRandomCardIds(void) {
	vector<int> ids = { 1, 1, 2, 2, 3, 3 };
	random_device rd;
	mt19937 g(rd());
	shuffle(ids.begin(), ids.end(), g);
	return ids;
}

void createCards(void) {
	int amount = 6;

	vector<int> ids = generateRandomCardIds();
	
	while (amount > 0) {
		card newCard;
		float x = (-5) + (2.5 * (amount >= 5 ? 0 : amount >= 3 ? 1 : 2));
		float y = (amount % 2 == 0 ? 1.5 : -2.5);
		newCard.position = vector3{ x, y, 0 };
		newCard.id = ids.back();
		ids.pop_back();
		cards.push_back(newCard);
		amount--;
	}
}

void clearCards(void) {
	cards.clear();
}
/*-[Keyboard Callback]-------------------------------------------------------*/
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a': // lowercase character 'a'
		cout << "You just pressed 'a'" << endl;
		moveCamera(1.0, 0.0, 0.0);
		break;
	case 'd': // lowercase character 'd'
		cout << "You just pressed 'd'" << endl;
		moveCamera(-1.0, 0.0, 0.0);
		break;
	case 'w': // lowercase character 'w'
		cout << "You just pressed 'w'" << endl;
		moveCamera(0.0, -1.0, 0);
		break;
	case 's': // lowercase character 's'
		cout << "You just pressed 's'" << endl;
		moveCamera(0.0, 1.0, 0);
		break;
	case 'r':
		cout << "You just pressed 'r'" << endl;
		moveCamera(0.0, 0.0, 1.0);
		break;
	case 'f':
		cout << "You just pressed 'f'" << endl;
		moveCamera(0.0, 0.0, -1.0);
		break;
	case ' ':
		cout << "You just pressed 'Space'" << endl;
		clearCards();
		createCards();
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
	} else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		cout << "Camera Position: " << "[" << position.x << ", " << position.y << ", " << position.z << "]" << endl;
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

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowPosition(500, 500); //determines the initial position of the window
	glutInitWindowSize(800, 600);	  //determines the size of the window
	windowid = glutCreateWindow("Memory Game"); // create and name window
	createCards();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);

	// register callbacks
	initTextures();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(onMouseClick);
	glutReshapeFunc(reshapeFunc);
	glutIdleFunc(idleFunc);

	glutMainLoop(); // start the main loop of GLUT
	
	return 0;
}