#include <algorithm>

#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>
#include <random>
#include <vector>

using namespace std;

// HAS TO BE EVEN
#define MAX_CARDS 6

#define SELECT_BORDER_THICKNESS 0.1

struct vector2 {
	float x;
	float y;
};

struct vector3 {
	float x;
	float y;
	float z;
};

struct textureMap {
	vector2 botLeft;
	vector2 topLeft;
	vector2 topRight;
	vector2 botRight;
};

struct card {
	int id{ 0 };
	vector3 position{ 0, 0, 0 };
	int groupId{ 0 };
	textureMap texture;
	float size;
};

vector3 position{ 0, 0, -11 };

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
int card_rows = 2;
bool turning = false;
int angle = 0;

// Data read from the header of the BMP file
unsigned char header[54]; // Each BMP file begins by a 54-bytes header
unsigned int dataPos;     // Position in the file where the actual data begins
unsigned int imageWidth, imageHeight;
unsigned int imageSize;   // = width*height*3
// Actual RGB data
unsigned char* imageData;

vector<card> cards;
int selectedId{ 0 };
int cardRows{ 0 };
int cardCols{ 0 };

// image file Strings
static std::string cardBack = "img/test.bmp";
static std::string background = "img/background.bmp";
static std::string cardFront = "img/front_textures.bmp";

// converted to C-Strings
char* cardBackPath = &cardBack[0];
char* backgroundPath = &background[0];
char* cardFrontPath = &cardFront[0];

// texture names
static GLuint cardBackName;
static GLuint backgroundName;
static GLuint cardFrontName;

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

