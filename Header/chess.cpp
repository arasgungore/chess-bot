#include "chess.h"
#include <iostream>
#include <algorithm>
#include <conio.h>
#include <windows.h>

// function definitions

// moves the cursor to a different (x, y) location on the terminal
void MoveCursorToXY(const short &x, const short &y) noexcept {
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD){x, y});
}

// returns the given string in lowercase
std::string ToLowerString(std::string s) noexcept {
	transform(s.begin(), s.end(), s.begin(), [](const unsigned char &c){ return tolower(c); });
	return s;
}

// returns a random number between "min" and "max"
template<class T> T GetRandomNumber(const T &min, const T &max) noexcept {
	return min + T(static_cast<double>(rand()) / static_cast<double>(RAND_MAX+1.0) * (max-min+1));
}



// chess class implementation

// constructor of chess class
Chess::Chess(const std::string &player1, const unsigned short &difficulty1, const std::string &player2, const unsigned short &difficulty2) noexcept
: white(player1, difficulty1), black(player2, difficulty2) {
	CopyBoard(STARTING_BOARD, board);
}

// checks whether the given coordinate is within board boundaries or not
bool Chess::WithinBounds(const short &coord) noexcept {
	return coord>=0 && coord<BOARD_SIZE;
}

// changes the given board coordinates from ASCII to numerical, e.g. ('d', '3') -> (3, 5)
void Chess::ChangeToRealCoordinates(char &x1, char &y1, char &x2, char &y2) noexcept {
	x1 -= 'a', x2 -= 'a';
	y1 = '8'-y1, y2 = '8'-y2;
}

// changes the given board coordinates from numerical to ASCII, e.g. (3, 5) -> ('d', '3')
void Chess::ChangeToString(char &x1, char &y1, char &x2, char &y2) noexcept {
	x1 += 'a', x2 += 'a';
	y1 = '8'-y1, y2 = '8'-y2;
}

// returns the given numerical board coordinates as a string
std::string Chess::ToString(const short &x1, const short &y1, const short &x2, const short &y2) noexcept {
	return {static_cast<char>(x1+'a'), static_cast<char>('8'-y1), static_cast<char>(x2+'a'), static_cast<char>('8'-y2)};
}

// returns the name that is displayed on the terminal for the given piece
std::string Chess::PieceNameToString(const char &piece) noexcept {
	switch(piece) {
		case W_PAWN:	return "W_PAWN";
		case B_PAWN:	return "B_PAWN";
		case W_ROOK:	return "W_ROOK";
		case B_ROOK:	return "B_ROOK";
		case W_KNIGHT:	return "W_KNIGHT";
		case B_KNIGHT:	return "B_KNIGHT";
		case W_BISHOP:	return "W_BISHOP";
		case B_BISHOP:	return "B_BISHOP";
		case W_QUEEN:	return "W_QUEEN";
		case B_QUEEN:	return "B_QUEEN";
		case W_KING:	return "W_KING";
		case B_KING:	return "B_KING";
		default:		return "";
	}
}

// returns the worth of the given piece in points
float Chess::EvaluatePiece(const char &piece) noexcept {
	switch(piece) {
		case W_PAWN:
		case B_PAWN:	return 10;		// pawn: 10 points
		case W_ROOK:
		case B_ROOK:	return 50;		// rook: 50 points
		case W_KNIGHT:
		case B_KNIGHT:
		case W_BISHOP:
		case B_BISHOP:	return 30;		// knight and bishop: 30 points
		case W_QUEEN:
		case B_QUEEN:	return 90;		// queen: 90 points
		case W_KING:
		case B_KING:	return 900;		// king: 900 points, but you can assume it is infinite
		default:		return 0;
	}
}

// clears all available moves from the terminal
void Chess::ClearAllMoves(const unsigned short &n) noexcept {
	MoveCursorToXY(0, DOWN + 3*BOARD_SIZE + 9);
	for(unsigned short i=0;i<n;++i)
		std::cout << CLEAR_LINE << std::endl;
}

// prints the vertical seperators for the game board on the terminal
void Chess::PrintSeparator(const char &ch) noexcept {
	for(unsigned short i=1;i<BOARD_SIZE;++i)
		std::cout << std::string(BOX_WIDTH, ch) << "|";
	std::cout << std::string(BOX_WIDTH, ch) << std::endl << TO_RIGHT;
}

// copies the board contents of "from" to "to"
void Chess::CopyBoard(const char from[BOARD_SIZE][BOARD_SIZE], char to[BOARD_SIZE][BOARD_SIZE]) noexcept {
	std::copy(*from, *from + BOARD_SIZE*BOARD_SIZE, *to);
}

// checks whether the board positions of the given two boards are equal or not
bool Chess::AreBoardsEqual(const char board1[BOARD_SIZE][BOARD_SIZE], const char board2[BOARD_SIZE][BOARD_SIZE]) noexcept {
	return std::equal(*board1, *board1 + BOARD_SIZE*BOARD_SIZE, *board2);
}

bool Chess::CanMovePiece(const short &x1, const short &y1, const short &x2, const short &y2, const std::forward_list<std::string> &all_moves) noexcept {
	return std::find(all_moves.cbegin(), all_moves.cend(), ToString(x1, y1, x2, y2)) != all_moves.cend();
}

// getter method, returns the piece located in the given x and y coordinates
char Chess::GetPiece(const short &x, const short &y) const noexcept {
//	CheckCoordinates(x, y, "GetPiece");
	return board[y][x];
}

