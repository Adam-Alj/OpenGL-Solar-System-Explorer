
/************************************************************************************

SolarSystem.cpp

Adam Al-Jumaily
A00362836

*************************************************************************************/



/* include the library header files */
#include <stdlib.h>
#include <freeglut.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


/* Definitions */
#define PI 3.14159265
#define DEG_TO_RAD PI/180.0

using namespace std;

// Quadratic to draw gluSpheres.
GLUquadric *quad = gluNewQuadric();

// Feature control variables.
bool coronaEnabled = true;
bool starsEnabled = true;
bool shieldsEnabled = false;
bool orbitalsEnabled = false;
bool superNova = false;

/* Variables to control planet positions.


  Format: {Distance From Sun, rotation degrees, rotation speed}
  
  */
float planetControlVariables[10][3] = {
	// Mercury
	{64, 0, .6985},
	// Venus
	{95, 0, .6563},
	// Earth
	{150, 0, .624},
	// Mars
	{180, 0, .6},
	//Jupiter
	{275, 0, .595},
	//Saturn
	{375, 0, .521},
	//Uranus
	{450, 0, .19},
	//Neptune
	{525, 0, .07},
	//Pluto
	{600, 0, .03},
	//Moon
	{12, 0, 1}
};

// Camera position vertex.
float cameraPosition[3]  = {
	0, 0, 800
};

// Camera lookat vertex.
float lookAtPosition[3] = {
	0, 0, 790
};

// Control angles for looking with mouse.
float cameraAngles[3] = {
	0, 0, 0
};

// Holds the mouse position.
int prevMousePos[2] = {0, 0};

// Holds 500 star points in 6 planes.
float starPoints[6][500][3];

// Corona lines and variables to control their growth.
float coronaLines[20000][3];
float coronaGrowthControl[20000][3];

// Control variables for shield color.
float shieldAlhpa = 0;
float shieldAlphaInc = 0.005;

// Vectors of topology for the enterprise.
vector<vector<GLfloat>>* enterpriseVertices;
vector<vector<int>>* enterpriseTriangles;

/*  
	function: parseString

	Reads in a string of four tokens delimited by spaces and
	returns a pointer to a generated array.

*/
string* parseString(string line) {
	int lineIndex = 2;
	string substring;
	string* stringArr = new string[4];
	stringArr[0] = line[0];
	for (int i = 1; i < 4; i++) {
		substring = line.substr(lineIndex, line.find_first_of(" ", lineIndex) - lineIndex);
		stringArr[i] = substring;
		lineIndex = line.find_first_of(" ", lineIndex) + 1;
	}
	return stringArr;
}

/*
	function initializeCorona

	Fills an array with randomly generated vertices within a sphere 
	at location 0,0,0 with radius 54. Generated growth control variables
	in a seperate array for animation.
*/
void initializeCorona() {
	float theta;
	float alpha;
	for (int i = 0; i < sizeof(coronaLines)/sizeof(coronaLines[0]); i++) {
		theta = fmod(rand(), 2*PI);
		alpha = fmod(rand(), PI) - (PI/2);
		coronaLines[i][0] = 54*cos(theta) * cos(alpha);
		coronaLines[i][1] = 54*sin(alpha);
		coronaLines[i][2] = 54 * sin(theta) * cos(alpha);
		coronaGrowthControl[i][0] = fmod(rand(), 0.15) + 0.8;
		coronaGrowthControl[i][1] = 1;
		coronaGrowthControl[i][2] = rand()%10 / 1000 + 0.01;
	}
}


