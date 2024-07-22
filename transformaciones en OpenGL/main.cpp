# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>
# include <iostream>
# include <cmath>

# include "uglyfont.h"
# include "keyboard.h"

using namespace std;

//------VARIABLES GLOBALES---------
const double PI = 4*atan(1.0); // Es una forma de calcular el valor de PI en código
bool commandLineinfo = true; // Informa errores por línea de comando
double 
	shipX = 400, // Posición en x
	shipY = 300, // Posición en y
    shipAngle = 0; // Orientación
int 
	// Para no salirse de pantalla
	xMax = 800, // Máximo pantalla en x
	xmin = 0, // Mínimo pantalla en x
	yMax = 600, // Máximo pantalla en y
	yMin = 0, // Mínimo pantalla en y
	
	mySeconds, // Segundos transcurridos desde que comenzó el programa
	myMilliseconds;

Keyboard keyboard('p', 'w', 's', 'a', 'd', 'q', 'r');

//----------------------------------------------------

// Renderiza texto en pantalla usando UglyFont
void drawText(string _string, float x, float y, float textScale = 1.0, float red = 1.0, float green = 1.0, float blue = 1.0, float transparent = 1.0, float textAngle = 0.0, int textCenter = 0, float textWidth = 1.0 )
{
	glPushAttrib(GL_ALL_ATTRIB_BITS); 
	glLineWidth(textWidth); // Ancho de linea del caracter
	glColor4f(red, green, blue, transparent); // Color del caracter
	glPushMatrix();
	glTranslated(x, y, 0); // Posicion del caracter
	glRotatef(textAngle, 0, 0, 1); // Angulo del caracter
	glScalef(textScale, textScale, textScale);
	YsDrawUglyFont(_string.c_str(), textCenter); // El texto puede estar centrado o no
	glPopMatrix();
	glPopAttrib();
}

void drawShipCabin()
{
	glColor3f(0.678f, 0.847f, 0.902f);
	double cabinRadius = 10; // Radio de la cabina de la nave
	int levelDetail = 30; // nivel de detalle (smooth)
	GLUquadricObj* q = gluNewQuadric(); // Crea un objeto de cuadrica (superficie curva) que se utilizará para dibujar la cabina
	gluQuadricDrawStyle(q, GLU_FILL); // Estilo de bibujo "relleno"
	gluDisk(q, 0, cabinRadius, levelDetail, levelDetail); // Dibuja un disco (la cabina de la nave) con el radio especificado, tercer y cuarto parámetro son segmentos(slices) y anillos(loops) = levelDetail
	gluDeleteQuadric(q); // Elimina el objeto de cuadrica.
}

void drawLeftWing() // Ala izquierda
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	glVertex2d(0, -10);
	glVertex2d(-20, 10);
	glVertex2d(20, 10);
	glEnd();
}

void drawRightWing() // Ala derecha
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	glVertex2d(0, -10);
	glVertex2d(-20, 10);
	glVertex2d(20, 10);
	glEnd();
}

void drawBodyShip() // Cuerpo de la nave
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glVertex2d(15,45);
	glVertex2d(-15,45);
	glVertex2d(-15,-45);
	glVertex2d(15,-45);
	glEnd();
	
	glPushMatrix(); // Iniciamos push para redondear la trompa de la nave
	glTranslated(0, 45, 0);  
	double radius = 15; double startAngle = 270; double nextAngle = 180; 
	int levelDetail = 30;
	GLUquadricObj* q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_FILL);
	gluPartialDisk(q, 0, radius, levelDetail, levelDetail, startAngle, nextAngle); // Función de GLU que permite dibujar una porción de un disco.
	gluDeleteQuadric(q);
	glPopMatrix();
}

