# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>
# include <iostream>
# include <cmath>
# include <list>

# include "uglyfont.h"
# include "keyboard.h"

using namespace std;

//------VARIABLES GLOBALES---------
double 
	shipX = 400, // Posición en x
	shipY = 300, // Posición en y
    shipAngle = 0, // Orientación nave
	machineGunAngle = 0, // Orientación arma
	scale = 2.5; 
const double PI = 4*atan(1.0); // Es una forma de calcular el valor de PI en código
const double
	zBullet = 0.6,
	zBodyShip = 0.3,
	zCabin = 0.6,
	zLeftWing = 0.5,
	zRightWing = 0.5,
	zRadar = 0.9;

static int AngLineRadar = 0; // angulo de rotacion de linea del radar
int 
	w = 800,
	h = 600,
	// Para no salirse de pantalla
	xMax = 800, // Máximo pantalla en x
	xmin = 0, // Mínimo pantalla en x
	yMax = 600, // Máximo pantalla en y
	yMin = 0, // Mínimo pantalla en y
	
	mySeconds, // Segundos transcurridos desde que comenzó el programa
	myMilliseconds,
	
	myMotor = 100;

bool commandLineinfo = true; // Informa errores por línea de comando
bool zoom = false; // Zoom

Keyboard keyboard(' ', 'w', 's', 'a', 'd', 'z', 'b');

//----------------------------------------------------

class Bullet 
{
private:
	double x;
	double y;
	double dX;
	double dY;
public:
	Bullet(double posX, double posY, double distanceX, double distanceY) : x(posX), y(posY), dX(distanceX), dY(distanceY) {}
	bool Update() 
	{
		x += dX;
		y += dY;
		//Si esta fuera de la pantalla, elimino la bala
		return ( x > w || x < 0 || y > h || y < 0 );
	}
	void Draw() {
		glVertex2d(x,y);
	}
};

list<Bullet> bullet;


// Renderiza texto en pantalla usando UglyFont
void drawText(string _string, float x, float y, float textScale = 1.0, float red = 1.0, float green = 1.0, float blue = 1.0, float alpha = 1.0, float textAngle = 0.0, int textCenter = 0, float textWidth = 1.0 )
{
	glPushAttrib(GL_ALL_ATTRIB_BITS); 
	glLineWidth(textWidth); // Ancho de linea del caracter
	glColor4f(red, green, blue, alpha); // Color del caracter
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
	glTranslated(shipX, shipY, zBodyShip); // Translado el cuerpo de la nave en el modelo
	glRotated(shipAngle, 0, 0, 1); // Rotación de el cuerpo de la nave en el modelo
	drawBodyShip();
	
	glPushMatrix();
	glTranslated(-20, -20, zLeftWing); // Translado ala izquierda de la nave en el modelo
	glRotated(50, 0, 0, 1); // Rotación de ala izquierda de la nave en el modelo
	drawLeftWing();
	glPopMatrix();
	
	glPushMatrix();
	glTranslated(20, -20, zRightWing); // Translado ala derecha de la nave en el modelo
	glRotated(-50, 0, 0, 1); // Rotación de ala derecha de la nave en el modelo
	drawRightWing();
	glPopMatrix();
	
	glPushMatrix();
	glTranslated(0, 15, zCabin); // Translado de la cabina de la nave
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

void drawRadar() 
{
	glPushMatrix();
	
	//Radar
	glPushMatrix();
	glColor4f(0.3f, 0.5f, 0.3f, 1.0f);
	glTranslatef (700, 500, zRadar);
	GLUquadricObj* q = gluNewQuadric();
	gluQuadricDrawStyle(q,GLU_FILL);
	gluDisk(q,0,90,30,30);
	gluDeleteQuadric(q);
	
	glPopMatrix();
	
	//Linea
	glPushMatrix();
	glColor3f(1.0f,0.0f,0.0f);
	glTranslatef (700, 500, 0.9);
	glRotatef (AngLineRadar, 0.0, 0.0, -1.0);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(0,0); glVertex2i(85,0);
	glEnd();
	glPopMatrix();
	
	glPopMatrix();
}

void DrawBullet() 
{
	glPushMatrix();
	glTranslated(0,0,zBullet);
	
	list<Bullet>::iterator b = bullet.begin();
	glPointSize(3);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POINTS);
	while( b != bullet.end() ) 
	{
		b->Draw();
		b++;
	}
	glEnd();
	
	glPopMatrix();
}

