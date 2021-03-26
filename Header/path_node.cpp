#include "chess.h"

// path node class implementation

// creates a subtree which contains all possible moves the player can make
void PathNode::CreateSubtree(Chess &c) noexcept {
	auto all_moves = c.AllMoves();					// add all possible moves to the Node's map
	for(auto &move : all_moves) {
		Chess::ChangeToRealCoordinates(move[0], move[1], move[2], move[3]);
		child_node_list.emplace(move, PathNode());
	}
}

// returns the maximum score for the minimizing player and the minimum score for the maximizing player in each recursion
float PathNode::AlphaBeta(Chess &c, unsigned short &depth, float alpha, float beta, const bool &maximizing_player, const bool &initial_turn) noexcept {
	if(!depth)
		return c.EvaluateBoard(initial_turn);
	CreateSubtree(c);
	float points = maximizing_player ? -9999 : 9999;
	for(auto &node : child_node_list) {						// 1-0: from, 3-2: to
		if(c.GetPiece(node.first[2], node.first[3]) == W_KING - 7*c.GetTurn()) {
			child_node_list.clear();
			return maximizing_player ? 9999 : -9999;		// if my target piece is king, then it is the most rewarding move
		}
		c.MovePiece(node.first[0], node.first[1], node.first[2], node.first[3], false, false);
		points = maximizing_player ? std::max(points, node.second.AlphaBeta(c, --depth, alpha, beta, false, initial_turn))		// recursive alpha-beta algorithm
		: std::min(points, node.second.AlphaBeta(c, --depth, alpha, beta, true, initial_turn));
		maximizing_player ? alpha = std::max(alpha, points) : beta = std::min(beta, points);
		++depth;
		c.MovePieceBack(node.first[0], node.first[1], node.first[2], node.first[3]);
		if(alpha >= beta)
			break;
	}
	child_node_list.clear();
	return points;
}

// the main function which returns the optimal move calculated by the alpha-beta algorithm
std::string PathNode::AlphaBetaRoot(Chess &c, unsigned short &difficulty) noexcept {
	CreateSubtree(c);
	std::vector<std::string> ideal_moves;
	float max_move_score = -9999;
	for(auto &node : child_node_list) {
		if(c.GetPiece(node.first[2], node.first[3]) == W_KING - 7*c.GetTurn()) {
			child_node_list.clear();
			return node.first;						// if my target piece is king, then this move is a game winner so return this move
		}
		c.MovePiece(node.first[0], node.first[1], node.first[2], node.first[3], false, false);
		float move_score = node.second.AlphaBeta(c, difficulty, -10000, 10000, false, !c.GetTurn());
		if(move_score > max_move_score) {
			max_move_score = move_score;
			ideal_moves.clear();
			ideal_moves.emplace_back(node.first);	// add the most rewarding move to the list
		}
		else if(move_score == max_move_score)
			ideal_moves.emplace_back(node.first);
		c.MovePieceBack(node.first[0], node.first[1], node.first[2], node.first[3]);
	}
	child_node_list.clear();						// delete the whole tree
	auto move = ideal_moves.cbegin();
	advance(move, GetRandomNumber<unsigned short>(0, ideal_moves.size()-1));
	return *move;
}