// getter method, returns whether it's team white's turn or not
bool Chess::GetTurn() const noexcept {
	return whites_turn;
}

// returns a reference to the player object
Bot& Chess::GetCurrentPlayer() noexcept {
	return whites_turn ? white : black;
}

// returns a copy of the player object
Bot Chess::GetCurrentPlayerConst() const noexcept {
	return whites_turn ? white : black;
}

Bot& Chess::GetOtherPlayer() noexcept {
	return whites_turn ? black : white;
}

Bot Chess::GetOtherPlayerConst() const noexcept {
	return whites_turn ? black : white;
}

// changes whose turn it is so the other player can make its move
void Chess::ChangeTurn() noexcept {
	whites_turn = !whites_turn;
}

// appends the last made game move to "all_game_moves" list
void Chess::AppendToAllGameMoves(const short &x1, const short &y1, const short &x2, const short &y2) noexcept {
	if(GetCurrentPlayerConst().GetCastling() && (board[y1][x1] == B_KING + 7*whites_turn) && (x2 == 2 || x2 == 6))
		all_game_moves.emplace_back(CASTLING, std::string(1, x2));
	else
		all_game_moves.emplace_back(NORMAL, ToString(x1, y1, x2, y2) + board[y1][x1] + board[y2][x2]);
}

// resets the game, everything is back to its default value
void Chess::Reset() noexcept {
	CopyBoard(STARTING_BOARD, board);
	white.Reset();
	black.Reset();
	all_game_moves.clear();
	whites_turn = true;
	moves_after_last_pawn_move_or_capture = 0;
	system("cls");
}

// a function for exception handling, checks whether the given coordinates is within board boundaries or not
void Chess::CheckCoordinates(const short &x, const short &y, const std::string &func_name) const noexcept(false) {
	try {
		if(!WithinBounds(x))		throw x;
		if(!WithinBounds(y))		throw y;
	}
	catch(const short &coord) {
		std::cerr << std::endl << std::endl << TO_RIGHT << "!ERROR!\t\tInvalid coordinate: '" << coord << "'.\t\t!ERROR!";
		std::cerr << std::endl << TO_RIGHT << "      \t\tException occurred in \"" << func_name << "\".";
		PrintAllMovesMadeInOrder();
		exit(1);
	}
}

bool Chess::EndGameText(const unsigned short &n, const Endgame &end_game) const noexcept {
	ClearAllMoves(n);
	MoveCursorToXY(RIGHT, DOWN + 3*BOARD_SIZE + 7);
	switch(end_game) {
		case CHECKMATE:
			std::cout << "!!!Checkmate!!!" << CLEAR_LINE << std::endl << TO_RIGHT << GetOtherPlayerConst().GetName() << " wins!";
			return true;
		default:
			std::cout << "!!!Draw!!!" << CLEAR_LINE << std::endl << TO_RIGHT;
			switch(end_game) {
				case FIFTY_MOVES:
					std::cout << "Fifty-move rule: No capture has been made and no pawn has been moved in the last 50 moves.";
					return true;
				case THREEFOLD_REP:
					std::cout << "Threefold repetition: Last position occured 3 times during the game.";
					return true;
				default:
					return false;
			}
	}
}

// returns the x coordinate of the eaten pawn piece
short Chess::GetEnPassant(const short &x, const short &y) const noexcept {
	if(all_game_moves.empty())
		return -1;
	if(all_game_moves.back().first != NORMAL)
		return -1;
	auto last_move = all_game_moves.back().second;
	ChangeToRealCoordinates(last_move[0], last_move[1], last_move[2], last_move[3]);
	return ((last_move[4] == W_PAWN - 7*whites_turn) && (abs(last_move[0]-x) == 1) && (last_move[3]-last_move[1] == 2*(whites_turn ? 1 : -1)) && (y == 4-whites_turn)) ? last_move[0] : -1;
}

template<class Iterator> short Chess::GetEnPassant(const char board[BOARD_SIZE][BOARD_SIZE], const Iterator &it) const noexcept {
	if(it->first != NORMAL)
		return -1;
	auto last_move = it->second;
	ChangeToRealCoordinates(last_move[0], last_move[1], last_move[2], last_move[3]);
	for(short x=0;x<BOARD_SIZE;++x)
		if(board[3+whites_turn][x] == W_PAWN - 7*whites_turn)
			if((last_move[4] == B_PAWN + 7*whites_turn) && (abs(last_move[0]-x) == 1) && (last_move[3]-last_move[1] == 2*(whites_turn ? -1 : 1)))
				return last_move[0];
	return -1;
}

