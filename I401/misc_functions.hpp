#pragma once
#pragma once
#include "types.hpp"
#include "constant.hpp"
#include <array>
#include <string>
using std::array;
using std::size_t;
std::string PositionToString(const size_t p) {
	return std::string(1, kPositionStringX[p % kBoardSize]) + std::string(1, kPositionStringY[p / kBoardSize]);
}
constexpr size_t ToPosition(const size_t x, const size_t y) {
	return x + y * kBoardSize;
}
constexpr Stone EnemyTurn(const Stone turn) {
	return (turn == Stone::Black ? Stone::White : Stone::Black);
}
