// OpenGL.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "OpenGL.h"

const GLfloat lightPosition[] = { 10.0,10.0,10.0,0.0 };
const GLfloat whiteLight[] = { 0.8,0.8,0.8,1.0 };
GLfloat matSpecular[] = { 0.3,0.3,0.3,1.0 };
GLfloat matShininess[] = { 20.0 };
GLfloat matEmission[] = { 0.3,0.3,0.3,1.0 };
GLfloat spin = 0;
void init()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glClearDepth(1.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, whiteLight);
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();;
	glOrtho(-10.0, 10.0, -10.0, 10.0, -10.0, 10.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glRotatef(spin, 0.0, 1.0, 0.0);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
	glutSolidSphere(3.0, 16, 16);
	glPopMatrix();
	glFlush();
}
void reshape(int w, int h)
{
	glViewport(0.0, 0.0, (GLsizei)w, (GLsizei)h);
}
void keyboardFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		spin += 30;
		break;
	case 'd':
		spin -= 30;
		break;
	}
	if (spin < 360)
		spin += 360;
	else if (spin >= 360)
		spin -= 360;
	glutPostRedisplay();
}
void mouseFunc(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
			matEmission[0] += 1.0;
			if (matEmission[0] > 1.0)
				matEmission[0] -= 1.1;
			break;
		case GLUT_MIDDLE_BUTTON:
			matEmission[1] += 1.0;
			if (matEmission[1] > 1.0)
				matEmission[1] -= 1.1;
			break;
		case GLUT_RIGHT_BUTTON:
			matEmission[2] += 1.0;
			if (matEmission[2] > 1.0)
				matEmission[2] -= 1.1;
			break;
		}
		glutPostRedisplay();
	}
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(450, 450);
	glutInitWindowPosition(150, 150);
	glutCreateWindow("rotate sphere");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
	init();
	glutMainLoop();
	return EXIT_SUCCESS;
}