bool Chess::ThreefoldRepetition() const noexcept {
	static char prev_board[BOARD_SIZE][BOARD_SIZE];
	CopyBoard(board, prev_board);
	unsigned short position_count = 1;
	auto it = all_game_moves.crbegin();
	auto last_move = it->second;
	while(true) {
		for(unsigned short i=0;i<2;++i)	{
			switch(it->first) {
				case CASTLING:
					return false;		// if a pawn has moved or a castling/capture has occured, then that board position is unique and repetition is not possible
				default:
					if(last_move[4] == W_PAWN || last_move[4] == B_PAWN || last_move[5] != EMPTY)
						return false;
					ChangeToRealCoordinates(last_move[0], last_move[1], last_move[2], last_move[3]);
					prev_board[short(last_move[1])][short(last_move[0])] = last_move[4], prev_board[short(last_move[3])][short(last_move[2])] = EMPTY;
					if(it->first == EN_PASSANT)
						prev_board[short(last_move[1])][short(last_move[2])] = i == whites_turn ? B_PAWN : W_PAWN;
			}
			if((++it) == all_game_moves.crend())
				return false;
			last_move = it->second;
		}
		if(AreBoardsEqual(prev_board, board))		// check for castling and en passant in every 2 turns
			if(GetOtherPlayerConst().GetCastling() == (it->first == CASTLING ? false : last_move[6 + (it->first == PROMOTION)]))
				if((all_game_moves.size() > 1 ? GetEnPassant(board, prev(all_game_moves.cend(), 2)) : -1)
				== (next(it) == all_game_moves.crend() ? -1 : GetEnPassant(prev_board, next(it))))
					if((++position_count) == 3)
						return true;
	}
}

bool Chess::IsCheck(const bool &turn) const noexcept {
	short x = -1, y = -1;
	for(short i=0;x==-1;++i)
		for(short j=0;j<BOARD_SIZE;++j)
			if(board[j][i] == B_KING + 7*turn) {
				x = i, y = j;
				break;
			}
	for(short i=x+1;i<BOARD_SIZE;++i)						// check queen and rook
		if(board[y][i] == W_ROOK - 7*turn)			return true;
		else if(board[y][i] == W_QUEEN - 7*turn)	return true;
		else if(board[y][i] != EMPTY)	break;
	for(short i=x-1;i>=0;--i)
		if(board[y][i] == W_ROOK - 7*turn)			return true;
		else if(board[y][i] == W_QUEEN - 7*turn)	return true;
		else if(board[y][i] != EMPTY)	break;
	for(short i=y+1;i<BOARD_SIZE;++i)
		if(board[i][x] == W_ROOK - 7*turn)			return true;
		else if(board[i][x] == W_QUEEN - 7*turn)	return true;
		else if(board[i][x] != EMPTY)	break;
	for(short i=y-1;i>=0;--i)
		if(board[i][x] == W_ROOK - 7*turn)			return true;
		else if(board[i][x] == W_QUEEN - 7*turn)	return true;
		else if(board[i][x] != EMPTY)	break;
	for(short i=x-1, j=y-1; i>=0 && j>=0; --i, --j)			// check queen and bishop
		if(board[j][i] == W_BISHOP - 7*turn)		return true;
		else if(board[j][i] == W_QUEEN - 7*turn)	return true;
		else if(board[j][i] != EMPTY)	break;
	for(short i=x-1, j=y+1; i>=0 && j<BOARD_SIZE; --i, ++j)
		if(board[j][i] == W_BISHOP - 7*turn)		return true;
		else if(board[j][i] == W_QUEEN - 7*turn)	return true;
		else if(board[j][i] != EMPTY)	break;
	for(short i=x+1, j=y-1; i<BOARD_SIZE && j>=0; ++i, --j)
		if(board[j][i] == W_BISHOP - 7*turn)		return true;
		else if(board[j][i] == W_QUEEN - 7*turn)	return true;
		else if(board[j][i] != EMPTY)	break;
	for(short i=x+1, j=y+1; i<BOARD_SIZE && j<BOARD_SIZE; ++i, ++j)
		if(board[j][i] == W_BISHOP - 7*turn)		return true;
		else if(board[j][i] == W_QUEEN - 7*turn)	return true;
		else if(board[j][i] != EMPTY)	break;
	for(short i=x-1;i<x+2;++i)								// check king
		for(short j=y-1;j<y+2;++j)
			if((board[j][i] == W_KING - 7*turn) && WithinBounds(i) && WithinBounds(j))			return true;
	if((board[y-1][x-2] == W_KNIGHT - 7*turn) && (y > 0) && (x > 1))							return true;	// check knight
	else if((board[y-1][x+2] == W_KNIGHT - 7*turn) && (y > 0) && (x < BOARD_SIZE-2))			return true;
	else if((board[y+1][x-2] == W_KNIGHT - 7*turn) && (y < BOARD_SIZE-1) && (x > 1))			return true;
	else if((board[y+1][x+2] == W_KNIGHT - 7*turn) && (y < BOARD_SIZE-1) && (x < BOARD_SIZE-2))	return true;
	else if((board[y-2][x-1] == W_KNIGHT - 7*turn) && (y > 1) && (x > 0))						return true;
	else if((board[y-2][x+1] == W_KNIGHT - 7*turn) && (y > 1) && (x < BOARD_SIZE-1))			return true;
	else if((board[y+2][x-1] == W_KNIGHT - 7*turn) && (y < BOARD_SIZE-2) && (x > 0))			return true;
	else if((board[y+2][x+1] == W_KNIGHT - 7*turn) && (y < BOARD_SIZE-2) && (x < BOARD_SIZE-1))	return true;
	else if((board[y + (turn ? -1 : 1)][x+1] == W_PAWN - 7*turn) && (x < BOARD_SIZE-1))			return true;	// check pawn
	else if((board[y + (turn ? -1 : 1)][x-1] == W_PAWN - 7*turn) && (x > 0))					return true;
	return false;
}

bool Chess::IsCheck(std::string &move) noexcept {
	ChangeToRealCoordinates(move[0], move[1], move[2], move[3]);
	MovePiece(move[0], move[1], move[2], move[3], false, false);
	const bool &is_check = IsCheck(!whites_turn);
	MovePieceBack(move[0], move[1], move[2], move[3]);
	ChangeToString(move[0], move[1], move[2], move[3]);
	return is_check;
}