// pregunta a OpenGL por el valor de una variable de estado
int integerv(GLenum pname)
{
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
	cout << "barra espaciadora: dispara" << endl;
	cout << "z: zoom on/off" << endl;
	cout << "b: boost" << endl;
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
	if(!zoom)
	{
	glOrtho(0, w, 0, h, 1, 3);
	}
	else
	{
	// aplica el zoom con la tecla z
	glOrtho(0-(w/scale),w+(w/scale),0-(h/scale),h+(h/scale),1,3);
	}
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	gluLookAt(0,0,2, 0,0,0, 0,1,0);// Ubica la camara
	glutPostRedisplay();
}

void display_cb() 
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	drawShip();
	drawRadar();
	drawShipBoard();
	DrawBullet();
	
	// muestra la cantidad de segundos transcurridos
    char st1[30] = "Segundos Transcurridos: ";
	char st2[10] = "";
	sprintf(st2, "%d", mySeconds); // Corrijo el uso de sprintf
	strcat(st1, st2); // Concatenar el número de segundos 
	drawText(st1, 20.0, 550.0, 15.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0, 1.0);
	
	char st3[10] = "Motor: ";
	char st4[10] = "";
	sprintf(st4, "%d", myMotor);
	strcat(st3, st4);
	
	// Determina el color basado en el valor de myMotor
	float r = 0.0, g = 0.0, b = 0.0;
	if (myMotor <= 100 && myMotor >= 51 ) 
	{
		g = 1.0; // Verde
	} else if (myMotor <= 50 && myMotor >= 10) 
	{
		r = 1.0; // Amarillo (rojo + verde)
		g = 1.0;
	} else if (myMotor <= 10) 
	{
		r = 1.0; // Rojo
	}
	
	drawText(st3,20.0, 530.0, 15.0, r, g, b, 1.0, 0.0, 0, 1.0);
	
	glutSwapBuffers();
}

void idle_cb() 
{
    double angle = shipAngle*PI/180.0; // Convierte shipAngle de grados a radianes
	double ang2;
	static unsigned int lt=0;
	int dt = glutGet(GLUT_ELAPSED_TIME) - lt;
	if(dt > 60) 
	{
		lt = glutGet(GLUT_ELAPSED_TIME);
		
		AngLineRadar=(AngLineRadar + 2) % 360; // Línea del radar gira 360°
		
		//Actualizamos las posiciones de las balas
		list<Bullet>::iterator b = bullet.begin();
		while( b != bullet.end() ) //Si esta fuera del mapa eliminamos la bala
		{
			if( b->Update() )
				b = bullet.erase(b);
			else
				b++;
		}
		
    // Teclado
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
	if(keyboard.Shoot()) // Dispara
	{
		ang2 = (shipAngle + machineGunAngle)*PI/180.0;
		bullet.push_back( Bullet( (shipX), (shipY), -50*sin(ang2), 50*cos(ang2)) );//la bala sale desde la trompa de la nave
	}
	if(keyboard.Zoom()) // Zoom
	{
		zoom =! zoom;
		if(!zoom)
		{  
			cout << "Escala natural: 1:1" << endl;
		}
		else 
		{
			// aplica el Zoom All con la tecla z
			cout << "Zoom All" << endl;
		}
		// rehace la matriz de proyección (la porcion de espacio visible)
		reshape_cb(w,h);
	}
	if(keyboard.Boost())
	{
		if(myMotor > 0)
		{
			myMotor -= 1;
		}
		shipX -= 10*sin(angle);
		shipY += 10*cos(angle);
	}
    // controlamos que no salga de la pantalla
    if(shipX < xmin) shipX = 0;
    if(shipX > xMax) shipX = 800;
    if(shipY < yMin) shipY = 0;
    if(shipY > yMax) shipY = 600;
    }
    glutPostRedisplay();

	/*ACLARACIÓN: Al verificar si myMilliseconds es un múltiplo de 1000, evito ejecutar la lógica de temporización en cada ciclo de renderizado. 
	Esto puede ayudar a reducir la carga de procesamiento, ya que solo se ejecuta una vez por segundo.*/
	myMilliseconds = glutGet(GLUT_ELAPSED_TIME); // GLUT_ELAPSED_TIME devuelve el numero de milisegundos desde que se llamo a glutInit
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
		if (myMotor == 0)
		{
			display_cb();
			cout << "Rompiste el motor!" << endl;
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
	glEnable(GL_DEPTH_TEST); // Activa Depth Test
	glDepthFunc( GL_LEQUAL ); //Función de comparación de z
	glClearColor(0.f,0.f,0.f,0.f);
}

int main (int argc, char **argv)
{
	glutInit(&argc, argv);
	initialize();
	glutMainLoop();
	
	return 0;
}