/*
	function initializeEnterprise

	Reads in from the enterprise file and fills a vector with information about
	vertex location and mesh topology.
*/
void initializeEnterprise() {
	string* splitLine = nullptr;
	string line;
	int numOfVertices = 0;
	int numOfTriangles = 0;
	ifstream infile;
	infile.open("enterprise.txt");

	if (!infile) {
		std::cout << "cant find it" << endl;
	}

	// Determine number of vertices.
	while (!infile.eof()) {
		getline(infile, line);
		splitLine = parseString(line);
		if (splitLine[0] == "v") {
			numOfVertices++;
		}
		else if (splitLine[0] == "f") {
			numOfTriangles++;
		}
	}
	infile.close();

	// Make a 2D vector to hold these vertices, add values.
	vector<vector<float>>* vertices = new vector<vector<float>>(numOfVertices, vector<float>(3));
	vector<vector<int>>* triangles = new vector<vector<int>>(numOfTriangles, vector<int>(3));

	infile.open("enterprise.txt");
	for (int i = 0; i < numOfVertices; i++) {
		getline(infile, line);
		splitLine = parseString(line);
		if (splitLine[0] == "v") {
			(*vertices)[i][0] = stof(splitLine[1], nullptr);
			(*vertices)[i][1] = stof(splitLine[2], nullptr);
			(*vertices)[i][2] = stof(splitLine[3], nullptr);
		}
		delete[] splitLine;
	}
	infile.close();

	enterpriseVertices = vertices;

	// Add values to the triangle topology vector.
	infile.open("enterprise.txt");
	int count = 0;
	while(!infile.eof()){
		getline(infile, line);
		splitLine = parseString(line);
		if (splitLine[0] == "f") {
			(*triangles)[count][0] = stoi(splitLine[1], nullptr);
			(*triangles)[count][1] = stoi(splitLine[2], nullptr);
			(*triangles)[count][2] = stoi(splitLine[3], nullptr);
			count++;
		}
		delete[] splitLine;
	}
	infile.close();

	enterpriseTriangles = triangles;

}

/*
	function initializeStars

	Generates random vertexes on 6 3D rectangles that encapsulate
	the solar system.
*/
void initializeStars() {
	for (int i = 0; i < 500; i++) {

		if (i < 250) {
			starPoints[0][i][0] = -1000;
			starPoints[0][i][1] = rand() % 2000 - 1000;
			starPoints[0][i][2] = rand() % 2000 - 1000;

			starPoints[1][i][0] = 1000;
			starPoints[1][i][1] = rand() % 2000 - 1000;
			starPoints[1][i][2] = rand() % 2000 - 1000;

			starPoints[2][i][0] = rand() % 2000 - 1000;
			starPoints[2][i][1] = -1000;
			starPoints[2][i][2] = rand() % 2000 - 1000;

			starPoints[3][i][0] = rand() % 2000 - 1000;
			starPoints[3][i][1] = 1000;
			starPoints[3][i][2] = rand() % 2000 - 1000;

			starPoints[4][i][0] = rand() % 2000 - 1000;
			starPoints[4][i][1] = rand() % 2000 - 1000;
			starPoints[4][i][2] = 1000;

			starPoints[5][i][0] = rand() % 2000 - 1000;
			starPoints[5][i][1] = rand() % 2000 - 1000;
			starPoints[5][i][2] = -1000;
		}
		else {
			starPoints[0][i][0] = rand() % 1000 - 2000;
			starPoints[0][i][1] = rand() % 2000 - 1000;
			starPoints[0][i][2] = rand() % 2000 - 1000;

			starPoints[1][i][0] = rand() % 1000 + 1000;
			starPoints[1][i][1] = rand() % 2000 - 1000;
			starPoints[1][i][2] = rand() % 2000 - 1000;

			starPoints[2][i][0] = rand() % 2000 - 1000;
			starPoints[2][i][1] = rand() % 1000 - 2000;
			starPoints[2][i][2] = rand() % 2000 - 1000;

			starPoints[3][i][0] = rand() % 2000 - 1000;
			starPoints[3][i][1] = rand() % 1000 + 1000;
			starPoints[3][i][2] = rand() % 2000 - 1000;

			starPoints[4][i][0] = rand() % 2000 - 1000;
			starPoints[4][i][1] = rand() % 2000 - 1000;
			starPoints[4][i][2] = rand() % 1000 + 1000;

			starPoints[5][i][0] = rand() % 2000 - 1000;
			starPoints[5][i][1] = rand() % 2000 - 1000;
			starPoints[5][i][2] = rand() % 1000 - 2000;
		}
	}
}