std::forward_list<std::string> Chess::PawnMoves(const short &x, const short &y) const noexcept {
	const auto &IsValid = whites_turn ? [](const char &ch){ return ch < 0; } : [](const char &ch){ return ch > 0; };
	const short &inc = whites_turn ? -1 : 1;
	std::forward_list<std::string> all_moves;
	if(board[y+inc][x] == EMPTY) {
		all_moves.emplace_front(ToString(x, y, x, y+inc));				// 1 square forward
		if((y == 1 + 5*whites_turn) && (board[y + 2*inc][x] == EMPTY))
			all_moves.emplace_front(ToString(x, y, x, y + 2*inc));		// 2 squares forward
	}
	if(GetEnPassant(x, y) != -1)
		all_moves.emplace_front(ToString(x, y, GetEnPassant(x, y), y+inc));
	if(IsValid(board[y+inc][x+1]) && (x < BOARD_SIZE-1))
		all_moves.emplace_front(ToString(x, y, x+1, y+inc));		// diagonal attack moves
	if(IsValid(board[y+inc][x-1]) && (x > 0))
		all_moves.emplace_front(ToString(x, y, x-1, y+inc));
	return all_moves;
}

std::forward_list<std::string> Chess::RookMoves(const short &x, const short &y) const noexcept {
	const auto &IsValid = whites_turn ? [](const char &ch){ return ch < 0; } : [](const char &ch){ return ch > 0; };
	std::forward_list<std::string> all_moves;
	for(short i=x+1;i<BOARD_SIZE;++i)			// right
		if(board[y][i] == EMPTY)
			all_moves.emplace_front(ToString(x, y, i, y));
		else {
			if(IsValid(board[y][i]))
				all_moves.emplace_front(ToString(x, y, i, y));
			break;
		}
	for(short i=x-1;i>=0;--i)					// left
		if(board[y][i] == EMPTY)
			all_moves.emplace_front(ToString(x, y, i, y));
		else {
			if(IsValid(board[y][i]))
				all_moves.emplace_front(ToString(x, y, i, y));
			break;
		}
	for(short i=y+1;i<BOARD_SIZE;++i)			// down
		if(board[i][x] == EMPTY)
			all_moves.emplace_front(ToString(x, y, x, i));
		else {
			if(IsValid(board[i][x]))
				all_moves.emplace_front(ToString(x, y, x, i));
			break;
		}
	for(short i=y-1;i>=0;--i)					// up
		if(board[i][x] == EMPTY)
			all_moves.emplace_front(ToString(x, y, x, i));
		else {
			if(IsValid(board[i][x]))
				all_moves.emplace_front(ToString(x, y, x, i));
			break;
		}
	return all_moves;
}

std::forward_list<std::string> Chess::KnightMoves(const short &x, const short &y) const noexcept {
	const auto &IsValid = whites_turn ? [](const char &ch){ return ch <= 0; } : [](const char &ch){ return ch >= 0; };
	std::forward_list<std::string> all_moves;
	if(IsValid(board[y-1][x-2]) && (y > 0) && (x > 1))
		all_moves.emplace_front(ToString(x, y, x-2, y-1));
	if(IsValid(board[y-1][x+2]) && (y > 0) && (x < BOARD_SIZE-2))
		all_moves.emplace_front(ToString(x, y, x+2, y-1));
	if(IsValid(board[y+1][x-2]) && (y < BOARD_SIZE-1) && (x > 1))
		all_moves.emplace_front(ToString(x, y, x-2, y+1));
	if(IsValid(board[y+1][x+2]) && (y < BOARD_SIZE-1) && (x < BOARD_SIZE-2))
		all_moves.emplace_front(ToString(x, y, x+2, y+1));
	if(IsValid(board[y-2][x-1]) && (y > 1) && (x > 0))
		all_moves.emplace_front(ToString(x, y, x-1, y-2));
	if(IsValid(board[y-2][x+1]) && (y > 1) && (x < BOARD_SIZE-1))
		all_moves.emplace_front(ToString(x, y, x+1, y-2));
	if(IsValid(board[y+2][x-1]) && (y < BOARD_SIZE-2) && (x > 0))
		all_moves.emplace_front(ToString(x, y, x-1, y+2));
	if(IsValid(board[y+2][x+1]) && (y < BOARD_SIZE-2) && (x < BOARD_SIZE-1))
		all_moves.emplace_front(ToString(x, y, x+1, y+2));
	return all_moves;
}

