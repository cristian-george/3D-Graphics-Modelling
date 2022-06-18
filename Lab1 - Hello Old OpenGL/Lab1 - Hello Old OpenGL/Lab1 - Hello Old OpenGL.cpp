#include <iostream>
#include <GL/glew.h> // glew apare înainte de freeglut
#include <GL/freeglut.h>
//Se încarcă freeglut.lib
#pragma comment (lib, "freeglut.lib")

//Se inițializează GLUT
void InitGLUT(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100); // pozitia initiala a ferestrei
	glutInitWindowSize(1000, 700); //dimensiunile ferestrei
}
//Se inițializează culoarea de fond a ecranului
void Initialize(void)
{
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f); // culoarea negru de fond a ecranului
}
// Se scrie funcția care randează vârfurile
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(20.0);
	glBegin(GL_POINTS);
	// primul varf
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex4f(-0.8f, 0.8f, 0.0f, 1.0f);
	// al doilea varf
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex4f(0.8f, 0.8f, 0.0f, 1.0f);
	// al treilea varf
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex4f(0.0f, 0.0f, 0.0f, 1.0f);
	// al patrulea varf
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex4f(-0.8f, -0.8f, 0.0f, 1.0f);
	// al cincelea varf
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex4f(0.8f, -0.8f, 0.0f, 1.0f);

	glEnd();
	glFlush();
}
///Se apelează în funcția Main metodele de inițializare, se specifică calback-ul de randare și se pornește bucla principală
int main(int argc, char* argv[])
{
	InitGLUT(argc, argv);
	glutCreateWindow("Puncte - OpenGL <<vechi>>"); // titlul ferestrei
	std::cout << "OpenGL version supported by this platform: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version supported by this platform:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	Initialize();
	glutDisplayFunc(RenderFunction);
	glutMainLoop();
	return 0;
}