/*
	function: displayControls

	Dumps the camera and scene controls to the console window.
*/
void displayControls() {
	cout << "Scene Controls" << endl;
	cout << "--------------" << endl << endl;
	cout << "r:  toggle rings" << endl
		<< "s:  toggle stars" << endl
		<< "c:  toggle the sun's corona" << endl
		<< "k:  toggle shields" << endl
		<< "n:  toggle/reset supernova" << endl << endl;
	cout << "Camera Controls" << endl;
	cout << "---------------" << endl << endl;
	cout << "Page Up:     Move forward" << endl
		<< "Pade Down:   Move Backwards" << endl
		<< "Up Arrow:    Move up" << endl
		<< "Down Arrow:  Move down" << endl
		<< "Right Arrow: Move right" << endl
		<< "Left Arrow:  Move left" << endl
		<< "Down Arrow:  Move down" << endl
		<< "Mouse:       Look around" << endl
		<< "Space:       Center the camera" << endl;
}

/*
	function: drawEnterprise

	Uses the vertices and triangle topology to draw the enterprise.
*/
void drawEnterprise() {
	float vertex1[3], vertex2[3], vertex3[3];
	for (int i = 0; i < (*enterpriseTriangles).size(); i++) {
		for (int j = 0; j < 3; j++) {
			vertex1[j] = (*enterpriseVertices)[(*enterpriseTriangles)[i][0] - 1][j] * 3 + cameraPosition[j];
			vertex2[j] = (*enterpriseVertices)[(*enterpriseTriangles)[i][1] - 1][j] * 3 + cameraPosition[j];
			vertex3[j] = (*enterpriseVertices)[(*enterpriseTriangles)[i][2] - 1][j] * 3 + cameraPosition[j];
		}
		vertex1[2] = vertex1[2] - 7;
		vertex2[2] = vertex2[2] - 7;
		vertex3[2] = vertex3[2] - 7;
		vertex1[1] = vertex1[1] - 5;
		vertex2[1] = vertex2[1] - 5;
		vertex3[1] = vertex3[1] - 5;

		glColor3f(.8, .8, .8);
		glBegin(GL_TRIANGLES);
		glVertex3fv(vertex1);
		glVertex3fv(vertex2);
		glVertex3fv(vertex3);
		glEnd();

	}
}

/*
	function drawOrbitals

	Uses line loop to draw the orbitals of the planets and moon.
*/
void drawOrbitals() {
	// Variables for the number of lines that will make up the circle.
	int sections = 180;
	float degreesPerSection = 360 / sections;

	// Draws the orbitals for all the planets.
	glColor4f(1, 1, 1, 0.5);
	for (int i = 0; i < sizeof(planetControlVariables)/sizeof(planetControlVariables[0]) - 1; i++) {
		glBegin(GL_LINE_LOOP);
		for (int j = 1; j < sections+1; j++) {
			glVertex3f(planetControlVariables[i][0] * sin(DEG_TO_RAD*(j*degreesPerSection)), 0, planetControlVariables[i][0] * cos(DEG_TO_RAD*(j*degreesPerSection)));
		}
		glEnd();
	}

	// Variables for the current position of earth.
	float earthPosX = planetControlVariables[2][0] * sin(DEG_TO_RAD*planetControlVariables[2][1]);
	float earthPosZ = planetControlVariables[2][0] * cos(DEG_TO_RAD*planetControlVariables[2][1]);

	// Draws the orbital for the moon.
	glBegin(GL_LINE_LOOP);
	for (int i = 1; i < sections + 1; i++) {
		glVertex3f(earthPosX + planetControlVariables[9][0] * sin(DEG_TO_RAD*(i*degreesPerSection)), 0, earthPosZ + planetControlVariables[9][0] * cos(DEG_TO_RAD*(i*degreesPerSection)));
	}
	glEnd();

}


