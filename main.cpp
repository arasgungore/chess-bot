#include "Header/chess.h"
#include <iostream>
#include <time.h>
#include <windows.h>

int main() {
	srand((unsigned int)time(NULL));
	Chess c("ArasDasDas", WHITE_BOT_DIFFICULTY, "MCihan899", BLACK_BOT_DIFFICULTY);
	ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
	if(AGAINST_BOT) {
		do {
			c.PrintBoard();
			if(TWO_BOTS) {				// bot vs bot (no human)
				MoveCursorToXY(RIGHT, DOWN + 3*BOARD_SIZE + 5);
				std::cout << CLEAR_LINE << std::endl << std::endl << CLEAR_LINE;
				while(c.BotsTurn());
			}
			else {
				if(BOT_IS_WHITE)
					while(true) {		// team black player vs team white bot
						MoveCursorToXY(RIGHT, DOWN + 3*BOARD_SIZE + 5);
						std::cout << CLEAR_LINE << std::endl << std::endl << CLEAR_LINE;
						if(!c.BotsTurn())
							break;
						std::cout << std::endl << TO_RIGHT << "Enter move coordinates (x1,y1)->(x2,y2):";
						std::cout << std::endl << std::endl << TO_RIGHT << "All possible moves:" << std::endl;
						if(!c.PlayersTurn())
							break;
					}
				else
					while(true) {		// team white player vs team black bot
						if(!c.PlayersTurn())
							break;
						MoveCursorToXY(RIGHT, DOWN + 3*BOARD_SIZE + 5);
						std::cout << CLEAR_LINE << std::endl << std::endl << CLEAR_LINE;
						if(!c.BotsTurn())
							break;
						std::cout << std::endl << TO_RIGHT << "Enter move coordinates (x1,y1)->(x2,y2):";
						std::cout << std::endl << std::endl << TO_RIGHT << "All possible moves:" << std::endl;
					}
			}
		} while(c.GameOver());
		exit(0);
	}
	do {								// player vs player (no bots)
		c.PrintBoard();
		while(c.PlayersTurn());
	} while(c.GameOver());
}
