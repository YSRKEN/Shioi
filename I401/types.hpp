#pragma once
#pragma once
#include <cstdint>
#include <tuple>
#include "constant.hpp"
using std::uint8_t;
enum Stone : uint8_t {
	None,
	Black,
	White
};
enum Direction : uint8_t {
	// Row(��) R[1, 0] L[-1, 0]
	Row,
	// Column(��) R[0, 1] L[0, -1]
	Column,
	// Diagonally right(�^) R[-1, 1] L[1, -1]
	DiagR,
	// Diagonally left(�_) R[1, 1] L[-1, -1]
	DiagL,
	Directions
};
enum Side : uint8_t {
	Right,
	Left,
	Sides
};
using std::array;
typedef std::pair<size_t, int> Score;
typedef std::pair<size_t, bool> Result;
typedef array<array<size_t, 5>, 2> Pattern;
typedef std::tuple<size_t, size_t, size_t> RenCount;
typedef array<array<array<size_t, Side::Sides>, Direction::Directions>, kBoardSize * kBoardSize> IterateTable;
typedef array<Stone, kBoardSize * kBoardSize> BaseBoard;
