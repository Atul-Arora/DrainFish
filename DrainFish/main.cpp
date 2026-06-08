#include <iostream>
#include <GL/freeglut.h>
#include <cstdint>
#include <cmath>

using namespace std;
#define WHITE_PAWN 0
#define WHITE_KNIGHT 1
#define WHITE_BISHOP 2
#define WHITE_ROOK 3
#define WHITE_QUEEN 4
#define WHITE_KING 5
#define BLACK_PAWN 6
#define BLACK_KNIGHT 7
#define BLACK_BISHOP 8
#define BLACK_ROOK 9
#define BLACK_QUEEN 10
#define BLACK_KING 11
#define EMPTY 12

class ChessBoard {
	std::uint64_t bitboards[12];
public:
	std::uint8_t getPiece(std::uint8_t square) {
		std::uint64_t mask = uint64_t(1) << square;
		for (int i = 0;i < 12;i++) {
			if (mask & bitboards[i]) return i;
		}
		return EMPTY;
	}

	void setPiece(std::uint8_t piece, std::uint8_t square) {
		std::int64_t mask = uint64_t(1) << square;
		for (int i = 0;i < 12;i++) {
			if (i == piece) bitboards[i] |= mask;
			else bitboards[i] &= ~mask;
		}


	}

	ChessBoard() {
		bitboards[WHITE_PAWN] = 0x00ff000000000000;
		bitboards[WHITE_KNIGHT] = 0x2400000000000000;
		bitboards[WHITE_BISHOP] = 0x4200000000000000;
		bitboards[WHITE_ROOK] = 0x8100000000000000;
		bitboards[WHITE_QUEEN] = 0x0800000000000000;
		bitboards[WHITE_KING] = 0x1000000000000000;
		bitboards[BLACK_PAWN] = 0x000000000000ff00;
		bitboards[BLACK_KNIGHT] = 0x0000000000000024;
		bitboards[BLACK_BISHOP] = 0x0000000000000042;
		bitboards[BLACK_ROOK] = 0x0000000000000081;
		bitboards[BLACK_QUEEN] = 0x0000000000000008;
		bitboards[BLACK_KING] = 0x0000000000000010;
		cout << "Initialized";
	}

};
ChessBoard game;

// global variables for window width and height
int width = 640;
int height = 480;

// global variables for board drawing parameters
double board_x = 0.5 * (width - height);
double board_y = 0;
double board_size = height;

double square_size = board_size * 0.9 * 0.125;
double square_x = board_x + board_size * 0.05;
double square_y = board_y + board_size * 0.05;

// These are board coordinates not screen coordinates 
int select_x = -1;
int select_y = -1;