void drawShip() // Modelo completo de la nave
{
	glPushMatrix(); // Inicio push inicial
	glTranslated(shipX, shipY, 0); // Translado el cuerpo de la nave en el modelo
	glRotated(shipAngle, 0, 0, 1); // Rotación de el cuerpo de la nave en el modelo
	drawBodyShip();
	
	glPushMatrix();
	glTranslated(-20, -20, 0); // Translado ala izquierda de la nave en el modelo
	glRotated(50, 0, 0, 1); // Rotación de ala izquierda de la nave en el modelo
	drawLeftWing();
	glPopMatrix();
	
	glPushMatrix();
	glTranslated(20, -20, 0); // Translado ala derecha de la nave en el modelo
	glRotated(-50, 0, 0, 1); // Rotación de ala derecha de la nave en el modelo
	drawRightWing();
	glPopMatrix();
	
	glPushMatrix();
	glTranslated(0, 15, 0); // Translado de la cabina de la nave
	drawShipCabin();
	glPopMatrix();
	
	glPopMatrix(); // Fin push inicial
}

void drawShipBoard() // Portador de la nave
{
	glColor3f(0.5f, 0.5f, 0.5f);
	glLineWidth(5.0);
	glBegin(GL_LINES);
	glVertex2i(350, 350); glVertex2i(350, 250);
	glVertex2i(450, 350); glVertex2i(450, 250);
	glVertex2i(350, 250); glVertex2i(450, 250);
	glEnd();
}

// pregunta a OpenGL por el valor de una variable de estado
int integerv(GLenum pname){
	int value;
	glGetIntegerv(pname,&value);
	return value;
}
# define _PRINT_INT_VALUE(pname) #pname << ": " << integerv(pname) <<endl
	
void info()
{
	if(commandLineinfo){
	cout << "Vendor:         " << glGetString(GL_VENDOR) << endl
	<< "Renderer:       " << glGetString(GL_RENDERER) << endl
	<< "GL_Version:     " << glGetString(GL_VERSION) << endl
	<< "GL_Extensions:  " << glGetString(GL_EXTENSIONS) << endl
	<< "GLU_Version:    " << gluGetString(GLU_VERSION) << endl
	<< "GLU_Extensions: " << gluGetString(GLU_EXTENSIONS) << endl
	<< _PRINT_INT_VALUE(GL_DOUBLEBUFFER)
	<< _PRINT_INT_VALUE(GL_STEREO)
	<< _PRINT_INT_VALUE(GL_AUX_BUFFERS)
	<< _PRINT_INT_VALUE(GL_RED_BITS)
	<< _PRINT_INT_VALUE(GL_GREEN_BITS)
	<< _PRINT_INT_VALUE(GL_BLUE_BITS)
	<< _PRINT_INT_VALUE(GL_ALPHA_BITS)
	<< _PRINT_INT_VALUE(GL_DEPTH_BITS)
	<< _PRINT_INT_VALUE(GL_STENCIL_BITS)
	<< _PRINT_INT_VALUE(GL_ACCUM_RED_BITS)
	<< _PRINT_INT_VALUE(GL_ACCUM_GREEN_BITS)
	<< _PRINT_INT_VALUE(GL_ACCUM_BLUE_BITS)
	<< _PRINT_INT_VALUE(GL_ACCUM_ALPHA_BITS)<< endl;
	}
	
	cout << "Teclas a utilizar:" << endl;
	cout << "w: avanza" << endl;
	cout << "s: retrocede" << endl;
	cout << "d: gira en sentido horario" << endl;
	cout << "a: gira en sentido antihorario" << endl;
}

void checkErrors()
{
	int error = glGetError();
	while(error != GL_NO_ERROR)
	{
		if (commandLineinfo)
		{
			if(error == GL_INVALID_ENUM) // Cuando se pasa un valor de enumeración no válido para OpenGL
				cout << "GL_INVALID_ENUM" << endl; 
			else if(error == GL_INVALID_VALUE) // Cuando se pasa un valor numérico no válido para OpenGL
				cout << "GL_INVALID_VALUE" << endl;
			else if (error == GL_INVALID_OPERATION) // Cuando se intenta realizar una operación no válida en el estado actual de OpenGL
				cout << "GL_INVALID_OPERATION" << endl;
			else if (error == GL_STACK_OVERFLOW) // Cuando se intenta realizar una operación que haría que se desborde la pila de matrices
				cout << "GL_STACK_OVERFLOW" << endl;
			else if (error == GL_STACK_UNDERFLOW) // Cuando se intenta realizar una operación que haría que se subdesbordara la pila de matrices
				cout << "GL_STACK_UNDERFLOW" << endl;
			else if (error == GL_OUT_OF_MEMORY) // Cuando OpenGL no tiene suficiente memoria disponible para realizar una operación
				cout << "GL_OUT_OF_MEMORY" << endl;
		}
		error = glGetError();
	}
}

