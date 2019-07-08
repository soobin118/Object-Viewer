#pragma once

#include <vgl.h>
#include <vec.h>
#include<fstream>
#include<iostream>
#include<mat.h>
#include<string>
using namespace std;


struct MyObjectVertex
{
	vec4 position;
	vec4 color;
	vec3 normal;
};

class MyObject
{
public:
	MyObject(void);
	~MyObject(void);

	vec3 *flatnormal;
	vec3 *phong;
	GLuint Init(int filenum);
	MyObjectVertex * p;
	int vnum;
	int fnum;
	int numVertices;

	float top, bottom, znear, zfar, left, right;

	MyObjectVertex * Vertices;
	void SetPositionAndColorAttribute(GLuint program);

	GLuint vao;
	GLuint buffer;
	bool bInitialized;

	void computingVertexNormal();

	void st(bool is_phong);
	void rotate(int axis, float time, bool is_phong);
	void redraw(bool is_phong);
	void Draw(GLuint program);
};

MyObject::MyObject(void)
{
	bInitialized = false;
	vnum = fnum = 0;
	Vertices = NULL;
	numVertices = 0;
	top = bottom = znear = zfar = left = right = 0;
}

MyObject::~MyObject(void)
{
	if (Vertices != NULL)
		delete[] Vertices;
	if (p != NULL)
		delete[] p;
	if (flatnormal != NULL)
		delete[] flatnormal;
	if (phong != NULL)
		delete[] phong;
}



GLuint MyObject::Init(int filenum)
{
	float x, y, z;
	char temp;
	int ind1, ind2, ind3;
	int cnt1 = 0;
	int cnt2 = 0;

	// The Object should be initialized only once;
	if (bInitialized == true) return vao;
	string file;
	if (filenum == 1)
		file = "buddha.obj";
	else if (filenum == 2)
		file = "bunny.obj";
	else if (filenum == 3)
		file = "cube.obj";
	else if (filenum == 4)
		file = "dragon.obj";
	ifstream openFile(file);
	string line;
	while (!openFile.eof()) {
		getline(openFile, line);
		if (line[0] == 'v')
		{
			vnum++;
		}
		else if (line[0] == 'f') {
			fnum++;
		}
		else if (line[0] == '#')
		{//ignore
		}
		else {//ignore
		}
	}openFile.close();

	numVertices = fnum * 3;

	Vertices = new MyObjectVertex[numVertices];
	flatnormal = new vec3[numVertices];
	phong = new vec3[numVertices];
	p = new MyObjectVertex[vnum];

	ifstream openFiledata(file);
	while (!openFiledata.eof()) {
		getline(openFiledata, line);
		char *str = (char*)malloc(sizeof(char)*(line.length() + 1));
		strcpy(str, line.c_str());
		if (line[0] == 'v')
		{
			sscanf(str, "%c %f %f %f", &temp, &x, &y, &z);
			left = (left > x ? x : left);
			right = (right < x ? x : right);
			top = (top < y ? y : top);
			bottom = (bottom > y ? y : bottom);
			znear = (znear < z ? z : znear);
			zfar = (zfar > z ? z : zfar);

			p[cnt1].position = vec4(x, y, z, 1);
			p[cnt1].color = vec4(0.7, 0.7, 0.7, 1);

			cnt1++;
		}
		else if (line[0] == 'f')
		{
			sscanf(str, "%c %d %d %d", &temp, &ind1, &ind2, &ind3);
			ind1 -= 1;
			ind2 -= 1;
			ind3 -= 1;

			vec3 a = vec3(Vertices[ind1].position.x, Vertices[ind1].position.y, Vertices[ind1].position.z);
			vec3 b = vec3(Vertices[ind2].position.x, Vertices[ind2].position.y, Vertices[ind2].position.z);
			vec3 c = vec3(Vertices[ind3].position.x, Vertices[ind3].position.y, Vertices[ind3].position.z);
			vec3 np = b - a;
			vec3 nq = c - a;
			vec3 n = normalize(cross(np, nq));

			Vertices[cnt2].position = p[ind1].position;
			Vertices[cnt2].color = p[ind1].color;
			flatnormal[cnt2] = n;
			Vertices[cnt2].normal = n;	cnt2++;

			Vertices[cnt2].position = p[ind2].position;
			Vertices[cnt2].color = p[ind2].color;
			flatnormal[cnt2] = n;
			Vertices[cnt2].normal = n;	cnt2++;

			Vertices[cnt2].position = p[ind3].position;
			Vertices[cnt2].color = p[ind3].color;
			flatnormal[cnt2] = n;
			Vertices[cnt2].normal = n;	cnt2++;
		}
		else {
			//ignore
		}
	}openFiledata.close();

	computingVertexNormal();

	for (int i = 0; i < vnum; i++)
	{
		for (int j = 0; j < numVertices; j++)
		{
			if (Vertices[j].position == p[i].position)
			{
				Vertices[j].normal = p[i].normal;
				phong[j] = p[i].normal;
			}
		}
	}


	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyObjectVertex)*numVertices, Vertices, GL_STATIC_DRAW);

	bInitialized = true;
	return vao;
}

