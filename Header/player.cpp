#include "chess.h"

// player class implementation

// player class constructor
Player::Player(const std::string &name) noexcept : name(name) {	}

// getter method which returns the name of the player
std::string Player::GetName() const noexcept {
	return name;
}

// getter method which returns the score of the player
unsigned short Player::GetScore() const noexcept {
	return score;
}

// getter method which returns whether the player can castle or not
bool Player::GetCastling() const noexcept {
	return castling;
}

// setter method which sets whether the player can castle or not
void Player::SetCastling(const bool &castling) noexcept {
	this->castling = castling;
}

// increases the score by an amount of "inc" points
void Player::IncreaseScore(const unsigned short &inc) noexcept {
	score += inc;
}

// resets the score and the ability to castle to their initial values
void Player::Reset() noexcept {
	score = 0;
	castling = true;
}

// operator overload for '=='
bool Player::operator== (const Player &p) const noexcept {
	return !name.compare(p.name);
}
