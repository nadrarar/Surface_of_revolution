#include <GL/glew.h>
#include <GL/freeglut.h>
#include "utilities.h"
#include "main_window.h"
#include "high_resolution_timer.h"
#include <map>
#include "ilcontainer.h"

/*	Surface of Revolution
	Shader Support Class
	Shader - Gouraud, Phong and Normal Mapping
	Windows High Resolution Timer

	Perry Kivolowitz - University of Wisconsin - Madison
*/

using namespace std;

// In retrospect, this is stupid. I keep toying with different methods
// of encapsulating potentially multiple reusable windows. This method
// isn't going to go forward.
map<char, Window *> window_map;
map<int, char> window_handle_map;

// The high resolution timer is not used in the example but is included
// so as to provide a reason to provide the source code.
static HighResolutionTime hrTime;
double elapsed_time;
double total_time;

/*	Notice that glGetError must be called in a loop. GL buffers up a bit mask
	of all the errors it has encountered - the bits remain set until cleared
	one by one by calls to glGetError.
*/

bool GLReturnedError(char * s)
{
	return false;

	bool return_error = false;
	GLenum glerror;

	while ((glerror = glGetError()) != GL_NO_ERROR)
	{
		return_error = true;
		cerr << s << ": " << gluErrorString(glerror) << endl;
	}

	return return_error;
}

/*	Many people fail to guard against a height of 0. This value would result
	in divide by zeros elsewhere. 
*/

void ReshapeFunc(int w, int h)
{
	if (h > 0)
	{
		// Recall, this method of handling multiple abstract windows is not one for which I am proud.
		Window ** window = &window_map[window_handle_map[glutGetWindow()]];
		(*window)->height = h;
		(*window)->width = w;
	}
}

void CloseFunc()
{
	window_map[window_handle_map[glutGetWindow()]]->window_handle = BAD_GL_VALUE;
}

void DisplayFunc()
{
	elapsed_time = hrTime.TimeSinceLastCall();
	total_time = hrTime.TotalTime();

	window_map[window_handle_map[glutGetWindow()]]->DisplayFunc();
}

void KeyboardFunc(unsigned char c, int x, int y)
{
	window_map[window_handle_map[glutGetWindow()]]->KeyboardFunc(c, x, y);
}

void SpecialFunc(int c, int x, int y)
{
	window_map[window_handle_map[glutGetWindow()]]->SpecialFunc(c, x, y);
}

int mouse_x_prev;
int mouse_y_prev;

void ActiveMouseFunc(int x, int y)
{

	int dx=x-mouse_x_prev;
	int dy=y-mouse_y_prev;
	//rudemenary smoothing of controls:  stops simultaneous change in x and y
	if(abs(dx)>=abs(dy))
		dy=0;
	else
		dx=0;

	window_map[window_handle_map[glutGetWindow()]]->x -= dx;
	window_map[window_handle_map[glutGetWindow()]]->y += dy;

	//note:x controls rotation of surface, y controls pitch.
	mouse_x_prev=x;
	mouse_y_prev=y;
}

void MouseClickFunc(int button, int state, int x, int y)
{
	if(state==GLUT_DOWN){
		mouse_x_prev=x;
		mouse_y_prev=y;
	}
}

void PassiveMouseFunc(int x, int y)
{
	window_map[window_handle_map[glutGetWindow()]]->x = x;
	window_map[window_handle_map[glutGetWindow()]]->y = y;
}

void BaseTimerFunc(int value)
{
	int window_handle = glutGetWindow();
	if (window_handle != 0)
		window_map[window_handle_map[window_handle]]->TimerFunc(value);
}

float PI_over_2 = 3.14159265f / 2.0f;
float PI = 3.14159265f;

/*	These are simple functions which drive the creation of various surfaces of revolution.
*/

float CosWave(float t)
{
	float f = .5;
	t = t * PI*4;
	float radius = cos(t) / 4.0f;
	return radius+f;
}

float HourGlass(float t)
{
	t = (2 * t - 1.0f) * PI_over_2;
	float radius = fabs(sin(t)) / 4.0f + 0.5f;
	return radius;
}

float Cone(float t)
{
	return t;
}

float InverseCone(float t)
{
	return 1.0f - t;
}

float TestCone(float t)
{
	return t / 2.0f;
}

float Cylinder(float t)
{
	return 0.5f;
}

/*	This method of creating a sphere does not sample the poles sufficiently densely. Therefore,
	a different means of sampling t should be used.
*/

float Sphere(float t)
{
	t = t - 0.5f;
	return sqrt(0.25f - t * t);
}

float Squiggle(float t)
{
	return t > 0.5f ? (t > 0.75f ? sin(t + PI) : cos(t + PI)) : sin(t - PI);
}

void DeallocateWindows()
{
	while (!window_map.empty())
	{
		map<char, Window *>::iterator iter = window_map.begin();
		delete(iter->second);
		window_map.erase(iter);
	}			
}

int main(int argc, char * argv[])
{
	glutInit(&argc, argv);
	ilInit();
	glutInitWindowSize(1024, 768);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

	MainWindow * main_window = new MainWindow(512, 512);

	window_map.insert(pair<char, Window *>('m', main_window));
	window_handle_map.insert(pair<GLuint, char>(window_map['m']->window_handle = glutCreateWindow("New Surfaces of Revolution"), 'm'));

	glutReshapeFunc(ReshapeFunc);
	//glutPassiveMotionFunc(PassiveMouseFunc);
	glutMotionFunc(ActiveMouseFunc);
	glutMouseFunc(MouseClickFunc);
	glutCloseFunc(CloseFunc);
	glutDisplayFunc(DisplayFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(SpecialFunc);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutTimerFunc(1000 / 60, BaseTimerFunc, 0);
	if (glewInit() != GLEW_OK)
	{
		cerr << "GLEW failed to initialize." << endl;
		DeallocateWindows();
		return 0;
	}
	MainWindowStruct mws;
	mws.grid_size = glm::ivec2(200, 200);
	mws.function = Sphere;
	mws.open = false;
	// Each window is its own GL context. Make sure the right context is current when PostGLInitialize() is called.
	for (map<char, Window *>::iterator iter = window_map.begin(); iter != window_map.end(); ++iter)
	{
		switch (iter->first)
		{
		case 'm':
			glutSetWindow(window_map['m']->window_handle);
			if (!window_map['m']->PostGLInitialize((void *) &mws))
			{
				DeallocateWindows();
				return 0;
			}
		}
	}

	glutMainLoop();
	DeallocateWindows();
	return 0;
}