//--------CALLBACKS------------

void reshape_cb (int w, int h) 
{
	if (w==0||h==0) return;
	glViewport(0,0,w,h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho(0, w, 0, h, -1, 1);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

void display_cb() 
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	drawShip();
	drawShipBoard();
	
	// muestra la cantidad de segundos transcurridos
    char st1[30] = "Segundos Transcurridos: ";
	char st2[10] = "";
	sprintf(st2, "%d", mySeconds); // Corrijo el uso de sprintf
	strcat(st1, st2); // Concatenar el número de segundos 
	drawText(st1, 20, 550, 30.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0, 1.0);
	
	glutSwapBuffers();
}

void idle_cb() 
{
    double angle = shipAngle*PI/180.0; // Convierte shipAngle de grados a radianes
    static unsigned int lapsedTime = 0; // Tiempo transcurrido
    if(glutGet(GLUT_ELAPSED_TIME) - lapsedTime > 60) 
    {
    // Primero miramos el estado del teclado y realizamos los cambios en la direccion y la velocidad de la nav
    if(keyboard.Fordward()) // Adelante
    {
    shipX -= 5*sin(angle);
    shipY += 5*cos(angle);
    }
    if(keyboard.Back()) // Atrás
    {
    shipX += 5*sin(angle);
    shipY -= 5*cos(angle);
    }
    if(keyboard.Left()) // Izquierda
    {
    shipAngle += 0.8;
    }
    if(keyboard.Right()) // Derecha
    {
    shipAngle -= 0.8;
    }

    // controlamos que no salga de la pantalla
    if(shipX < xmin) shipX = 0;
    if(shipX > xMax) shipX = 800;
    if(shipY < yMin) shipY = 0;
    if(shipY > yMax) shipY = 600;

    lapsedTime = glutGet(GLUT_ELAPSED_TIME);
    }
    glutPostRedisplay();

	int dt = glutGet(GLUT_ELAPSED_TIME) - lapsedTime; // Iniciamos deltaTime
	myMilliseconds = glutGet(GLUT_ELAPSED_TIME); // GLUT_ELAPSED_TIME devuelve el numero de milisegundos desde que se llamo a glutInit
	if(dt > 60) // Comprueba si han pasado más de 60 milisegundos desde la última actualización
	{
		lapsedTime = glutGet(GLUT_ELAPSED_TIME); // Actualiza lapsedTime al tiempo actual para la próxima comparación
		display_cb();
	}
	
	/*ACLARACIÓN: Al verificar si myMilliseconds es un múltiplo de 1000, evito ejecutar la lógica de temporización en cada ciclo de renderizado. 
	Esto puede ayudar a reducir la carga de procesamiento, ya que solo se ejecuta una vez por segundo.*/
	if (myMilliseconds%1000 == 0) // Verifica si el tiempo total es un múltiplo de 1000 (es decir, cada segundo)
	{
		mySeconds = myMilliseconds / 1000; // Calcula el número de segundos transcurridos dividiendo los milisegundos entre 1000
		if (mySeconds >= 30) // Si pasaron 30 segundos finaliza el programa
		{
			display_cb();
			cout << "Termino el tiempo!" << endl;
			cout << "Pasaron " << mySeconds << " segundos!" <<endl;
			exit(EXIT_SUCCESS);
		}
	}
}

void initialize()
{
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH); // Color RGB, doble buffer y zBuffer
	glutInitWindowSize(800,600);
	glutInitWindowPosition (500,50);
	glutCreateWindow("TRANSFORMACIONES EN OPENGL");
	info();
	checkErrors();
	glutDisplayFunc(display_cb);
	glutReshapeFunc(reshape_cb);
	glutIdleFunc(idle_cb);
	keyboard.InitKeyboard();
	glClearColor(0.f,0.f,0.f,0.f);
}

int main (int argc, char **argv)
{
	glutInit(&argc, argv);
	initialize();
	glutMainLoop();
	
	return 0;
}
