#include "chess.h"

Bot::Bot(const std::string &name, const unsigned short &difficulty) noexcept : Player(name), difficulty(difficulty) {	}

unsigned short Bot::GetDifficulty() const noexcept {	return difficulty;	}

std::string Bot::GetIdealMove(Chess &c) noexcept {	return root.AlphaBetaRoot(c, difficulty);	}

std::string Bot::GetIdealMove(Chess &c, unsigned short difficulty) noexcept {	return root.AlphaBetaRoot(c, difficulty);	}

bool Bot::operator== (const Bot &b) const noexcept {	return !name.compare(b.name);	}