void drawPiece(int x, int y, int size, int piece)
{
	glBegin(GL_POLYGON);
	glColor3f(1, 1, 1);

	size = abs(size);

	switch (piece)
	{
	case BLACK_PAWN:
		glColor3f(0, 0, 0);
	case WHITE_PAWN:
		glVertex2i(x - size / 16, y);
		glVertex2i(x - size / 6, y + size / 3);
		glVertex2i(x + size / 6, y + size / 3);
		glVertex2i(x + size / 16, y);
		glVertex2i(x + size / 8, y - size / 16);
		glVertex2i(x + size / 8, y - size / 6);
		glVertex2i(x + size / 16, y - size / 4);
		glVertex2i(x - size / 16, y - size / 4);
		glVertex2i(x - size / 8, y - size / 6);
		glVertex2i(x - size / 8, y - size / 16);
		break;
	case BLACK_KNIGHT:
		glColor3f(0, 0, 0);
	case WHITE_KNIGHT:
		glVertex2i(x + size / 8, y);
		glVertex2i(x + size / 8, y + size / 3);
		glVertex2i(x - size / 4, y + size / 3);
		glVertex2i(x - size / 8, y - size / 3);
		glVertex2i(x - size / 16, y - size / 4);
		glVertex2i(x + size / 12, y - size / 4);
		glVertex2i(x + size / 3, y - size / 5);
		glVertex2i(x + size / 3, y);
		break;
	case BLACK_BISHOP:
		glColor3f(0, 0, 0);
	case WHITE_BISHOP:
		glVertex2i(x - size / 6, y + size / 3);
		glVertex2i(x - size / 8, y + size / 3.5);
		glVertex2i(x - size / 5, y + size / 6);
		glVertex2i(x - size / 5, y);
		glVertex2i(x - size / 32, y - size / 3);
		glVertex2i(x - size / 24, y - size / 3);
		glVertex2i(x - size / 24, y - size / 2.7);
		glVertex2i(x + size / 24, y - size / 2.7);
		glVertex2i(x + size / 24, y - size / 3);
		glVertex2i(x + size / 32, y - size / 3);
		glVertex2i(x + size / 5, y - size / 32);
		glVertex2i(x - size / 8, y + size / 4);
		glVertex2i(x + size / 5, y + size / 16);
		glVertex2i(x + size / 5, y + size / 6);
		glVertex2i(x + size / 8, y + size / 3.5);
		glVertex2i(x + size / 6, y + size / 3);
		break;
	case BLACK_ROOK:
		glColor3f(0, 0, 0);
	case WHITE_ROOK:
		glVertex2i(x, y + size / 3);
		glVertex2i(x - size / 4, y + size / 3);
		glVertex2i(x - size / 4, y + size / 4);
		glVertex2i(x - size / 6, y + size / 4);
		glVertex2i(x - size / 6, y);
		glVertex2i(x - size / 5, y);
		glVertex2i(x - size / 5, y - size / 5);
		glVertex2i(x - size / 8, y - size / 5);
		glVertex2i(x - size / 8, y - size / 10);
		glVertex2i(x - size / 24, y - size / 10);
		glVertex2i(x - size / 24, y - size / 5);
		glVertex2i(x + size / 24, y - size / 5);
		glVertex2i(x + size / 24, y - size / 10);
		glVertex2i(x + size / 8, y - size / 10);
		glVertex2i(x + size / 8, y - size / 5);
		glVertex2i(x + size / 5, y - size / 5);
		glVertex2i(x + size / 5, y);
		glVertex2i(x + size / 6, y);
		glVertex2i(x + size / 6, y + size / 4);
		glVertex2i(x + size / 4, y + size / 4);
		glVertex2i(x + size / 4, y + size / 3);
		break;
	case BLACK_KING:
		glColor3f(0, 0, 0);
	case WHITE_KING:
		glEnd();
		glBegin(GL_QUADS);
		glVertex2i(x - size / 16, y - size / 2.5);
		glVertex2i(x + size / 16, y - size / 2.5);
		glVertex2i(x + size / 16, y);
		glVertex2i(x - size / 16, y);
		glVertex2i(x - size / 8, y - size / 3.5);
		glVertex2i(x + size / 8, y - size / 3.5);
		glVertex2i(x + size / 8, y - size / 3);
		glVertex2i(x - size / 8, y - size / 3);
		glEnd();
		glBegin(GL_POLYGON);
		goto continue_draw;
	case BLACK_QUEEN:
		glColor3f(0, 0, 0);
	case WHITE_QUEEN:
	continue_draw:
		glVertex2i(x - size / 4, y + size / 3);
		glVertex2i(x - size / 3, y - size / 6);
		glVertex2i(x - size / 5, y - size / 7);
		glVertex2i(x - size / 16, y - size / 5);
		glVertex2i(x - size / 16, y - size / 3.5);
		glVertex2i(x + size / 16, y - size / 3.5);
		glVertex2i(x + size / 15, y - size / 5);
		glVertex2i(x + size / 5, y - size / 7);
		glVertex2i(x + size / 3, y - size / 6);
		glVertex2i(x + size / 4, y + size / 3);
		break;
	default:
		break;
	}
	glEnd();
}


