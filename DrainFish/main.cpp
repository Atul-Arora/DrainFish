#include <iostream>
#include <GL/freeglut.h>
#include <cstdint>
#include <cmath>
#include <math.h>

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
#define WHITE 13
#define BLACK 14

struct Move {
	uint8_t from;
	uint8_t to;
	uint8_t promotion;
};


struct BoardState {
	uint64_t bitboards[12];
	bool WKC, WQC, BKC, BQC;
	uint64_t passantTarget;
	uint8_t turn;
};

class ChessBoard {
	BoardState* stateStack;
	int stateIndex;
public:
	std::uint8_t getPiece(std::uint8_t square) {
		std::uint64_t mask = uint64_t(1) << square;
		for (int i = 0;i < 12;i++) {
			if (mask & stateStack[stateIndex].bitboards[i]) return i;
		}
		return EMPTY;
	}

	void setPiece(std::uint8_t piece, std::uint8_t square) {
		std::uint64_t mask = uint64_t(1) << square;
		for (int i = 0;i < 12;i++) {
			if (i == piece) stateStack[stateIndex].bitboards[i] |= mask;
			else stateStack[stateIndex].bitboards[i] &= ~mask;
		}
	}
	void move(const Move& move) {
		stateStack[stateIndex + 1] = stateStack[stateIndex];
		stateIndex++;
		uint64_t fromBoard = uint64_t(1) << move.from;
		uint64_t toBoard = uint64_t(1) << move.to;
		uint64_t moveBoard = fromBoard | toBoard;
		uint8_t  movedPiece = 0;
		int start, end;
		
		if (stateStack[stateIndex].turn == WHITE)
		{
			start = 0; end = 6;
		}
		else { start = 6; end = 12; }
		//Moving a piece
		for (int i = start;i < end;i++)
		{
			if (stateStack[stateIndex].bitboards[i] & fromBoard) // Basically we are checking which piece is being moved by checking which bitboard contains the from square
			{
				stateStack[stateIndex].bitboards[i] ^= moveBoard; // 1 XOR 1 = 0 0 XOR 1 = 1
				movedPiece = i;
				break;
			}
		}
		//Capturing a piece
		if (stateStack[stateIndex].turn == WHITE)
		{
			start = 6; end = 12;
		}
		else { start = 0; end = 6; }
		//enpassant capture
		for(int i=start;i<end;i++){
			if (stateStack[stateIndex].bitboards[i] & toBoard) {
				stateStack[stateIndex].bitboards[i] ^= toBoard; // Remove the captured piece from the board
				break;
		}
		}
		if (toBoard == stateStack[stateIndex].passantTarget)
		{
			if (movedPiece == WHITE_PAWN) stateStack[stateIndex].bitboards[BLACK_PAWN] ^= stateStack[stateIndex].passantTarget << 8;
			else if (movedPiece == BLACK_PAWN) stateStack[stateIndex].bitboards[WHITE_PAWN] ^= stateStack[stateIndex].passantTarget >> 8;
		}

		// set en-passant target
		if ((movedPiece == WHITE_PAWN || movedPiece == BLACK_PAWN) && abs(move.from - move.to) == 16) stateStack[stateIndex].passantTarget = uint64_t(1) << int((move.from + move.to) * 0.5);
		else stateStack[stateIndex].passantTarget = 0;

		if (movedPiece == WHITE_KING)
		{
			if (moveBoard == 0x5000000000000000) stateStack[stateIndex].bitboards[WHITE_ROOK] ^= 0xa000000000000000;
			else if (moveBoard == 0x1400000000000000) stateStack[stateIndex].bitboards[WHITE_ROOK] ^= 0x0900000000000000;
		}
		else if (movedPiece == BLACK_KING)
		{
			if (moveBoard == 0x0000000000000050) stateStack[stateIndex].bitboards[BLACK_ROOK] ^= 0x00000000000000a0;
			else if (moveBoard == 0x0000000000000014) stateStack[stateIndex].bitboards[BLACK_ROOK] ^= 0x0000000000000009;
		}
		if (moveBoard & 0x9000000000000000) stateStack[stateIndex].WKC = false;
		if (moveBoard & 0x1100000000000000) stateStack[stateIndex].WQC = false;
		if (moveBoard & 0x0000000000000090) stateStack[stateIndex].BKC = false;
		if (moveBoard & 0x0000000000000011) stateStack[stateIndex].BQC = false;

		// promotions
		if (move.promotion != EMPTY)
		{
			stateStack[stateIndex].bitboards[stateStack[stateIndex].turn == WHITE ? WHITE_PAWN : BLACK_PAWN] ^= toBoard;
			stateStack[stateIndex].bitboards[move.promotion] ^= toBoard;
		}

		if (stateStack[stateIndex].turn == WHITE) stateStack[stateIndex].turn = BLACK;
		else stateStack[stateIndex].turn = WHITE;
	}
	void Undo(){
		if (stateIndex > 0) --stateIndex;
	}


