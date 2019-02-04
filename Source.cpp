//---------------------------------------
// Program: fireworks.cpp
// Purpose: To display fireworks
// Author:  Phillip Boudreau
// Date:    Spring 2019
//---------------------------------------
#define _USE_MATH_DEFINES

#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#include <cstdlib>
#include <vector>
#include <cmath>
#include <chrono>
#include <iostream>
#include <random>

using namespace std;

struct Vertex3D {
	int x;
	int y;
	int z;

	//default ctor, initialize point to origin
	Vertex3D() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	//3 int ctor, initialize point to specified location
	Vertex3D(int inX, int inY, int inZ) {
		this->x = inX;
		this->y = inY;
		this->z = inZ;
	}

	//copy ctor
	Vertex3D(const Vertex3D& copyMe) {
		this->x = copyMe.x;
		this->y = copyMe.y;
		this->z = copyMe.z;
	}

	void operator=(const Vertex3D& other)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
	}
};

struct Line3D {
	Vertex3D p1; // <-- point which the line is rotated about
	Vertex3D p2;
	double length;

	Line3D() {
		Vertex3D newP1(0, 0, 0);
		Vertex3D newP2(1, 1, 1);
		this->p1 = newP1;
		this->p2 = newP2;
		this->length = sqrt(3);
	}

	Line3D(const Vertex3D inP1, const Vertex3D inP2) {
		this->p1 = inP1;
		this->p2 = inP2;
		this->length = sqrt(pow(this->p2.x - this->p1.x, 2)
			+ pow(this->p2.y - this->p1.y, 2)
			+ pow(this->p2.z - this->p1.z, 2));
	}

	Line3D(const Line3D& copyMe) {
		Vertex3D newP1(copyMe.p1);
		Vertex3D newP2(copyMe.p2);
		this->p1 = newP1;
		this->p2 = newP2;
		this->length = sqrt(pow(this->p2.x - this->p1.x, 2)
			+ pow(this->p2.y - this->p1.y, 2)
			+ pow(this->p2.z - this->p1.z, 2));
	}

	void rotate(double angleX, double angleY) {
		//save coordinates of point to be rotated about
		int savex = this->p1.x;
		int savey = this->p1.y;
		int savez = this->p1.z;

		//translate line to origin
		this->p1.x = 0;
		this->p1.y = 0;
		this->p1.z = 0;
		this->p2.x -= savex;
		this->p2.y -= savey;
		this->p2.z -= savez;


		//rotate about y axis
		this->p2.x = (this->p2.x * cos(angleY)) - (this->p2.z * sin(angleY));
		this->p2.z = (this->p2.z * cos(angleY)) + (this->p2.x * sin(angleY));
		//rotate about x axis
		this->p2.y = (this->p2.y * cos(angleX)) - (this->p2.z * sin(angleX));
		this->p2.z = (this->p2.z * cos(angleX)) + (this->p2.y * sin(angleX));

		//translate new line back to original position
		this->p1.x = savex;
		this->p1.y = savey;
		this->p1.z = savez;
		this->p2.x += savex;
		this->p2.y += savey;
		this->p2.z += savez;
	}

	void setP1(const Vertex3D& other)
	{
		this->p1 = other;
	}

	void operator=(const Line3D& other)
	{
		this->p1 = other.p1;
		this->p2 = other.p2;
		this->length = other.length;
	}
};

//simple structure to hold individual firework data
struct Firework {
	Vertex3D center;
	int numLines;
	vector<Vertex3D> branchEnds;
	float rgb[3];

	//default ctor, initialize to the "empty" firework
	Firework() {
		Vertex3D defaultCenter(0, 0, 0);
		this->center = defaultCenter;
		this->numLines = 0;
		for (int i = 0; i < 3; i++) {
			this->rgb[i] = 0.0f;
		}

		generatePoints();
	}

	//vertex ctor, initialize location to specified point
	//and generate a random number of {x, y, z} endpoints at random
	//locations
	Firework(const Vertex3D inpCenter) {
		this->center = inpCenter;
		this->numLines = (rand() % 10) + 75;
		for (int i = 0; i < 3; i++) {
			this->rgb[i] = (double)rand() / RAND_MAX;
		}

		generatePoints();
	}

	//copy ctor
	Firework(const Firework& copyMe) {
		this->center = copyMe.center;
		this->numLines = copyMe.numLines;
		this->branchEnds = copyMe.branchEnds;
		for (int i = 0; i < 3; i++) {
			this->rgb[i] = copyMe.rgb[i];
		}
	}

	/*Vertex3D getPoint() 
	{
		double d, x, y, z;
		do {
			x = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
			y = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
			z = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
			d = x * x + y * y + z * z;
		} while (d > 1.0);
		return Vertex3D(x * 500, y * 500, z * 500);
	}*/

	Vertex3D getPoint() 
	{
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
		default_random_engine generator(seed);
		normal_distribution<double> d(0.0, 1.0);
		double u = (double)rand() / RAND_MAX;
		double x = d(generator);
		double y = d(generator);
		double z = d(generator);

		double mag = sqrt(x * x + y * y + z * z);
		x /= mag; 
		y /= mag; 
		z /= mag;

		// Math.cbrt is cube root
		double c = cbrt(u);

		return Vertex3D(x * c * 500, y * c * 500, z * c * 500);
	}


	float* getColor()
	{
		return this->rgb;
	}

	void generatePoints()
	{
		Vertex3D v(this->center.x + ((rand() % 15) + 300), this->center.y, this->center.z);
		Line3D baseLine(this->center, v);

		Line3D baseCopy(baseLine);
		for (int i = 0; i < this->numLines; i++)
		{
			baseLine.rotate(((double)rand() / RAND_MAX) * (M_PI * 2), ((double)rand() / RAND_MAX) * (M_PI * 2));
			branchEnds.push_back(baseLine.p2);
			baseLine = baseCopy;
		}
	}
};

void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	int numFireworks = (rand() % 3) + 5;
	vector<Firework> fireworks;

	for (int i = 0; i < numFireworks; i++) {
		double coinFlip = (double)rand() / RAND_MAX;
		int randx = coinFlip > (double)((int)coinFlip) + 0.5 ? -1 * (rand() % 800) : rand() % 800;
		coinFlip = (double)rand() / RAND_MAX;
		int randy = coinFlip > (double)((int)coinFlip) + 0.5 ? -1 * (rand() % 800) : rand() % 800;
		coinFlip = (double)rand() / RAND_MAX;
		int randz = coinFlip > (double)((int)coinFlip) + 0.5 ? -1 * (rand() % 800) : rand() % 800;
		Vertex3D vert(randx, randy, randz);
		Firework f(vert);
		fireworks.push_back(f);
	}

	for (int i = 0; i < numFireworks; i++) {
		Firework f = fireworks[i];
		Vertex3D center = f.center;
		float* rgb = f.getColor();
		glColor3f(rgb[0], rgb[1], rgb[2]);
		glBegin(GL_LINES);
		for (int j = 0; j < f.branchEnds.size(); j++)
		{
			//Vertex3D v = f.branchEnds[j];
			glVertex3f(center.x, center.y, center.z);
			//glVertex3f(v.x, v.y, v.z);
			Vertex3D v = f.getPoint();
			glVertex3f(v.x + center.x, v.y + center.y, v.z + center.z);
		}
		glEnd();
	}

	glFlush();
}

void init() {
	srand(time(NULL));
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1000.0, 1000.0, -1000.0, 1000.0, -1000.0, 1000.0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(200, 200);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Fireworks");
	glutDisplayFunc(display);
	init();
	glutMainLoop();
}