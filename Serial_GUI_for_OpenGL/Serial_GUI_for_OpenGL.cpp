#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>

#using <System.dll>
using namespace System;
using namespace System::IO::Ports;
using namespace System::Threading;

#define WIDTH 800
#define HEIGHT 600

GLfloat lightpos[]	 = { 0.0, 3.0, 5.0, 1.0 };
GLfloat lightcolor[] = { 1.0, 1.0, 1.0, 1.0 };

std::string buff = "";
float dt;
double AngleX, AngleY, AngleZ;

void MarshalString(String ^ s, std::string& os);
void resize(int w, int h);
void display(void);
void init(void);
void idle(void);

void MarshalString(String ^ s, std::string& os) {
	using namespace Runtime::InteropServices;
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}

static void DataReceivedHandler(Object^ sender, SerialDataReceivedEventArgs^ e){

	SerialPort^ sp = (SerialPort^)sender;
	String^ indata = sp->ReadExisting();
	std::string str;
	
	MarshalString(indata, str);

	for (int i = 0; i < str.size(); i++) {
		if (str[i] == '\n') {
			sscanf_s(buff.c_str(), "%f,%lf,%lf", &dt, &AngleX, &AngleY);
			std::cout << dt << ":" << AngleX << ":" << AngleY << ":" << AngleZ << std::endl;
			buff = "";
		} else {
			buff += str[i];
		}
	}
}

void resize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0, 2.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	glRotated(AngleX, -1.0f, 0.0f, 0.0f);
	glRotated(AngleY, 0.0f, 0.0f, -1.0f);
	glRotated(AngleZ, 0.0f, 1.0f, 0.0f);

	glColor3d(0.0, 0.0, 0.0);
	glutSolidCube(0.5);
	glutSwapBuffers();
}

void init(void) {
	glClearColor(0.0, 0.0, 0.0, 1.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightcolor);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightcolor);
}

void idle(void) {
	glutPostRedisplay();
}

int main(int argc, char *argv[]) {
	
	SerialPort^ mySerialPort = gcnew SerialPort("COM7", 115200, Parity::None, 8, StopBits::One);
	mySerialPort->DataReceived += gcnew SerialDataReceivedEventHandler(DataReceivedHandler);
	mySerialPort->Open();

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);
	init();
	glutMainLoop();
	return 0;
}