void MyObject::computingVertexNormal()
{
	vec4 a, b, c;
	vec3 an = vec3(0.0, 0.0, 0.0);
	int cnt = 0;
	for (int i = 0; i < vnum; i++)
	{
		a = p[i].position;
		an = vec3(0.0, 0.0, 0.0);
		cnt = 0;
		for (int j = 0; j < numVertices; j++)
		{
			if (Vertices[j].position == a)
			{
				an += Vertices[j].normal;
				cnt++;
			}
		}
		p[i].normal = an / (float)cnt;
	}
}

void MyObject::redraw(bool is_phong)
{
	if (is_phong == false) {
		vec3 a, b, c, np, nq, n;
		for (int i = 0; i < numVertices; i++)
		{
			Vertices[i].normal = flatnormal[i];
		}
	}
	else
	{
		for (int i = 0; i < numVertices; i++)
		{
			Vertices[i].normal = phong[i];
		}

	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyObjectVertex)*numVertices, Vertices, GL_STATIC_DRAW);
}

void MyObject::st(bool is_phong)
{
	vec3 objsize = vec3(right - left, top - bottom, znear - zfar);
	vec3 objposition = vec3((right + left) / 2.0, (top + bottom) / 2.0, (znear + zfar) / 2.0);

	float rate;
	if (objsize.x >= objsize.y && objsize.x >= objsize.z)
	{
		rate = 0.9 / objsize.x;
	}
	else if (objsize.y >= objsize.x && objsize.y >= objsize.z)
	{
		rate = 0.9 / objsize.y;
	}
	else {
		rate = 0.9 / objsize.z;
	}
	for (int j = 0; j < vnum; j++)
	{
		p[j].position = Scale(rate, rate, rate)*Translate(-objposition.x, -objposition.y, -objposition.z)*p[j].position;
	}
	for (int i = 0; i < numVertices; i++) {
		Vertices[i].position = Scale(rate, rate, rate)*Translate(-objposition.x, -objposition.y, -objposition.z)*Vertices[i].position;
		vec3 a = vec3(Vertices[i].position.x, Vertices[i].position.y, Vertices[i].position.z);
		vec3 b = vec3(Vertices[i+1].position.x, Vertices[i+1].position.y, Vertices[i+1].position.z);
		vec3 c = vec3(Vertices[i+2].position.x, Vertices[i+2].position.y, Vertices[i+2].position.z);
		vec3 np = b - a;
		vec3 nq = c - a;
		vec3 n = normalize(cross(np, nq));
		Vertices[i].normal = flatnormal[i] = n;
		Vertices[i + 1].normal = flatnormal[i+1] = n;
		Vertices[i + 2].normal = flatnormal[i+2] = n;
	}
	computingVertexNormal();
	for (int i = 0; i < vnum; i++)
	{
		for (int j = 0; j < numVertices; j++)
		{
			if (Vertices[j].position == p[i].position)
			{
				Vertices[j].normal = p[i].normal;
				phong[j] = p[i].normal;
			}
		}
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyObjectVertex)*numVertices, Vertices, GL_STATIC_DRAW);

}