std::forward_list<std::string> Chess::BishopMoves(const short &x, const short &y) const noexcept {
	const auto &IsValid = whites_turn ? [](const char &ch){ return ch < 0; } : [](const char &ch){ return ch > 0; };
	std::forward_list<std::string> all_moves;
	for(short i=x-1, j=y-1; i>=0 && j>=0; --i, --j)						// upper left diagonal
		if(board[j][i] == EMPTY)
			all_moves.emplace_front(ToString(x, y, i, j));
		else {
			if(IsValid(board[j][i]))
				all_moves.emplace_front(ToString(x, y, i, j));
			break;
		}
	for(short i=x-1, j=y+1; i>=0 && j<BOARD_SIZE; --i, ++j)				// lower left diagonal
		if(board[j][i] == EMPTY)
			all_moves.emplace_front(ToString(x, y, i, j));
		else {
			if(IsValid(board[j][i]))
				all_moves.emplace_front(ToString(x, y, i, j));
			break;
		}
	for(short i=x+1, j=y-1; i<BOARD_SIZE && j>=0; ++i, --j)				// upper right diagonal
		if(board[j][i] == EMPTY)
			all_moves.emplace_front(ToString(x, y, i, j));
		else {
			if(IsValid(board[j][i]))
				all_moves.emplace_front(ToString(x, y, i, j));
			break;
		}
	for(short i=x+1, j=y+1; i<BOARD_SIZE && j<BOARD_SIZE; ++i, ++j)		// lower right diagonal
		if(board[j][i] == EMPTY)
			all_moves.emplace_front(ToString(x, y, i, j));
		else {
			if(IsValid(board[j][i]))
				all_moves.emplace_front(ToString(x, y, i, j));
			break;
		}
	return all_moves;
}

std::forward_list<std::string> Chess::QueenMoves(const short &x, const short &y) const noexcept {
	auto all_moves = RookMoves(x, y);			// queen = rook + bishop
	all_moves.merge(BishopMoves(x, y));
	return all_moves;
}

std::forward_list<std::string> Chess::KingMoves(const short &x, const short &y) const noexcept {
	const auto &IsValid = whites_turn ? [](const char &ch){ return ch <= 0; } : [](const char &ch){ return ch >= 0; };
	std::forward_list<std::string> all_moves;
	for(short i=x-1;i<x+2;++i)		// add moves within 1 square reach
		for(short j=y-1;j<y+2;++j)
			if(IsValid(board[j][i]) && WithinBounds(i) && WithinBounds(j))
				all_moves.emplace_front(ToString(x, y, i, j));
	if(GetCurrentPlayerConst().GetCastling())		// add castling moves if castling is possible
		if(!IsCheck(whites_turn)) {
			const short &line = (BOARD_SIZE-1)*whites_turn;
			if((board[line][0] == B_ROOK + 7*whites_turn) && board[line][1] == EMPTY && board[line][2] == EMPTY && board[line][3] == EMPTY)
				all_moves.emplace_front(ToString(4, line, 2, line));	// long castling
			else if((board[line][7] == B_ROOK + 7*whites_turn) && board[line][5] == EMPTY && board[line][6] == EMPTY)
				all_moves.emplace_front(ToString(4, line, 6, line));	// short castling
		}
	return all_moves;
}

std::forward_list<std::string> Chess::AllMoves() noexcept {
	std::forward_list<std::string> all_moves;
	for(short y=0;y<BOARD_SIZE;++y)
		for(short x=0;x<BOARD_SIZE;++x) {
			if((board[y][x] < 0) == whites_turn)
				continue;
			switch(board[y][x]) {
				case W_PAWN:
				case B_PAWN:
					all_moves.merge(PawnMoves(x, y));
					break;
				case W_ROOK:
				case B_ROOK:
					all_moves.merge(RookMoves(x, y));
					break;
				case W_KNIGHT:
				case B_KNIGHT:
					all_moves.merge(KnightMoves(x, y));
					break;
				case W_BISHOP:
				case B_BISHOP:
					all_moves.merge(BishopMoves(x, y));
					break;
				case W_QUEEN:
				case B_QUEEN:
					all_moves.merge(QueenMoves(x, y));
					break;
				case W_KING:
				case B_KING:
					all_moves.merge(KingMoves(x, y));
			}
		}
	for(auto it = all_moves.begin(), prev = all_moves.before_begin(); it != all_moves.cend();)		// if the possible move makes me checkmate after the opponent's turn, remove it from the list
		if(IsCheck(*it))
			it = all_moves.erase_after(prev);
		else
			++it, ++prev;
	return all_moves;
}

std::string Chess::GetRandomMove() noexcept {
	auto all_moves = AllMoves();
	auto move = all_moves.begin();
	advance(move, GetRandomNumber<unsigned short>(0, distance(all_moves.cbegin(), all_moves.cend()) - 1));
	ChangeToRealCoordinates((*move)[0], (*move)[1], (*move)[2], (*move)[3]);
	return *move;
}

void Chess::ManuallyPromotePawn(const short &x, const short &y) noexcept {
	MoveCursorToXY(RIGHT, DOWN + 3*BOARD_SIZE + 7);
	std::cout << "Enter your choice of promotion [(r)ook, (k)night, (b)ishop, (q)ueen]";
	char key = getch();
	while(true)
		switch(key = tolower(key)) {
			case 'r':	board[y][x] = whites_turn ? W_ROOK : B_ROOK;		return;
			case 'k':	board[y][x] = whites_turn ? W_KNIGHT : B_KNIGHT;	return;
			case 'b':	board[y][x] = whites_turn ? W_BISHOP : B_BISHOP;	return;
			case 'q':	board[y][x] = whites_turn ? W_QUEEN : B_QUEEN;		return;
			default:	key = getch();
		}
}