void initBackgroundTexture(void) {
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

void initCardBackTexture(void) {
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

void initCardFrontTexture(void) {
	loadBMP_custom(cardFrontPath);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &cardFrontName);
	glBindTexture(GL_TEXTURE_2D, cardFrontName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, imageData);
}

void initTextures(void) {
	initCardBackTexture();
	cout << "CardBack ID: " << cardBackName << endl;
	initCardFrontTexture();
	cout << "Front ID: " << cardFrontName << endl;
	initBackgroundTexture();
	cout << "Background ID: " << backgroundName << endl;
}

void displayBackground(void) {
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, backgroundName);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-7.5, -5.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-7.5, 5.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(7.5, 5.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(7.5, -5.0, 0.0);
	glEnd();
	glFlush();
	glDisable(GL_TEXTURE_2D);
}

void displayCard(card c) {
	vector3 botLeft = c.position;

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, cardFrontName);
	switch (c.id) {
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
		glTexCoord2f(c.texture.botLeft.x, c.texture.botLeft.y); glVertex3f(botLeft.x, botLeft.y, botLeft.z);
		glTexCoord2f(c.texture.topLeft.x, c.texture.topLeft.y); glVertex3f(botLeft.x, botLeft.y + c.size, botLeft.z);
		glTexCoord2f(c.texture.topRight.x, c.texture.topRight.y); glVertex3f(botLeft.x + c.size, botLeft.y + c.size, botLeft.z);
		glTexCoord2f(c.texture.botRight.x, c.texture.botRight.y); glVertex3f(botLeft.x + c.size, botLeft.y, botLeft.z);
	glEnd();
	
	glFlush();
	glDisable(GL_TEXTURE_2D);

	if (c.id == selectedId) {
		glPushMatrix();
		glColor3f(0.9f, 0.91f, 0.14f);
		glBegin(GL_QUADS);
		glVertex3f(botLeft.x - SELECT_BORDER_THICKNESS, botLeft.y - SELECT_BORDER_THICKNESS, botLeft.z);
		glVertex3f(botLeft.x - SELECT_BORDER_THICKNESS, botLeft.y + c.size + SELECT_BORDER_THICKNESS, botLeft.z);
		glVertex3f(botLeft.x + c.size + SELECT_BORDER_THICKNESS, botLeft.y + c.size + SELECT_BORDER_THICKNESS, botLeft.z);
		glVertex3f(botLeft.x + c.size + SELECT_BORDER_THICKNESS, botLeft.y - SELECT_BORDER_THICKNESS, botLeft.z);
		glEnd();
		glPopMatrix();
	}
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

vector<int> generateRandomGroupIds(void) {
	vector<int> ids;
	for (int i = 0; i < (MAX_CARDS / 2); i++) {
		ids.push_back(i);
		ids.push_back(i);
	}
	random_device rd;
	mt19937 g(rd());
	shuffle(ids.begin(), ids.end(), g);
	return ids;
}

textureMap mapTexture(int groupId) {
	textureMap map;

	switch (groupId) {
	case 0:
		map.botLeft = vector2{ 0.0, 0.75 };
		map.topLeft = vector2{ 0.0, 1.0 };
		map.topRight = vector2{ 0.25, 1.0 };
		map.botRight = vector2{ 0.25, 0.75 };
		break;
	case 1:
		map.botLeft = vector2{ 0.25, 0.75 };
		map.topLeft = vector2{ 0.25, 1.0 };
		map.topRight = vector2{ 0.5, 1.0 };
		map.botRight = vector2{ 0.5, 0.75 };
		break;
	case 2:
		map.botLeft = vector2{ 0.5, 0.75 };
		map.topLeft = vector2{ 0.5, 1.0 };
		map.topRight = vector2{ 0.75, 1.0 };
		map.botRight = vector2{ 0.75, 0.75 };
		break;
	case 3:
		map.botLeft = vector2{ 0.75, 0.75 };
		map.topLeft = vector2{ 0.75, 1.0 };
		map.topRight = vector2{ 1.0, 1.0 };
		map.botRight = vector2{ 1.0, 0.75 };
		break;
	case 4:
		map.botLeft = vector2{ 0.0, 0.5 };
		map.topLeft = vector2{ 0.0, 0.75 };
		map.topRight = vector2{ 0.25, 0.75 };
		map.botRight = vector2{ 0.25, 0.5 };
		break;
	case 5:
		map.botLeft = vector2{ 0.25, 0.5 };
		map.topLeft = vector2{ 0.25, 0.75 };
		map.topRight = vector2{ 0.5, 0.75 };
		map.botRight = vector2{ 0.5, 0.5 };
		break;
	case 6:
		map.botLeft = vector2{ 0.5, 0.5 };
		map.topLeft = vector2{ 0.5, 0.75 };
		map.topRight = vector2{ 0.75, 0.75 };
		map.botRight = vector2{ 0.75, 0.5 };
		break;
	case 7:
		map.botLeft = vector2{ 0.75, 0.5 };
		map.topLeft = vector2{ 0.75, 0.75 };
		map.topRight = vector2{ 1.0, 0.75 };
		map.botRight = vector2{ 1.0, 0.5 };
		break;
	default:
		map.botLeft = vector2{ 0.0, 0.0 };
		map.topLeft = vector2{ 0.0, 1.0 };
		map.topRight = vector2{ 1.0, 1.0 };
		map.botRight = vector2{ 1.0, 0.0 };
		break;
	}

	return map;
}

void createCards(void) {
	vector2 botLeftViewport{ -6, -3.5 };

	if (MAX_CARDS % 4 == 0)
		cardRows = 4;
	else if (MAX_CARDS % 3 == 0)
		cardRows = 3;
	else
		cardRows = 2;
	
	cardCols = MAX_CARDS / cardRows;
	float colSize = 12.0f / cardCols;
	float rowSize = 7.0f / cardRows;
	vector<int> groupIds = generateRandomGroupIds();
	float cardSize = (colSize < rowSize) ? colSize * 0.8f : rowSize * 0.8f;

	for (int i = 0; i < cardRows; i++) {
		for (int j = 0; j < MAX_CARDS / cardRows; j++) {
			card newCard;
			newCard.id = cardCols * i + j;
			newCard.groupId = groupIds.back();
			groupIds.pop_back();
			newCard.position = vector3{ botLeftViewport.x + (colSize * j), botLeftViewport.y + (rowSize * i), 0 };
			newCard.texture = mapTexture(newCard.groupId);
			newCard.size = cardSize;
			cards.push_back(newCard);
		}
	}
}

void clearCards(void) {
	cards.clear();
}

int getClickedCard(vector2 clickedPos) {
	float colSize = 12.0f / cardCols;
	float rowSize = 7.0f / cardRows;

	int col = ceil(((clickedPos.x + 1) * 12.0f / 2.0f) / colSize) - 1;
	int row = ceil(((clickedPos.y + 1) * 7.0f / 2.0f) / rowSize) - 1;
	int sid = cardCols * row + col;

	return sid;
}

/*-[Keyboard Callback]-------------------------------------------------------*/
void keyboard(unsigned char key, int x, int y) {
	int newSelectedId;
	switch (key) {
	case 'a': // lowercase character 'a'
		cout << "You just pressed 'a'" << endl;
		//moveCamera(1.0, 0.0, 0.0);
		newSelectedId = selectedId - card_rows;
		if (newSelectedId >= 0) {
			selectedId = newSelectedId;
		}
		break;
	case 'd': // lowercase character 'd'
		cout << "You just pressed 'd'" << endl;
		//moveCamera(-1.0, 0.0, 0.0);
		newSelectedId = selectedId + card_rows;
		if (newSelectedId < MAX_CARDS) {
			selectedId = newSelectedId;
		}
		break;
	case 'w': // lowercase character 'w'
		cout << "You just pressed 'w'" << endl;
		//moveCamera(0.0, -1.0, 0);
		newSelectedId = selectedId - 1;
		if (newSelectedId % card_rows == 0) {
			selectedId = newSelectedId;
		}
		break;
	case 's': // lowercase character 's'
		cout << "You just pressed 's'" << endl;
		//moveCamera(0.0, 1.0, 0);
		newSelectedId = selectedId + 1;
		if (newSelectedId % card_rows == (card_rows - 1)) {
			selectedId = newSelectedId;
		}
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

	int keyid = (int)key - 48;
	selectedId = keyid;
	cout << "Key: " << keyid << endl;

	glutPostRedisplay();
}

/*-[MouseClick Callback]-----------------------------------------------------*/
void onMouseClick(int button, int state, int x, int y) {
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		cout << "Middle button clicked at position "
			<< "x: " << x << " y: " << y << endl;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		float newx = x / (WINDOW_WIDTH / 2.0f) - 1;
		float newy = -(y / (WINDOW_HEIGHT / 2.0f) - 1);

		selectedId = getClickedCard(vector2{ newx, newy });
		turning = true;
	}
}

/*-[Reshape Callback]--------------------------------------------------------*/
void reshapeFunc(int x, int y) {
	if (y == 0 || x == 0) return;  //Nothing is visible then, so return

	WINDOW_WIDTH = x;
	WINDOW_HEIGHT = y;

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
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);	  //determines the size of the window
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
	glutFullScreen();

	glutMainLoop(); // start the main loop of GLUT

	return 0;
}