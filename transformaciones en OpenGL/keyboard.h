# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>
# include <iostream>

class Keyboard
{
private:
	static bool keyboard[256]; // Booleano para cada tecla que indica si esta esta presionada o no
	static bool exit; // Indica si se presiono la combinacion alt+F4
	unsigned char map[7]; // Mapa de teclas
public:
	Keyboard(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
	
	void InitKeyboard();
	
	static void KeyPressed_cb(unsigned char key, int x, int y);
	static void KeyRelease_cb(unsigned char key, int x, int y);
	static void Special_cb(int key, int xmodif, int ymodif);
	
	int xmodif = 0;
	int ymodif = 0;
	
	bool Shoot();
	bool Fordward();
	bool Back();
	bool Left();
	bool Right();
	bool Exit();
};
