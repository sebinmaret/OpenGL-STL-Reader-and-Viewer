//OpenGL Assignment for Computer Graphics
//Author: Sebin Abraham Maret
//        seb17jan@gmail.com
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <iostream>
#include <math.h>

using namespace std;

//[Class] definition to hold the mesh data read from the STL file.
class Mesh
{
public:
	vector<vector<vector<float>>> faces;

	void addFace(vector<vector<float>> fi)
	{
		faces.push_back(fi);
	}
};

//[Function] To check if a given string starts with another given string.
bool startsWith(const string &haystack, const string &needle)
{
	return needle.length() <= haystack.length() && equal(needle.begin(), needle.end(), haystack.begin());
}

//global variable object of the class Mesh to be accessed by all functions.
Mesh mesh1;

int xval = 0;
int yval = 0;
int cur_btn = 0;

//[Function] To read STL data from given "filename" and return Mesh object.
Mesh readSTL(string filename)
{
	Mesh mesh1 = Mesh();

	ifstream fin;

	fin.open(filename);

	int state = 0;
	vector<vector<float>> temp_face;

	while (!fin.eof())
	{
		string line = "";
		getline(fin, line);

		//strip leading and trailing spaces
		line = line.substr(line.find_first_not_of(" "), line.find_last_not_of(" "));
		switch (state)
		{
		case 0:
			if (startsWith(line, "solid"))
			{
				state = 1;
			}
			break;
		case 1:
			if (startsWith(line, "facet normal"))
			{
				//Face starts here.
				state = 2;
				temp_face = {};
			}
			else if (startsWith(line, "endsolid"))
			{
				state = 0;
			}
			break;
		case 2:
			if (startsWith(line, "outer loop"))
			{
				state = 3;
			}
			else if (startsWith(line, "endfacet"))
			{
				state = 1;
			}
			break;
		case 3:
			if (startsWith(line, "vertex"))
			{
				//Coordinates of vertex is read here.
				state = 3;

				//get line without the keyword: vertex
				string coords = line.substr(line.find_first_not_of("vertex"));
				istringstream coord_stream;
				coord_stream.str(coords);
				vector<float> coord_vec;

				while (!coord_stream.eof())
				{
					string t;
					coord_stream >> t;
					float t1 = strtof(t.c_str(), NULL);
					coord_vec.push_back(t1);
				}

				//Pushing the coordinates of a vertex to the current face vector(temp_face).
				temp_face.push_back(coord_vec);
			}
			else if (startsWith(line, "endloop"))
			{
				state = 2;
				//When endloop is reached,one face is completed.Add it to mesh1.
				mesh1.addFace(temp_face);
			}
			break;
		}
	}
	return mesh1;
}

//[Function] To render the mesh.
void renderMesh()
{

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	for (long i = 0; i < mesh1.faces.size(); i = i + 1)
	{
		vector<vector<float>> f = mesh1.faces[i];
		glBegin(GL_LINES);
		glVertex3f(f[0][0], f[0][1], f[0][2]);
		glVertex3f(f[1][0], f[1][1], f[1][2]);

		glVertex3f(f[1][0], f[1][1], f[1][2]);
		glVertex3f(f[2][0], f[2][1], f[2][2]);

		glVertex3f(f[2][0], f[2][1], f[2][2]);
		glVertex3f(f[0][0], f[0][1], f[0][2]);
		glEnd();
	}
	glFlush();
}

//[Function] To Handle Mouse Button Events
void buttonHandler(int button, int state, int x, int y)
{

	if (button == 3 && state == 1)
	{

		glScalef(1.1, 1.1, 1.1);
	}
	else if (button == 4 && state == 1)
	{
		glScalef(0.9, 0.9, 0.9);
	}
	else if (button == GLUT_LEFT_BUTTON && state == 0)
	{
		xval = x;
		yval = y;
		cur_btn = 0;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == 0)
	{
		xval = x;
		yval = y;
		cur_btn = 2;
	}
	glutPostRedisplay();
}

//[Function] To Rotate and Translate the mesh. Called on Mouse button Click + Move events.
void dragMesh(int x, int y)
{
	int diffx = x - xval;
	int diffy = y - yval;
	float trans[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, trans);
	float cur_scale = sqrt(trans[0] * trans[0] + trans[1] * trans[1] + trans[2] * trans[2]);
	if (cur_btn == GLUT_LEFT_BUTTON)
	{
		glRotatef(diffx * 0.5, trans[1] * 1, trans[5] * 1, trans[9] * 1);

		glGetFloatv(GL_MODELVIEW_MATRIX, trans);

		glRotatef(diffy * 0.5, trans[0] * 1, trans[4] * 1, trans[8] * 1);
	}
	else if (cur_btn == GLUT_RIGHT_BUTTON)
	{

		glTranslatef(trans[0] * diffx * 0.3 / cur_scale, trans[4] * diffx * 0.3 / cur_scale, trans[8] * diffx * 0.3 / cur_scale);
		glTranslatef(trans[1] * diffy * -0.3 / cur_scale, trans[5] * diffy * -0.3 / cur_scale, trans[9] * diffy * -0.3 / cur_scale);
	}
	xval = x;
	yval = y;
	glutPostRedisplay();
}

// main function
int main(int argc, char **argv)
{

	mesh1 = readSTL("input.txt");

	// initialize glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	// set window size
	glutInitWindowSize(700, 500);
	// create window with window text
	glutCreateWindow("OpenGL Assignment - B170161CS");

	glOrtho(-100, 100, -100, 100, -100, 100);
	glColor3f(0.3, 0.3, 0.3);

	glutDisplayFunc(renderMesh);
	glutMouseFunc(buttonHandler);
	glutMotionFunc(dragMesh);
	glutMainLoop();
	return 0;
}