	ChessBoard() {
		stateStack = new BoardState[500];
		stateIndex = 0;
		stateStack[0].bitboards[WHITE_PAWN] = 0x00ff000000000000;
		stateStack[0].bitboards[WHITE_KNIGHT] = 0x4200000000000000;
		stateStack[0].bitboards[WHITE_BISHOP] = 0x2400000000000000;
		stateStack[0].bitboards[WHITE_ROOK] = 0x8100000000000000;
		stateStack[0].bitboards[WHITE_QUEEN] = 0x0800000000000000;
		stateStack[0].bitboards[WHITE_KING] = 0x1000000000000000;
		stateStack[0].bitboards[BLACK_PAWN] = 0x000000000000ff00;
		stateStack[0].bitboards[BLACK_KNIGHT] = 0x0000000000000042;
		stateStack[0].bitboards[BLACK_BISHOP] = 0x0000000000000024;
		stateStack[0].bitboards[BLACK_ROOK] = 0x0000000000000081;
		stateStack[0].bitboards[BLACK_QUEEN] = 0x0000000000000008;
		stateStack[0].bitboards[BLACK_KING] = 0x0000000000000010;
		cout << "Initialized";
		stateStack[0].WKC = true;
		stateStack[0].WQC = true;
		stateStack[0].BKC = true;
		stateStack[0].BQC = true;

		stateStack[0].passantTarget=0;

		stateStack[0].turn = WHITE;

	}

	~ChessBoard() {
		delete[] stateStack;
	};

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

Move moveStack[500];
int moveIndex = 0;
int promote_x = -1;
int promote_y = -1;

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
	// show the previous move
	glLineWidth(square_size * 0.15);
	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	glVertex2f(square_x + square_size * (moveStack[moveIndex - 1].from % 8 + 0.5), square_y + square_size * (moveStack[moveIndex - 1].from / 8 + 0.5));
	glVertex2f(square_x + square_size * (moveStack[moveIndex - 1].to % 8 + 0.5), square_y + square_size * (moveStack[moveIndex - 1].to / 8 + 0.5));
	glEnd();