void drawStars() {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 500; j++) {
			float alpha = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			glColor4f(1, 1, 1, alpha);
			glBegin(GL_POINTS);
			glVertex3fv(starPoints[i][j]);
			glEnd();
		}
	}
}

void drawSun() {
	glPushMatrix();
	glColor4f(1, 1, 0, 0.95);
	glTranslatef(0, 0, 0);
	gluSphere(quad, 27, 50, 50);
	glPopMatrix();
}

void drawCorona() {
	for (int i = 0; i < sizeof(coronaLines) / sizeof(coronaLines[0]); i++) {
		glBegin(GL_LINES);
		glColor4f(1, 1, 0, 1);
		glVertex3f(0, 0, 0);
		glColor4f(1, 0.27, 0, 0);
		glVertex3f((coronaGrowthControl[i][0] * coronaLines[i][0]) * (1 - coronaGrowthControl[i][1]) + coronaGrowthControl[i][1] * coronaLines[i][0],
			(coronaGrowthControl[i][0] * coronaLines[i][1]) * (1 - coronaGrowthControl[i][1]) + coronaGrowthControl[i][1] * coronaLines[i][1],
			(coronaGrowthControl[i][0] * coronaLines[i][2]) * (1 - coronaGrowthControl[i][1]) + coronaGrowthControl[i][1] * coronaLines[i][2]);
		glEnd();
	}
}

void drawMercury() {
	glPushMatrix();
	glColor3f(.6, .6, .4);
	glTranslatef(planetControlVariables[0][0] * sin(DEG_TO_RAD*planetControlVariables[0][1]), 0, planetControlVariables[0][0] * cos(DEG_TO_RAD*planetControlVariables[0][1]));
	gluSphere(quad, 2, 50, 50);
	glPopMatrix();
}

void drawVenus() {
	glPushMatrix();
	glColor3f(.8, .6, 0);
	glTranslatef(planetControlVariables[1][0] * sin(DEG_TO_RAD*planetControlVariables[1][1]), 0, planetControlVariables[1][0] * cos(DEG_TO_RAD*planetControlVariables[1][1]));
	gluSphere(quad, 4.5, 50, 50);
	glPopMatrix();
}

void drawEarth() {
	glPushMatrix();
	glColor3f(0, .6, 1);
	glTranslatef(planetControlVariables[2][0] * sin(DEG_TO_RAD*planetControlVariables[2][1]), 0, planetControlVariables[2][0] * cos(DEG_TO_RAD*planetControlVariables[2][1]));	
	gluSphere(quad, 5, 50, 50);
	glPopMatrix();
}

void drawMars() {
	glPushMatrix();
	glColor3f(.6, 0.2, 0);
	glTranslatef(planetControlVariables[3][0] * sin(DEG_TO_RAD*planetControlVariables[3][1]), 0, planetControlVariables[3][0] * cos(DEG_TO_RAD*planetControlVariables[3][1]));
	gluSphere(quad, 3, 50, 50);
	glPopMatrix();
}

void drawJupiter() {
	glPushMatrix();
	glColor3f(.8, .4, 0);
	glTranslatef(planetControlVariables[4][0] * sin(DEG_TO_RAD*planetControlVariables[4][1]), 0, planetControlVariables[4][0] * cos(DEG_TO_RAD*planetControlVariables[4][1]));	
	gluSphere(quad, 30, 50, 50);
	glPopMatrix();
}

void drawSaturn() {
	glPushMatrix();
	glColor3f(1, .8, .4);
	glTranslatef(planetControlVariables[5][0] * sin(DEG_TO_RAD*planetControlVariables[5][1]), 0, planetControlVariables[5][0] * cos(DEG_TO_RAD*planetControlVariables[5][1]));	
	gluSphere(quad, 25, 50, 50);
	glPopMatrix();
}

