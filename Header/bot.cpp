#include "chess.h"

// bot class implementation

// bot class constructor
Bot::Bot(const std::string &name, const unsigned short &difficulty) noexcept : Player(name), difficulty(difficulty) {	}

// getter method, returns the difficulty level of the bot
unsigned short Bot::GetDifficulty() const noexcept {
	return difficulty;
}

// returns the optimal move the bot can make with the default difficulty level (according to alpha-beta pruning at least)
std::string Bot::GetIdealMove(Chess &c) noexcept {
	return root.AlphaBetaRoot(c, difficulty);
}

// function overload, returns the optimal move the bot can make with the given difficulty level
std::string Bot::GetIdealMove(Chess &c, unsigned short difficulty) noexcept {
	return root.AlphaBetaRoot(c, difficulty);
}

// operator overload for '=='
bool Bot::operator== (const Bot &b) const noexcept {
	return !name.compare(b.name);	
}