// function to update the screen
void draw()
{
	// clear the buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// draw the board background
	glColor3f(0.3, 0.15, 0.1);
	glBegin(GL_QUADS);
	glVertex2f(board_x, board_y);
	glVertex2f(board_x + board_size, board_y);
	glVertex2f(board_x + board_size, board_y + board_size);
	glVertex2f(board_x, board_y + board_size);
	glEnd();

	// draw the board squares
	glBegin(GL_QUADS);
	for (int x = 0; x < 8; ++x)
		for (int y = 0; y < 8; ++y)
		{
			if ((x + y) % 2) glColor3f(0.05, 0.3, 0.05);
			else glColor3f(0.7, 0.65, 0.55);

			glVertex2f(square_x + square_size * x, square_y + square_size * y);
			glVertex2f(square_x + square_size * (x + 1), square_y + square_size * y);
			glVertex2f(square_x + square_size * (x + 1), square_y + square_size * (y + 1));
			glVertex2f(square_x + square_size * x, square_y + square_size * (y + 1));
		}
	glEnd();

	// highlight the selected square
	if (select_x >= 0 && select_y >= 0 && select_x < 8 && select_y < 8)
	{
		glColor3f(0, 1, 0);
		glBegin(GL_QUADS);

		glVertex2f(square_x + square_size * select_x, square_y + square_size * select_y);
		glVertex2f(square_x + square_size * (select_x + 1), square_y + square_size * select_y);
		glVertex2f(square_x + square_size * (select_x + 1), square_y + square_size * (select_y + 1));
		glVertex2f(square_x + square_size * select_x, square_y + square_size * (select_y + 1));

		glEnd();
	}

	for (int x = 0; x < 8; ++x)
		for (int y = 0; y < 8; ++y)
		{
			uint8_t square = x + y * 8;
			drawPiece(square_x + square_size * (0.5 + x), square_y + square_size * (0.5 + y), square_size, game.getPiece(square));
		}

	// update the display by swapping buffers
	glutSwapBuffers();
}

// function to handle mouse input
void mouse(int button, int state, int x, int y)
{
	// select a square when we click on the window
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		int click_x = floor((x - square_x) / square_size);
		int click_y = floor((y - square_y) / square_size);
		int select_square = select_x + 8 * select_y;
		int click_square = click_x + 8 * click_y;


		if (select_x < 8 && select_x >= 0 && select_y < 8 && select_y >= 0 && game.getPiece(select_x + 8 * select_y)  != EMPTY) 
		{
			if (click_x < 8 && click_x >= 0 && click_y < 8 && click_y >= 0 && select_square!=click_square) {
				game.setPiece(game.getPiece(select_x + 8 * select_y), click_x + 8 * click_y);
				game.setPiece(EMPTY, select_x + 8 * select_y);
			}
			select_x = -1;
			select_y = -1;

		}
		else {
			select_x = click_x;
			select_y = click_y;
		}

		cout <<"X coordinate is : " << click_x << endl;
		cout <<"Y coordinate is : " << click_y << endl;
	}
}

// function to handle keyboard input
void keydown(unsigned char key, int x, int y)
{
	if (key == 27) glutLeaveMainLoop(); // exit when the user presses esc
}

// timer function
void timer(int value)
{
	// update the display every 16ms
	glutPostRedisplay();
	glutTimerFunc(16, timer, NULL);
}

// function to handle window resizing
void resize(int w, int h)
{
	// set global width and height
	width = w;
	height = h;

	// recalculate board parameters
	if (width > height)
	{
		board_size = height;
		board_x = 0.5 * (width - height);
		board_y = 0;
	}
	else
	{
		board_size = width;
		board_x = 0;
		board_y = 0.5 * (height - width);
	}

	square_size = board_size * 0.9 * 0.125;
	square_x = board_x + board_size * 0.05;
	square_y = board_y + board_size * 0.05;

	// reset the viewport
	glViewport(0, 0, width, height);
	glLoadIdentity();
	gluOrtho2D(0, width, height, 0);
}

// main function
int main(int argc, char** argv)
{
	// initialize OpenGL
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	// create a window
	glutInitWindowSize(width, height);
	glutCreateWindow("Chess Tutorial");

	// set the clear color
	glClearColor(0, 0, 0, 1);

	// set up a 2D projection
	gluOrtho2D(0, width, height, 0);

	// register callback functions
	glutDisplayFunc(draw);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keydown);
	glutTimerFunc(16, timer, NULL);
	glutReshapeFunc(resize);

	// enter GLUT's main rendering loop
	glutMainLoop();

	// end program
	return 0;
}