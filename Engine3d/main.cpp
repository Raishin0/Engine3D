#define _USE_MATH_DEFINES
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
#include <ctime>

struct float3
{
	float3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}

	float x;
	float y;
	float z;
};
struct float2
{
	float2(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}

	float x;
	float y;
};

int g_iWindowWidth = 512;
int g_iWindowHeight = 512;
int g_iGLUTWindowHandle = 0;
int g_iErrorCode = 0;

void InitGL(int argc, char* argv[]);
void DisplayGL();
void IdleGL();
void KeyboardGL(unsigned char c, int x, int y);
void KeyboardUpGL(unsigned char c, int x, int y);
void MotionGL(int x, int y);
void ReshapeGL(int w, int h);
void DrawCube();

// Rotation parameters
float mouse_x = 0.0f;
float mouse_y = 0.0f;
float fDeltaTime;
float speed = 5;
bool kbuffer[127];

std::clock_t g_PreviousTicks;
std::clock_t g_CurrentTicks;
float3 pos(0, 0, 0);
float3 dir(0, 0, 0);
float2 input(0, 0);
float2 rot(0, M_PI / 2.0f);

// We're exiting, cleanup the allocated resources.
void Cleanup(int exitCode, bool bExit = true);

int main(int argc, char* argv[])
{
	// Capture the previous time to calculate the delta time on the next frame
	g_PreviousTicks = std::clock();

	InitGL(argc, argv);
	glutMainLoop();
	Cleanup(g_iErrorCode);
}

void Cleanup(int errorCode, bool bExit)
{
	if (g_iGLUTWindowHandle != 0)
	{
		glutDestroyWindow(g_iGLUTWindowHandle);
		g_iGLUTWindowHandle = 0;
	}

	if (bExit)
	{
		exit(errorCode);
	}
}

void InitGL(int argc, char* argv[])
{
	std::cout << "Initialise OpenGL..." << std::endl;

	glutInit(&argc, argv);
	int iScreenWidth = glutGet(GLUT_SCREEN_WIDTH);
	int iScreenHeight = glutGet(GLUT_SCREEN_HEIGHT);

	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowPosition((iScreenWidth - g_iWindowWidth) / 2,
		(iScreenHeight - g_iWindowHeight) / 2);
	glutInitWindowSize(g_iWindowWidth, g_iWindowHeight);

	g_iGLUTWindowHandle = glutCreateWindow("OpenGL");

	// Register GLUT callbacks
	glutDisplayFunc(DisplayGL);
	glutIdleFunc(IdleGL);
	glutMotionFunc(MotionGL);
	glutKeyboardFunc(KeyboardGL);
	glutKeyboardUpFunc(KeyboardUpGL);
	glutReshapeFunc(ReshapeGL);

	// Setup initial GL State
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);

	glShadeModel(GL_SMOOTH);

	std::cout << "Initialise OpenGL: Success!" << std::endl;
}

void DisplayGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                   // Clear the color buffer, and the depth buffer.

	glMatrixMode(GL_MODELVIEW);                                           // Switch to modelview matrix mode
	glLoadIdentity();                                                       // Load the identity matrix
	glEnable(GL_DEPTH_TEST);                                              //Apply the depth
                                                       
	gluLookAt(pos.x, 0, pos.z,
			  pos.x -dir.x, -dir.y, pos.z - dir.z,
			  0, 1, 0);

	glTranslatef(0, 0, -6.0f);                                     
	DrawCube();

	glutSwapBuffers();
	glutPostRedisplay();
}

void IdleGL()
{
	// Update our simulation
	g_CurrentTicks = std::clock();
	float deltaTicks = (g_CurrentTicks - g_PreviousTicks);
	g_PreviousTicks = g_CurrentTicks;
	fDeltaTime = deltaTicks / (float)CLOCKS_PER_SEC;
	//Keyboard
	float rSpeed = speed * fDeltaTime;
	if (kbuffer['a'])
	{
		input.x = -rSpeed;
	}
	if (kbuffer['d'])
	{
		input.x = rSpeed;
	}
	if (kbuffer['w'])
	{
		input.y = -rSpeed;
	}
	if (kbuffer['s'])
	{
		input.y = rSpeed;
	}
	if (kbuffer['r'])
	{
		std::cout << "Reset Parameters..." << std::endl;
		pos.x = 0.0f;
		pos.z = 0.0f;

		kbuffer['r'] = false;
	}
	if (kbuffer['\033'])
	{
		Cleanup(0);// Cleanup up and quit
	}
	dir = float3(sinf(rot.x), cosf(rot.y), cosf(rot.x));

	float3 newVel;
	newVel.x = dir.x*input.y + dir.z*input.x;
	newVel.z = dir.z*input.y - dir.x*input.x;
	pos.x += newVel.x;
	pos.z += newVel.z;
	input.x = 0.f;
	input.y = 0.f;

	glutPostRedisplay();
}