void MyObject::rotate(int axis, float time, bool is_phong)
{
	/*rotationaxis=1 -> x
	2 -> y
	3 -> z	*/
	vec3 a, b, c;
	if (axis == 1)
	{
		for (int i = 0; i < vnum; i++)
		{
			p[i].position = RotateX(time)*p[i].position;
		}
		for (int i = 0; i < numVertices; i++)
		{
			Vertices[i].position = RotateX(time)*Vertices[i].position;
			Vertices[i + 1].position = RotateX(time)*Vertices[i + 1].position;
			Vertices[i + 2].position = RotateX(time)*Vertices[i + 2].position;
			a = vec3(Vertices[i].position.x, Vertices[i].position.y, Vertices[i].position.z);
			b = vec3(Vertices[i + 1].position.x, Vertices[i + 1].position.y, Vertices[i + 1].position.z);
			c = vec3(Vertices[i + 2].position.x, Vertices[i + 2].position.y, Vertices[i + 2].position.z);
			vec3 np = b - a;
			vec3 nq = c - a;
			vec3 n = normalize(cross(np, nq));
			Vertices[i].normal = flatnormal[i] = n;
			Vertices[i + 1].normal = flatnormal[i + 1] = n;
			Vertices[i + 2].normal = flatnormal[i + 2] = n;
			if (is_phong == true) {
				computingVertexNormal();
				for (int i = 0; i < vnum; i++)
				{
					for (int j = 0; j < numVertices; j++)
					{
						if (Vertices[j].position == p[i].position)
						{
							Vertices[j].normal = p[i].normal;
							phong[j] = p[i].normal;
						}
					}
				}
			}
			
			i = i + 2;
		}
	}
	else if (axis == 2)
	{
		for (int i = 0; i < vnum; i++)
		{
			p[i].position = RotateY(time)*p[i].position;
		}
		for (int i = 0; i < numVertices; i++)
		{
			Vertices[i].position = RotateY(time)*Vertices[i].position;
			Vertices[i + 1].position = RotateY(time)*Vertices[i + 1].position;
			Vertices[i + 2].position = RotateY(time)*Vertices[i + 2].position;
			a = vec3(Vertices[i].position.x, Vertices[i].position.y, Vertices[i].position.z);
			b = vec3(Vertices[i + 1].position.x, Vertices[i + 1].position.y, Vertices[i + 1].position.z);
			c = vec3(Vertices[i + 2].position.x, Vertices[i + 2].position.y, Vertices[i + 2].position.z);
			vec3 np = b - a;
			vec3 nq = c - a;
			vec3 n = normalize(cross(np, nq));
			Vertices[i].normal = flatnormal[i] = n;
			Vertices[i + 1].normal = flatnormal[i + 1] = n;
			Vertices[i + 2].normal = flatnormal[i + 2] = n;
			if (is_phong == true) {
				computingVertexNormal();
				for (int i = 0; i < vnum; i++)
				{
					for (int j = 0; j < numVertices; j++)
					{
						if (Vertices[j].position == p[i].position)
						{
							Vertices[j].normal = p[i].normal;
							phong[j] = p[i].normal;
						}
					}
				}
			}

			i = i + 2;
		}
	}
	else
	{
		for (int i = 0; i < vnum; i++)
		{
			p[i].position = RotateZ(time)*p[i].position;
		}
		for (int i = 0; i < numVertices; i++)
		{
			Vertices[i].position = RotateZ(time)*Vertices[i].position;
			Vertices[i + 1].position = RotateZ(time)*Vertices[i + 1].position;
			Vertices[i + 2].position = RotateZ(time)*Vertices[i + 2].position;
			a = vec3(Vertices[i].position.x, Vertices[i].position.y, Vertices[i].position.z);
			b = vec3(Vertices[i + 1].position.x, Vertices[i + 1].position.y, Vertices[i + 1].position.z);
			c = vec3(Vertices[i + 2].position.x, Vertices[i + 2].position.y, Vertices[i + 2].position.z);
			vec3 np = b - a;
			vec3 nq = c - a;
			vec3 n = normalize(cross(np, nq));
			Vertices[i].normal = flatnormal[i] = n;
			Vertices[i + 1].normal = flatnormal[i + 1] = n;
			Vertices[i + 2].normal = flatnormal[i + 2] = n;
			if (is_phong == true) {
				computingVertexNormal();
				for (int i = 0; i < vnum; i++)
				{
					for (int j = 0; j < numVertices; j++)
					{
						if (Vertices[j].position == p[i].position)
						{
							Vertices[j].normal = p[i].normal;
							phong[j] = p[i].normal;
						}
					}
				}
			}

			i = i + 2;
		}
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyObjectVertex)*numVertices, Vertices, GL_STATIC_DRAW);
}

void MyObject::SetPositionAndColorAttribute(GLuint program)
{
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(MyObjectVertex), BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(MyObjectVertex), BUFFER_OFFSET(sizeof(vec4)));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(MyObjectVertex), BUFFER_OFFSET(sizeof(vec4) + sizeof(vec4)));
}


void MyObject::Draw(GLuint program)
{
	if (!bInitialized) return;			// check whether it is initiazed or not. 

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	SetPositionAndColorAttribute(program);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);
}