void Chess::MovePiece(const short &x1, const short &y1, const short &x2, const short &y2, const bool &manual_promotion, const bool &update_board) noexcept {
//	CheckCoordinates(x1, y1, "MovePiece");
//	CheckCoordinates(x2, y2, "MovePiece");
	AppendToAllGameMoves(x1, y1, x2, y2);		// similar to FEN notation but not really, the starting and ending points of the moving piece, promoted piece if there is promotion and ability to do castling
	switch(board[y1][x1]) {
		case W_PAWN:
		case B_PAWN:
			if(y2 == ((BOARD_SIZE-1) * !whites_turn)) {			// check for castling, promotion or en passant
				if(manual_promotion) {
					ManuallyPromotePawn(x1, y1);
					MoveCursorToXY(RIGHT, DOWN + 3*BOARD_SIZE + 7);
					std::cout << "All possible moves:" << CLEAR_LINE;
				}
				else if(whites_turn ? WHITE_BOT_RANDOM : BLACK_BOT_RANDOM)
					board[y1][x1] = (whites_turn ? 1 : -1) * GetRandomNumber(2, 5);
				else
					board[y1][x1] = whites_turn ? W_QUEEN : B_QUEEN;
				all_game_moves.back().first = PROMOTION;
				all_game_moves.back().second.push_back(board[y1][x1]);
			}
			else if(x1 != x2 && board[y2][x2] == EMPTY) {
				board[y1][x2] = EMPTY;
				if(update_board) {
					GetCurrentPlayer().IncreaseScore(EvaluatePiece(W_PAWN));
					UpdateScore(GetCurrentPlayerConst());
					UpdateBoard(x2, y1);
				}
				all_game_moves.back().first = EN_PASSANT;
			}
			break;
		case W_KING:
		case B_KING:
			if(GetCurrentPlayerConst().GetCastling()) {
				const short &line = (BOARD_SIZE-1) * whites_turn;
				switch(x2) {
					case 2:
						board[line][3] = board[line][0], board[line][0] = EMPTY;
						if(update_board) {
							UpdateBoard(0, line);
							UpdateBoard(3, line);
						}
						break;
					case 6:
						board[line][5] = board[line][7], board[line][7] = EMPTY;
						if(update_board) {
							UpdateBoard(7, line);
							UpdateBoard(5, line);
						}
				}
			}
		case W_ROOK:
		case B_ROOK:
			GetCurrentPlayer().SetCastling(false);
	}
	if(all_game_moves.back().first != CASTLING)				all_game_moves.back().second.push_back(GetCurrentPlayerConst().GetCastling());
	board[y2][x2] = board[y1][x1], board[y1][x1] = EMPTY;
	if(update_board) {
		if(all_game_moves.back().first != CASTLING)
			if(all_game_moves.back().second[5] != EMPTY) {
				GetCurrentPlayer().IncreaseScore(EvaluatePiece(all_game_moves.back().second[5]));
				UpdateScore(GetCurrentPlayerConst());
				moves_after_last_pawn_move_or_capture = 0;		// the piece is eaten
			}
		UpdateBoard(x1, y1);
		UpdateBoard(x2, y2);
	}
	ChangeTurn();		// it's the opponent's turn
}

// undoes the move from (x1,y1) to (x2,y2), the opposite of the "MovePiece" function
void Chess::MovePieceBack(const short &x1, const short &y1, const short &x2, const short &y2) noexcept {
	ChangeTurn();
	board[y1][x1] = board[y2][x2], board[y2][x2] = all_game_moves.back().first == CASTLING ? static_cast<char>(EMPTY) : all_game_moves.back().second[5];
	switch(board[y1][x1]) {
		case W_PAWN:
		case B_PAWN:
			if(x1 != x2 && board[y2][x2] == EMPTY)
				board[y1][x2] = whites_turn ? B_PAWN : W_PAWN;
			break;
		case W_ROOK:
		case B_ROOK:
			if(prev(all_game_moves.cend(), 3)->first != CASTLING)
				if(prev(all_game_moves.cend(), 3)->second[6 + (prev(all_game_moves.cend(), 3)->first == PROMOTION)])
					GetCurrentPlayer().SetCastling(true);
			break;
		case W_QUEEN:
		case B_QUEEN:
			if(all_game_moves.back().first == PROMOTION)
				board[y1][x1] = whites_turn ? W_PAWN : B_PAWN;
			break;
		case W_KING:
		case B_KING:
			if(all_game_moves.back().first == CASTLING) {
				GetCurrentPlayer().SetCastling(true);
				const short line = (BOARD_SIZE-1) * whites_turn;
				switch(x2) {
					case 2:
						board[line][0] = board[line][3], board[line][3] = EMPTY;
						break;
					case 6:
						board[line][7] = board[line][5], board[line][5] = EMPTY;
				}
			}
			else if(prev(all_game_moves.cend(), 3)->first != CASTLING)
				if(prev(all_game_moves.cend(), 3)->second[6 + (prev(all_game_moves.cend(), 3)->first == PROMOTION)])
					GetCurrentPlayer().SetCastling(true);
	}
	all_game_moves.pop_back();
}

void Chess::UpdateBoard(const short &x, const short &y) const noexcept {
	const unsigned short &diff = BOX_WIDTH - PieceNameToString(board[y][x]).length();
	MoveCursorToXY(RIGHT + (BOX_WIDTH+1)*x, DOWN + 3*y + 1);
	std::cout << std::string(diff/2, ' ') << PieceNameToString(board[y][x]) << std::string(diff/2, ' ');
	if(diff%2)	std::cout << " ";
}

void Chess::UpdateScore(const Bot &p) const noexcept {
	const unsigned short &dx = p==white ? white.GetName().length() + 2 : (BOX_WIDTH+1)*BOARD_SIZE - 5;
	MoveCursorToXY(RIGHT+dx, DOWN + 3*BOARD_SIZE + 2);
	std::cout << std::string(std::to_string(p.GetScore()).length(), ' ');
	MoveCursorToXY(RIGHT+dx, DOWN + 3*BOARD_SIZE + 2);
	std::cout << p.GetScore();
}