void drawUranus() {
	glPushMatrix();
	glColor3f(.6, .8, 1);
	glTranslatef(planetControlVariables[6][0] * sin(DEG_TO_RAD*planetControlVariables[6][1]), 0, planetControlVariables[6][0] * cos(DEG_TO_RAD*planetControlVariables[6][1]));	
	gluSphere(quad, 15, 50, 50);
	glPopMatrix();
}

void drawNeptune() {
	glPushMatrix();
	glColor3f(.2, .6, 1);
	glTranslatef(planetControlVariables[7][0] * sin(DEG_TO_RAD*planetControlVariables[7][1]), 0, planetControlVariables[7][0] * cos(DEG_TO_RAD*planetControlVariables[7][1]));
	gluSphere(quad, 15, 50, 50);
	glPopMatrix();
}

void drawPluto() {
	glPushMatrix();
	glColor3f(.6, 1, .8);
	glTranslatef(planetControlVariables[8][0] * sin(DEG_TO_RAD*planetControlVariables[8][1]), 0, planetControlVariables[8][0] * cos(DEG_TO_RAD*planetControlVariables[8][1]));
	gluSphere(quad, 2, 50, 50);
	glPopMatrix();
}

void drawMoon() {
	glPushMatrix();
	glColor3f(.85, .85, .85);
	glTranslatef(planetControlVariables[2][0] * sin(DEG_TO_RAD*planetControlVariables[2][1]) + planetControlVariables[9][0] * sin(DEG_TO_RAD*planetControlVariables[9][1]),
		0, 
		planetControlVariables[2][0] * cos(DEG_TO_RAD*planetControlVariables[2][1]) + planetControlVariables[9][0] * cos(DEG_TO_RAD*planetControlVariables[9][1]));
	gluSphere(quad, 2, 50, 50);
	glPopMatrix();
}

void drawShield() {
	glPushMatrix();
	glColor4f(0.54*shieldAlhpa + (1-shieldAlhpa)*0.82, 0.73*shieldAlhpa + (1 - shieldAlhpa)*0.54, 0.87*shieldAlhpa + (1 - shieldAlhpa)*0.87, 0.4);
	glTranslatef(cameraPosition[0], cameraPosition[1] - 2, cameraPosition[2] - 3);
	gluSphere(quad, 1, 50, 50);
	glPopMatrix();
}

/*
	function: changeCameraPos

	linearly modifies the camera and lookat positions.
*/
void changeCameraPos(float posX, float posY, float posZ, float lookX, float lookY, float lookZ) {
	cameraPosition[0] += posX;
	cameraPosition[1] += posY;
	cameraPosition[2] += posZ;

	lookAtPosition[0] += lookX;
	lookAtPosition[1] += lookY;
	lookAtPosition[2] += lookZ;
}

/*
	function: centerCamera

	centers the camera to look parallel to the z axis.
*/
void centerCamera() {
	lookAtPosition[0] = cameraPosition[0];
	lookAtPosition[1] = cameraPosition[1];
	lookAtPosition[2] = cameraPosition[2] - 10;
	for (int i = 0; i < 3; i++) {
		cameraAngles[i] = 0;
	}
}

/************************************************************************

Function:		myDisplay

Description:	Display callback, clears frame buffer and depth buffer,
positions the camera and draws the cube.

*************************************************************************/
void myDisplay(void)
{

	// clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// load the identity matrix into the model view matrix
	glLoadIdentity();

	// set the camera position
	gluLookAt(
		cameraPosition[0], cameraPosition[1], cameraPosition[2],
		lookAtPosition[0], lookAtPosition[1], lookAtPosition[2],
		0, 1, 0);

	drawEnterprise();
	if (starsEnabled) {
		drawStars();
	}
	drawSun();
	drawMercury();
	drawVenus();
	drawEarth();
	drawMars();
	drawJupiter();
	drawSaturn();
	drawUranus();
	drawNeptune();
	drawPluto();
	drawMoon();
	if (orbitalsEnabled) {
		drawOrbitals();
	}
	if (coronaEnabled) {
		drawCorona();
	}
	if (shieldsEnabled) {
		drawShield();
	}
	
	// swap the drawing buffers
	glutSwapBuffers();
}


