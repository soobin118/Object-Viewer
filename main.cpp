#include <vgl.h>
#include <InitShader.h>
#include "MyCube.h"
#include "MyObject.h"
#include<string.h>
#include <vec.h>
#include <mat.h>
#include<string>
using namespace std;

MyCube cube;
MyObject object;

GLuint program;
GLuint prog_phong;

GLuint uMat;
GLuint uColor;
mat4 g_Mat = mat4(1.0f);

int winWidth = 500;
int winHeight = 500;

bool is_phong = true;
bool is_rotation = 0;
int rotationaxis = 1;
float time = 0;
int filenum = 0;

float mSpec = 0.3;
float Shininess = 50;


mat4 myLookAt(vec3 eye, vec3 at, vec3 up)
{
	// Implement your own look-at function
	mat4 V(1.0f);
	vec3 n = at - eye;
	n /= length(n);

	float a = dot(up, n);
	vec3 v = up - a*n;
	v /= length(v);

	vec3 w = cross(n, v);

	mat4 Rw(1.0f);

	Rw[0][0] = w.x;	Rw[0][1] = v.x; Rw[0][2] = -n.x;
	Rw[1][0] = w.y;	Rw[1][1] = v.y; Rw[1][2] = -n.y;
	Rw[2][0] = w.z;	Rw[2][1] = v.z; Rw[2][2] = -n.z;

	mat4 Rc(1.0f);
	for (int i = 0; i<4; i++)
		for (int j = 0; j<4; j++)
			Rc[i][j] = Rw[j][i];

	mat4 Tc = Translate(-eye.x, -eye.y, -eye.z);

	V = Rc*Tc;

	return V;
}
mat4 myPerspective(float fovy, float aspectRatio, float zNear, float zFar)
{
	mat4 P(1.0f);

	float rad = fovy * 3.141592 / 180.0;

	float sz = 1 / zFar;
	float h = zFar * tan(rad / 2);

	float sy = 1 / h;
	float w = h*aspectRatio;
	float sx = 1 / w;

	mat4 S = Scale(sx, sy, sz);
	mat4 M(1.0f);

	float c = -zNear / zFar;
	M[2][2] = 1 / (c + 1);
	M[2][3] = -c / (c + 1);
	M[3][2] = -1;
	M[3][3] = 0;

	P = M*S;

	return P;
}


void myInit()
{
	cube.Init();
	object.Init(filenum);
	object.st(is_phong);

	program = InitShader("vshader.glsl", "fshader.glsl");
	prog_phong = InitShader("vphong.glsl", "fphong.glsl");
}

void DrawAxis()
{
	glUseProgram(program);
	uMat = glGetUniformLocation(program, "uMat");
	uColor = glGetUniformLocation(program, "uColor");

	mat4 x_a = Translate(0.5, 0, 0)*Scale(1, 0.01, 0.01);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat*x_a);
	glUniform4f(uColor, 1, 0, 0, 1);
	cube.Draw(program);

	mat4 y_a = Translate(0, 0.5, 0)*Scale(0.01, 1, 0.01);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat*y_a);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);

	mat4 z_a = Translate(0, 0, 0.5)*Scale(0.01, 0.01, 1);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat*z_a);
	glUniform4f(uColor, 0, 0, 1, 1);
	cube.Draw(program);
}

void display()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	mat4 ViewMat = myLookAt(vec3(0, 0, 1.5), vec3(0, 0, 0), vec3(0, 1, 0));

	float aspect = winWidth / (float)winHeight;
	float h = 1;

	mat4 ProjMat = myPerspective(60, aspect, 0.1, 100);

	g_Mat = ProjMat*ViewMat;
	DrawAxis();

	glUseProgram(prog_phong);
	GLuint uProjMat = glGetUniformLocation(prog_phong, "uProjMat");
	GLuint uModelMat = glGetUniformLocation(prog_phong, "uModelMat");
	GLuint uTime = glGetUniformLocation(prog_phong, "uTime");
	GLuint uSpc = glGetUniformLocation(prog_phong, "uSpc");
	GLuint uShininess = glGetUniformLocation(prog_phong, "uShininess");

	vec4 spc = vec4(mSpec, mSpec, mSpec, 1);

	glUniformMatrix4fv(uProjMat, 1, GL_TRUE, ProjMat);
	glUniformMatrix4fv(uModelMat, 1, GL_TRUE, ViewMat);
	glUniform4f(uSpc, spc[0], spc[1], spc[2], spc[3]);
	glUniform1f(uShininess, Shininess);
	glUniform1f(uTime, time);

	object.Draw(prog_phong);
	if (is_rotation == true) {
		object.rotate(rotationaxis, (60 / 180.0)*3.141592, is_phong);
	}
	glutSwapBuffers();
}