float Chess::EvaluateMove(const short &x, const short &y) const noexcept {
	if(board[y][x] == EMPTY)
		return 0;
	static float PIECE_POS_POINTS[6][BOARD_SIZE][BOARD_SIZE] =
	{{{-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},		// king
	{-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
	{-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
	{-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
	{-2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0},
	{-1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0},
	{2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0},
	{2.0, 3.0, 1.0, 0.0, 0.0, 1.0, 3.0, 2.0}}
	,
	{{-2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0},		// queen
	{-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0},
	{-1.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -1.0},
	{-0.5, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -0.5},
	{0.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -0.5},
	{-1.0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0, -1.0},
	{-1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, -1.0},
	{-2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0}}
	,
	{{-2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0},		// bishop
	{-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0},
	{-1.0, 0.0, 0.5, 1.0, 1.0, 0.5, 0.0, -1.0},
	{-1.0, 0.5, 0.5, 1.0, 1.0, 0.5, 0.5, -1.0},
	{-1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, -1.0},
	{-1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0},
	{-1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, -1.0},
	{-2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0}}
	,
	{{-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0},		// knight
	{-4.0, -2.0, 0.0, 0.0, 0.0, 0.0, -2.0, -4.0},
	{-3.0, 0.0, 1.0, 1.5, 1.5, 1.0, 0.0, -3.0},
	{-3.0, 0.5, 1.5, 2.0, 2.0, 1.5, 0.5, -3.0},
	{-3.0, 0.0, 1.5, 2.0, 2.0, 1.5, 0.0, -3.0},
	{-3.0, 0.5, 1.0, 1.5, 1.5, 1.0, 0.5, -3.0},
	{-4.0, -2.0, 0.0, 0.5, 0.5, 0.0, -2.0, -4.0},
	{-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0}}
	,
	{{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},				// rook
	{0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.5},
	{-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
	{-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
	{-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
	{-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
	{-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
	{0.0, 0.0, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0}}
	,
	{{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},				// pawn
	{5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0},
	{1.0, 1.0, 2.0, 3.0, 3.0, 2.0, 1.0, 1.0},
	{0.5, 0.5, 1.0, 2.5, 2.5, 1.0, 0.5, 0.5},
	{0.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 0.0},
	{0.5, -0.5, -1.0, 0.0, 0.0, -1.0, -0.5, 0.5},
	{0.5, 1.0, 1.0, -2.0, -2.0, 1.0, 1.0, 0.5},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}}};				// returns negative points if the pieces are black, positive points if the pieces are white
	return (board[y][x]<0 ? -1 : 1) * (EvaluatePiece(board[y][x]) + PIECE_POS_POINTS[board[y][x] + 7*(board[y][x]<0) - 1][board[y][x]<0 ? BOARD_SIZE-y-1 : y][x]);
}

float Chess::EvaluateBoard(const bool &turn) const noexcept {
	float total_evaluation = 0.0;
	for(short y=0;y<BOARD_SIZE;++y)
		for(short x=0;x<BOARD_SIZE;++x)
			total_evaluation += EvaluateMove(x, y);
	return (turn ? 1 : -1) * total_evaluation;
}

void Chess::PrintBoard() const noexcept {
	std::cout << TO_DOWN << TO_RIGHT;
	for(short y=0;y<BOARD_SIZE;++y) {
		PrintSeparator(' ');
		std::cout << "\b\b\b" << BOARD_SIZE-y << "  ";
		for(short x=0;x<BOARD_SIZE;++x) {
			const unsigned short &diff = BOX_WIDTH - PieceNameToString(board[y][x]).length();
			std::cout << std::string(diff/2, ' ') << PieceNameToString(board[y][x]) << std::string(diff/2, ' ');
			if(diff%2)				std::cout << " ";
			if(x < BOARD_SIZE-1)	std::cout << "|";
		}
		if(y < BOARD_SIZE-1) {
			std::cout << std::endl << TO_RIGHT;
			PrintSeparator('_');
		}
	}
	std::cout << std::endl << TO_RIGHT;
	PrintSeparator(' ');
	for(char ch='a';ch<'a'+BOARD_SIZE;++ch)
		std::cout << std::string(BOX_WIDTH/2, ' ') << ch << std::string(BOX_WIDTH/2, ' ');
	std::cout << std::endl << std::endl << TO_RIGHT << white.GetName() << ": 0";
	std::cout << std::string((BOX_WIDTH+1)*BOARD_SIZE - white.GetName().length() - black.GetName().length() - 10, ' ') << black.GetName() << ": 0";
	std::cout << std::endl << std::endl << TO_RIGHT << white.GetName() << "'s turn...";
	std::cout << std::endl << TO_RIGHT << "Enter move coordinates (x1,y1)->(x2,y2):";
	std::cout << std::endl << std::endl << TO_RIGHT << "All possible moves:" << std::endl;
}

void Chess::PrintAllMovesMadeInOrder() const noexcept {
	std::cout << std::endl << std::endl << TO_RIGHT << "All moves made in order:" << std::endl;
	bool turn = true;
	for(const auto &game_move : all_game_moves) {
		std::cout << std::endl << TO_RIGHT << (turn ? white : black).GetName() << ": ";
		switch(game_move.first) {
			case CASTLING:
				std::cout << "castling " << (game_move.second[0] == 2 ? "long" : "short");	break;
			default:
				std::cout << ToLowerString(PieceNameToString(game_move.second[4])).substr(2) << " '" << game_move.second.substr(0, 2) << "' to ";
				if(game_move.second[5] != EMPTY)
					std::cout << ToLowerString(PieceNameToString(game_move.second[5])).substr(2) + " ";
				std::cout << "'" << game_move.second.substr(2, 2) << "'";
				switch(game_move.first) {
					case PROMOTION:
						std::cout << " promoted to " << ToLowerString(PieceNameToString(game_move.second[6])).substr(2);
						break;
					case EN_PASSANT:
						std::cout << " (en passant)";
					default:
						break;
				}
		}
		turn = !turn;
	}
}

bool Chess::CheckEndgame(const unsigned short &n) noexcept {
	if(AllMoves().empty()) {			// if the opponent has no moves left, then it is checkmate
		GetOtherPlayer().IncreaseScore(EvaluatePiece(W_KING));
		UpdateScore(GetOtherPlayerConst());
		return EndGameText(n, CHECKMATE);
	}
	else if(all_game_moves.back().first != CASTLING) {
		if(all_game_moves.back().second[4] == W_PAWN - 7*whites_turn)
			moves_after_last_pawn_move_or_capture = 0;		// pawn has moved
		else if(all_game_moves.back().second[5] != EMPTY)
			moves_after_last_pawn_move_or_capture = 0;		// capture has occured
		else if((++moves_after_last_pawn_move_or_capture) == 50)
			return EndGameText(n, FIFTY_MOVES);
	}
	else if((++moves_after_last_pawn_move_or_capture) == 50)
		return EndGameText(n, FIFTY_MOVES);
	if(ThreefoldRepetition())
		return EndGameText(n, THREEFOLD_REP);
	return false;
}

bool Chess::PlayersTurn() noexcept {
	auto all_moves = AllMoves();
	all_moves.sort();
	unsigned short i=0;
	for(const auto &move : all_moves) {
		if(!((i++)%MOVES_PER_LINE))	std::cout << std::endl;
		std::cout << TO_RIGHT << move.substr(0, 2) << " " << move.substr(2);
	}
	if(IsCheck(whites_turn)) {
		std::cout << std::endl << std::endl << TO_RIGHT << "Check!";
		i += 2*MOVES_PER_LINE;
	}
	MoveCursorToXY(RIGHT+41, DOWN + 3*BOARD_SIZE + 5);
	while(true) {
		std::string from, to;
		std::cin >> from;
		if(!ToLowerString(from).compare("quit"))
			return EndGameText(i/MOVES_PER_LINE + 1, QUIT);
		if(!ToLowerString(from).compare("exit"))
			return EndGameText(i/MOVES_PER_LINE + 1, QUIT);
		std::cin >> to;
		from.resize(2);
		to.resize(2);
		from.shrink_to_fit();
		to.shrink_to_fit();
		from[0] = tolower(from[0]), to[0] = tolower(to[0]);
		ChangeToRealCoordinates(from[0], from[1], to[0], to[1]);
		if((from[0]!=to[0] || from[1]!=to[1]) && WithinBounds(from[0]) && WithinBounds(from[1]) && WithinBounds(to[0]) && WithinBounds(to[1]))
			if(CanMovePiece(from[0], from[1], to[0], to[1], all_moves)) {
				MovePiece(from[0], from[1], to[0], to[1], true, true);
				if(CheckEndgame(i/MOVES_PER_LINE + 1))
					return false;
				break;
			}
		MoveCursorToXY(RIGHT+41, DOWN + 3*BOARD_SIZE + 5);
		std::cout << CLEAR_LINE << std::endl << CLEAR_LINE;
		MoveCursorToXY(RIGHT+41, DOWN + 3*BOARD_SIZE + 5);
	}
	MoveCursorToXY(RIGHT, DOWN + 3*BOARD_SIZE + 4);
	std::cout << GetCurrentPlayerConst().GetName() << "'s turn..." << CLEAR_LINE;
	MoveCursorToXY(RIGHT+41, DOWN + 3*BOARD_SIZE + 5);
	std::cout << CLEAR_LINE << std::endl << CLEAR_LINE;
	ClearAllMoves(i/MOVES_PER_LINE + 1);
	MoveCursorToXY(0, DOWN + 3*BOARD_SIZE + 8);
	return true;
}

bool Chess::BotsTurn() noexcept {
	const auto &move = (whites_turn ? WHITE_BOT_RANDOM : BLACK_BOT_RANDOM) ? GetRandomMove() : GetCurrentPlayer().GetIdealMove(*this);
	MovePiece(move[0], move[1], move[2], move[3], false, true);
	if(CheckEndgame())
		return false;
	MoveCursorToXY(RIGHT, DOWN + 3*BOARD_SIZE + 4);
	std::cout << GetCurrentPlayerConst().GetName() << "'s turn..." << CLEAR_LINE;
	return true;
}

bool Chess::GameOver() noexcept {
	std::cout << std::endl << std::endl << std::endl << TO_RIGHT << "Press R to play again.";
	std::cout << std::endl << TO_RIGHT << "Press any other key to quit.";
	PrintAllMovesMadeInOrder();
	char key = getch();
	switch(key = tolower(key)) {
		case 'r':
			Reset();
			return true;
		default:
			return false;
	}
}