/************************************************************************

Function:		myIdle

Description:	Rotates planets/changes shield color/ pulses corona.

*************************************************************************/
void myIdle()
{
	// Rotates the planets
	for (int i = 0; i < 10; i++) {
		planetControlVariables[i][1] += planetControlVariables[i][2];
		if (planetControlVariables[i][1] >= 360) {
			planetControlVariables[i][1] = 0;
		}
	}

	// Controls pulsation of the corona and supernova.
	for (int i = 0; i < sizeof(coronaLines) / sizeof(coronaLines[0]); i++) {
		if (!superNova) {
			coronaGrowthControl[i][1] -= coronaGrowthControl[i][2];
			if (coronaGrowthControl[i][1] > 1 || coronaGrowthControl[i][1] < 0) {
				coronaGrowthControl[i][2] = -coronaGrowthControl[i][2];
			}
		}
		else {
			if (coronaGrowthControl[i][2] > 0) {
				coronaGrowthControl[i][2] = -coronaGrowthControl[i][2];
			}
			coronaGrowthControl[i][1] += 0.05;
		}
	}

	// Makes the shield change colors.
	if (shieldsEnabled) {
		shieldAlhpa += shieldAlphaInc;
		if (shieldAlhpa <= 0 || shieldAlhpa >= 1) {
			shieldAlphaInc = -shieldAlphaInc;
		}
	}

	// redraw the new state
	glutPostRedisplay();
}

// Control variables for moving the camera with the mouse.
float upwardAngleIncrement = 0.5;
float downAngleIncrement = 0.5;
float sideAngleIncrement = 0.5;

/*
	function: mouseMove

	passive mouse callback function.
	Used to look around the scene with the mouse.
*/
void mouseMove(int x, int y) {
	
	if (y > prevMousePos[1]) {
		if (upwardAngleIncrement == 0) {
			upwardAngleIncrement = 0.5;
		}
		cameraAngles[1] += downAngleIncrement;
		cameraAngles[2] += downAngleIncrement;
		lookAtPosition[1] = cameraPosition[1] - sin(DEG_TO_RAD*cameraAngles[1]);
		lookAtPosition[2] = cameraPosition[2] - cos(DEG_TO_RAD*cameraAngles[2]);
	}
	else if (y < prevMousePos[1]) {
		if (downAngleIncrement == 0) {
			downAngleIncrement = 0.5;
		}
		cameraAngles[1] -= upwardAngleIncrement;
		cameraAngles[2] -= upwardAngleIncrement;
		lookAtPosition[1] = cameraPosition[1] - sin(DEG_TO_RAD*cameraAngles[1]);
		lookAtPosition[2] = cameraPosition[2] - cos(DEG_TO_RAD*cameraAngles[2]);
	} 
	if (x > prevMousePos[0]) {
		cameraAngles[0] -= sideAngleIncrement;
		cameraAngles[2] -= sideAngleIncrement;
		lookAtPosition[0] = cameraPosition[0] - sin(DEG_TO_RAD * cameraAngles[0]);
		lookAtPosition[2] = cameraPosition[2] - cos(DEG_TO_RAD * cameraAngles[2]);
	}
	else if (x < prevMousePos[0]) {
		cameraAngles[0] += sideAngleIncrement;
		cameraAngles[2] += sideAngleIncrement;
		lookAtPosition[0] = cameraPosition[0] - sin(DEG_TO_RAD * cameraAngles[0]);
		lookAtPosition[2] = cameraPosition[2] - cos(DEG_TO_RAD * cameraAngles[2]);
	}
	
	if (lookAtPosition[2] >= 799.670 && lookAtPosition[1] <= -.948) {
		lookAtPosition[2] = 799.670;
		lookAtPosition[1] = -.948;
		downAngleIncrement = 0;
	}
	if (lookAtPosition[2] >= 799.670 && lookAtPosition[1] >= 0.88) {
		lookAtPosition[2] = 799.670;
		lookAtPosition[1] = 0.88;
		upwardAngleIncrement = 0;
	}
	prevMousePos[0] = x;
	prevMousePos[1] = y;
	glutPostRedisplay();
}