void idle()
{
	if (is_rotation == true) {
		time += 1;
		Sleep(33);					// for vSync
		glutPostRedisplay();
	}
}

void reshape(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glutPostRedisplay();
}
void myKeyboard(unsigned char c, int x, int y)
{
	switch (c)
	{
	case'1':
		printf("Using Vertex Normal!\n");
		is_phong = true;
		object.redraw(is_phong);
		glutPostRedisplay();
		break;
	case'2':
		printf("Using Surface Normal!\n");
		is_phong = false;
		object.redraw(is_phong);
		glutPostRedisplay();
		break;
	case'3':
		printf("Increasing Specular Effect!\n");
		if (mSpec >= 1.0) {
		}
		else
			mSpec += 0.1;

		glutPostRedisplay();
		break;
	case'4':
		printf("Decreasing Specular Effect!\n");
		if (mSpec < 0.1) {
		}
		else
			mSpec -= 0.1;
		glutPostRedisplay();
		break;
	case'5':
		printf("Increasing Shininess!\n");
		if (Shininess >= 100) {}
		else
			Shininess += 10;
		glutPostRedisplay();
		break;
	case'6':
		printf("Decreasing Shininess!\n");
		if (Shininess <= 10) {}
		else
			Shininess -= 10;
		glutPostRedisplay();
		break;
	case' ':
		is_rotation = !is_rotation;
		if (is_rotation == true)
			printf("Play!\n");
		else
			printf("Stop!\n");
		glutPostRedisplay();
		break;
	default:
		break;
	}
}
void myMouse(int button, int state, int x, int y)
{
	/*rotationaxis=1 -> x
	2 -> y
	3 -> z	*/
	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			rotationaxis = 1;
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			rotationaxis = 3;
		}
		else {//middle button
			rotationaxis = 2;
		}
	}
}
int main(int argc, char ** argv)
{
	glutInit(&argc, argv);

	printf("SIMPLE OBJ MODEL VIEWER\n");
	printf("Programming Assignment #3 for Computer Graphics 2\n");
	printf("Department of Digital Contents, Sejong University\n");
	printf("\n----------------------------------------------------------------\n");
	printf("Spacebar: starting/stoping rotation\n");
	printf("\nLeft Mouse Button: rotating around x-axis\n");
	printf("Middle Mouse Button : rotating around y-axis\n");
	printf("Right Mouse Button: ratating around z-axis\n");
	printf("\n'1' key: Using Vertex Normal for shading\n");
	printf("'2' key: Using Surface Normal for shading\n");
	printf("'3' key: Increasing Specular effect (ks)\n");
	printf("'4' key: Decreasing Specular effect (ks)\n");
	printf("'5' key: Increasing Shininess (n)\n");
	printf("'6' key: Decreasing Shininess (n)\n");
	printf("\n'Q' key: Exit the program.\n");
	printf("----------------------------------------------------------------\n");
	char fileName[20];
	while (1)
	{
		printf("\nInput File Path:");

		scanf("%s", fileName);

		if (strcmp(fileName, "buddha.obj") == 0) {
			filenum = 1;
			break;
		}
		else if (strcmp(fileName, "bunny.obj") == 0) {
			filenum = 2;
			break;
		}
		else if (strcmp(fileName, "cube.obj") == 0) {
			filenum = 3;
			break;
		}
		else if (strcmp(fileName, "dragon.obj") == 0) {
			filenum = 4;
			break;
		}
		else
			printf("That file doesn't exist! Please retry!\n");
	}
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(winWidth, winHeight);

	glutCreateWindow("OpenGL");

	glewExperimental = true;
	glewInit();

	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouse);
	glutMainLoop();

	return 0;
}
