# include "Keyboard.h"

bool Keyboard::keyboard[256];
bool Keyboard::exit;

Keyboard::Keyboard(unsigned char shoot, unsigned char forward, unsigned char back, unsigned char left, unsigned char right, unsigned char zoom, unsigned char boost)
{
	for(int i = 0; i < 256; i++) // Marco todas las teclas como no presionadas
	{
		Keyboard::keyboard[i] = false;
		
		// Guardo el mapeo de teclas
		map[0] = shoot;
		map[1] = forward;
		map[2] = back;
		map[3] = left;
		map[4] = right;
		map[5] = zoom;
		map[6] = boost;	
	}
}

// Esta funcion registra los callbacks para que el objeto pueda escuchar cuando una tecla se presiona o se libera
void Keyboard::InitKeyboard()
{
	glutIgnoreKeyRepeat(true);
	glutKeyboardFunc(Keyboard::KeyPressed_cb);
	glutKeyboardUpFunc(Keyboard::KeyRelease_cb);
	glutSpecialFunc(Keyboard::Special_cb);
}

void Keyboard::KeyPressed_cb(unsigned char key, int x, int y)
{
	Keyboard::keyboard[key] = true; // Marca la tecla 'key' como presionada, asignando al arreglo 'keyboard' en la posicion 'key' el valor true
	if(key == 27)
	{
		exit = true;
	}
}

void Keyboard::KeyRelease_cb(unsigned char key, int x, int y)
{
	Keyboard::keyboard[key] = false; // Marca el arreglo 'keyboard' en la posicion 'key' como false, para indicar que la tecla NO esta presionada
}

void Keyboard::Special_cb(int key, int xmodif, int ymodif)
{
	if(key == GLUT_KEY_F4 && glutGetModifiers() == GLUT_ACTIVE_ALT)
	{
		exit = true; // ALT + F4 sale
	}
}

//Las funciones que siguen simplemente se fijan si la tecla guardada en map esta presionada
bool Keyboard::Shoot() 
{
	return keyboard[map[0]];
}
bool Keyboard::Fordward()
{
	return keyboard[map[1]];
}
bool Keyboard::Back()
{
	return keyboard[map[2]];
}
bool Keyboard::Left()
{
	return keyboard[map[3]];
}
bool Keyboard::Right() 
{
	return keyboard[map[4]];
}
bool Keyboard::Zoom()
{
	return keyboard[map[5]];
}
bool Keyboard::Boost()
{
	return keyboard[map[6]];
}
bool Keyboard::Exit() 
{
	return exit;
}