/*
	function: keyboard

	callback function for regular character keystrokes.
	Used to enable and disable scene elements.
*/
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'S':
	case 's':
		starsEnabled = !starsEnabled;
		break;
	case 'C':
	case 'c':
		coronaEnabled = !coronaEnabled;
		break;
	case 'K':
	case 'k':
		shieldsEnabled = !shieldsEnabled;
		break;
	case 'R':
	case 'r':
		orbitalsEnabled = !orbitalsEnabled;
		break;
	case 'N':
	case 'n':
		if (superNova) {
			for (int i = 0; i < sizeof(coronaLines) / sizeof(coronaLines[0]); i++) {
				coronaGrowthControl[i][1] = 0;
			}
		}
		superNova = !superNova;
		break;
	case ' ':
		centerCamera();
		break;
	}
	glutPostRedisplay();
}

/*
	function: specialInput

	callback function for special key presses.
	Used for camera control from the keyboard.
*/
void specialInput(int key, int x, int y)
{

	switch (key)
	{
	case GLUT_KEY_PAGE_UP:
		changeCameraPos(0, 0, -5, 0, 0, -5);
		break;
	case GLUT_KEY_PAGE_DOWN:
		changeCameraPos(0, 0, 5, 0, 0, 5);
		break;
	case GLUT_KEY_UP:
		changeCameraPos(0, 5, 0, 0, 5, 0);
		break;
	case GLUT_KEY_DOWN:
		changeCameraPos(0, -5, 0, 0, -5, 0);
		break;
	case GLUT_KEY_LEFT:
		changeCameraPos(-5, 0, 0, -5, 0, 0);
		break;
	case GLUT_KEY_RIGHT:
		changeCameraPos(5, 0, 0, 5, 0, 0);
		break;
	}

	glutPostRedisplay();
}


/************************************************************************

Function:		initializeGL

Description:	Initializes the OpenGL rendering context for display.

*************************************************************************/
void initializeGL(void)
{
	// enable depth testing
	glEnable(GL_DEPTH_TEST);

	// set background color to be black
	glClearColor(0, 0, 0, 1.0);

	// change into projection mode so that we can change the camera properties
	glMatrixMode(GL_PROJECTION);

	// load the identity matrix into the projection matrix
	glLoadIdentity();

	// set window mode to 2D orthographic 
	gluPerspective(90, 1, 1, 5000);

	// initialize randomly generated star locations.
	initializeStars();

	// Add the enterprise to an array
	initializeEnterprise();

	// Initialize randomly generated corona lines.
	initializeCorona();

	// change into model-view mode so that we can change the object positions
	glMatrixMode(GL_MODELVIEW);
}



/************************************************************************

Function:		main

Description:	Sets up the openGL rendering context and the windowing
system, then begins the display loop.

*************************************************************************/
void main(int argc, char** argv)
{
	// Dump controls.
	displayControls();
	// initialize the toolkit
	glutInit(&argc, argv);
	// set display mode
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	// set window size
	glutInitWindowSize(1000, 1000);
	// set window position on screen
	glutInitWindowPosition(100, 0);
	// open the screen window
	glutCreateWindow("colorcube");
	// enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// register redraw function
	glutDisplayFunc(myDisplay);
	// register the idle function
	glutIdleFunc(myIdle);
	// register the keyboard function
	glutKeyboardFunc(keyboard);
	// register special function.
	glutSpecialFunc(specialInput);
	// register passive motion function.
	glutPassiveMotionFunc(mouseMove);
	// initialize the rendering context
	initializeGL();
	// Seed rand()
	srand(time(NULL));

	// go into a perpetual loop
	glutMainLoop();
}