void KeyboardGL(unsigned char c, int x, int y)
{
	// Store the current scene so we can test if it has changed later.
	//std::cout << c << std::endl;
	/*switch (c)
	{
	case 'a':
	case 'A':
	{
		vel.x = -speed;
	}
	break;
	case 'd':
	case 'D':
	{
		vel.x = speed;
	}
	break;
	case 'w':
	case 'W':
	{
		vel.z = -speed;
	}
	break;
	case 's':
	case 'S':
	{
		vel.z = speed;
	}
	break;

	case 'r':
	case 'R':
	{
		std::cout << "Reset Parameters..." << std::endl;
		pos.x = 0.0f;
		pos.z = 0.0f;
	}
	break;
	case '\033': // escape quits
	{
		Cleanup(0);// Cleanup up and quit
	}
	break;
	}
	*/
	
	kbuffer[c] = true;
}

void KeyboardUpGL(unsigned char c, int x, int y)
{
	kbuffer[c] = false;
}

void MotionGL(int x, int y)
{
	float speed_mouse_x = x - mouse_x;
	float speed_mouse_y = y - mouse_y;
	rot.x -= fmodf(speed_mouse_x*0.01f, 0.1f);
	rot.y -= fmodf(speed_mouse_y*0.01f, 0.1f);
	rot.y = (rot.y < 0) ? 0 : fminf(rot.y, M_PI);
	mouse_x = x;
	mouse_y = y;
	std::cout << rot.x << " - " << rot.y << std::endl;

	glutPostRedisplay();
}

void ReshapeGL(int w, int h)
{
	std::cout << "ReshapGL( " << w << ", " << h << " );" << std::endl;

	if (h == 0)										// Prevent A Divide By Zero error
	{
		h = 1;										// Making Height Equal One
	}

	g_iWindowWidth = w;
	g_iWindowHeight = h;

	glViewport(0, 0, g_iWindowWidth, g_iWindowHeight);

	// Setup the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)g_iWindowWidth / (GLdouble)g_iWindowHeight, 0.1, 100.0);

	glutPostRedisplay();
}

void DrawCube() {

	glBegin(GL_QUADS);
	// Top face
	glColor3f(0.0f, 1.0f, 0.0f);                                   // Green
	glVertex3f(1.0f, 1.0f, -1.0f);                                   // Top-right of top face
	glVertex3f(-1.0f, 1.0f, -1.0f);                                   // Top-left of top face
	glVertex3f(-1.0f, 1.0f, 1.0f);                                   // Bottom-left of top face
	glVertex3f(1.0f, 1.0f, 1.0f);                                   // Bottom-right of top face

	// Bottom face
	glColor3f(1.0f, 0.5f, 0.0f);                                  // Orange
	glVertex3f(1.0f, -1.0f, -1.0f);                                  // Top-right of bottom face
	glVertex3f(-1.0f, -1.0f, -1.0f);                                  // Top-left of bottom face
	glVertex3f(-1.0f, -1.0f, 1.0f);                                  // Bottom-left of bottom face
	glVertex3f(1.0f, -1.0f, 1.0f);                                  // Bottom-right of bottom face

	// Front face
	glColor3f(1.0f, 0.0f, 0.0f);                                  // Red
	glVertex3f(1.0f, 1.0f, 1.0f);                                  // Top-Right of front face
	glVertex3f(-1.0f, 1.0f, 1.0f);                                  // Top-left of front face
	glVertex3f(-1.0f, -1.0f, 1.0f);                                  // Bottom-left of front face
	glVertex3f(1.0f, -1.0f, 1.0f);                                  // Bottom-right of front face

	// Back face
	glColor3f(1.0f, 1.0f, 0.0f);                                 // Yellow
	glVertex3f(1.0f, -1.0f, -1.0f);                                 // Bottom-Left of back face
	glVertex3f(-1.0f, -1.0f, -1.0f);                                 // Bottom-Right of back face
	glVertex3f(-1.0f, 1.0f, -1.0f);                                 // Top-Right of back face
	glVertex3f(1.0f, 1.0f, -1.0f);                                 // Top-Left of back face

	// Left face
	glColor3f(0.0f, 0.0f, 1.0f);                                   // Blue
	glVertex3f(-1.0f, 1.0f, 1.0f);                                   // Top-Right of left face
	glVertex3f(-1.0f, 1.0f, -1.0f);                                   // Top-Left of left face
	glVertex3f(-1.0f, -1.0f, -1.0f);                                   // Bottom-Left of left face
	glVertex3f(-1.0f, -1.0f, 1.0f);                                   // Bottom-Right of left face

	// Right face
	glColor3f(1.0f, 0.0f, 1.0f);                                   // Violet
	glVertex3f(1.0f, 1.0f, 1.0f);                                   // Top-Right of left face
	glVertex3f(1.0f, 1.0f, -1.0f);                                   // Top-Left of left face
	glVertex3f(1.0f, -1.0f, -1.0f);                                   // Bottom-Left of left face
	glVertex3f(1.0f, -1.0f, 1.0f);                                   // Bottom-Right of left face
	glEnd();
}