	for (int x = 0; x < 8; ++x)
		for (int y = 0; y < 8; ++y)
		{
			uint8_t square = x + y * 8;
			drawPiece(square_x + square_size * (0.5 + x), square_y + square_size * (0.5 + y), square_size, game.getPiece(square));
		}
	if (promote_y == 0)
	{
		glBegin(GL_QUADS);

		glColor3f(1, 1, 1);
		glVertex2f(square_x + square_size * (promote_x - 0.1), square_y - square_size * 0.1);
		glVertex2f(square_x + square_size * (promote_x + 1.1), square_y - square_size * 0.1);
		glVertex2f(square_x + square_size * (promote_x + 1.1), square_y + square_size * 4.1);
		glVertex2f(square_x + square_size * (promote_x - 0.1), square_y + square_size * 4.1);


		glColor3f(0, 0, 0);
		glVertex2f(square_x + square_size * promote_x, square_y);
		glVertex2f(square_x + square_size * (promote_x + 1), square_y);
		glVertex2f(square_x + square_size * (promote_x + 1), square_y + square_size * 4);
		glVertex2f(square_x + square_size * promote_x, square_y + square_size * 4);

		glEnd();

		drawPiece(square_x + square_size * (0.5 + promote_x), square_y + square_size * 0.5, square_size, WHITE_QUEEN);
		drawPiece(square_x + square_size * (0.5 + promote_x), square_y + square_size * 1.5, square_size, WHITE_ROOK);
		drawPiece(square_x + square_size * (0.5 + promote_x), square_y + square_size * 2.5, square_size, WHITE_BISHOP);
		drawPiece(square_x + square_size * (0.5 + promote_x), square_y + square_size * 3.5, square_size, WHITE_KNIGHT);
	}
	if (promote_y == 7)
	{
		glBegin(GL_QUADS);

		glColor3f(0, 0, 0);
		glVertex2f(square_x + square_size * (promote_x - 0.1), square_y + square_size * 8.1);
		glVertex2f(square_x + square_size * (promote_x + 1.1), square_y + square_size * 8.1);
		glVertex2f(square_x + square_size * (promote_x + 1.1), square_y + square_size * 3.9);
		glVertex2f(square_x + square_size * (promote_x - 0.1), square_y + square_size * 3.9);


		glColor3f(1, 1, 1);
		glVertex2f(square_x + square_size * promote_x, square_y + square_size * 8);
		glVertex2f(square_x + square_size * (promote_x + 1), square_y + square_size * 8);
		glVertex2f(square_x + square_size * (promote_x + 1), square_y + square_size * 4);
		glVertex2f(square_x + square_size * promote_x, square_y + square_size * 4);

		glEnd();

		drawPiece(square_x + square_size * (0.5 + promote_x), square_y + square_size * 7.5, square_size, BLACK_QUEEN);
		drawPiece(square_x + square_size * (0.5 + promote_x), square_y + square_size * 6.5, square_size, BLACK_ROOK);
		drawPiece(square_x + square_size * (0.5 + promote_x), square_y + square_size * 5.5, square_size, BLACK_BISHOP);
		drawPiece(square_x + square_size * (0.5 + promote_x), square_y + square_size * 4.5, square_size, BLACK_KNIGHT);
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

		// if we need to promote, then promote a piece
		if (promote_y == 0)
		{
			if (click_x == promote_x)
			{
				uint8_t promotion = EMPTY;
				if (click_y == 0) promotion = WHITE_QUEEN;
				else if (click_y == 1) promotion = WHITE_ROOK;
				else if (click_y == 2) promotion = WHITE_BISHOP;
				else if (click_y == 3) promotion = WHITE_KNIGHT;

				if (promotion != EMPTY)
				{
					moveStack[moveIndex - 1].promotion = promotion;
					game.Undo();
					game.move(moveStack[moveIndex - 1]);

					promote_x = -1;
					promote_y = -1;
				}
			}
		}
		else if (promote_y == 7)
		{
			if (click_x == promote_x)
			{
				uint8_t promotion = EMPTY;
				if (click_y == 7) promotion = BLACK_QUEEN;
				else if (click_y == 6) promotion = BLACK_ROOK;
				else if (click_y == 5) promotion = BLACK_BISHOP;
				else if (click_y == 4) promotion = BLACK_KNIGHT;

				if (promotion != EMPTY)
				{
					moveStack[moveIndex - 1].promotion = promotion;
					game.Undo();
					game.move(moveStack[moveIndex - 1]);

					promote_x = -1;
					promote_y = -1;
				}
			}
		}

		else if (select_x < 8 && select_x >= 0 && select_y < 8 && select_y >= 0 && game.getPiece(select_x + 8 * select_y)  != EMPTY) 
		{
			if (click_x < 8 && click_x >= 0 && click_y < 8 && click_y >= 0 && select_square!=click_square) {
				moveStack[moveIndex].from = select_square;
				moveStack[moveIndex].to = click_square;
				moveStack[moveIndex].promotion = EMPTY;
				game.move(moveStack[moveIndex]);
				moveIndex++;

				if (click_y == 7 && game.getPiece(click_square) == BLACK_PAWN || click_y == 0 && game.getPiece(click_square) == WHITE_PAWN)
				{
					promote_x = click_x;
					promote_y = click_y;
				}
				else
				{
					promote_x = -1;
					promote_y = -1;
				}

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
	if (key == ' ') // flip the board if the user presses the spacebar
	{
		square_x += 8 * square_size;
		square_y += 8 * square_size;
		square_size *= -1;
	}
	if (key == '\b') {
		game.Undo();
		--moveIndex;
	}
}


// timer function
void timer(int value)
{
	// update the display every 16ms
	glutPostRedisplay();
	glutTimerFunc(16, timer, NULL);
}

void resize(int w, int h)
{
	// set global width and height
	width = w;
	height = h;

	// recalculate board parameters
	bool isFlipped = square_size < 0;
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

	if (isFlipped) keydown(' ', 0, 0);

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