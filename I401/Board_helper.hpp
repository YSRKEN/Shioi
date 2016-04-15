#pragma once
#include <utility>
#include <array>
#include "types.hpp"
#include "misc_functions.hpp"
#include "constant.hpp"
using std::array;
namespace Board_helper {
	constexpr std::pair<array<size_t, 2>, size_t> ToPositionByDirection(const Direction d, const size_t i, const size_t j) {
		//C++11 constexprではswitch文が使えないので致し方なし
		return (d < DiagR)
			? (d == Row)
				? std::pair<array<size_t, 2>, size_t>{ { { ToPosition(j, i), 0 }}, 1U }// Row(─)
				: std::pair<array<size_t, 2>, size_t>{ { { ToPosition(i, j), 0 } }, 1U }// Column(│)
			: (d == DiagR)
				? std::pair<array<size_t, 2>, size_t>{ { { ToPosition(i - j, j), ToPosition(kBoardSize - 1 - j, i + j)}}, 2U }// Diagonally right(／)
				: (d == DiagL) 
					? std::pair<array<size_t, 2>, size_t>{ { { ToPosition(i + j, j), ToPosition(j, i + j) } }, 2U }// Diagonally left(＼)
					: std::pair<array<size_t, 2>, size_t>{};
	}
	constexpr std::pair<array<size_t, 2>, size_t> CalcOuterLoopMaxByDirection(const Direction d) {
		//C++11 constexprではswitch文が使えないので致し方なし
		return (d < DiagR)
			? std::pair<array<size_t, 2>, size_t>{ { { kBoardSize, 0 }}, 1U }// Row(─) Column(│)
			: (d == DiagR)
				? std::pair<array<size_t, 2>, size_t>{ { { kBoardSize, kBoardSize - 4}}, 2U }// Diagonally right(／)
				: (d == DiagL) 
					? std::pair<array<size_t, 2>, size_t>{ { { kBoardSize - 4, kBoardSize - 4 } }, 2U }// Diagonally left(＼)
					: std::pair<array<size_t, 2>, size_t>{};
	}
	constexpr std::pair<array<size_t, 2>, size_t> CalcInnerLoopMaxByDirection(const Direction d, const size_t i) {
		//C++11 constexprではswitch文が使えないので致し方なし
		return (d < DiagR)
			? std::pair<array<size_t, 2>, size_t>{ { { kBoardSize, 0 }}, 1U }// Row(─) Column(│)
			: (d == DiagR)
				? std::pair<array<size_t, 2>, size_t>{ { { i + 1, kBoardSize}}, 2U }// Diagonally right(／)
				: (d == DiagL) 
					? std::pair<array<size_t, 2>, size_t>{ { { kBoardSize, kBoardSize } }, 2U }// Diagonally left(＼)
					: std::pair<array<size_t, 2>, size_t>{};
	}
	// Check Cho-ren
	constexpr bool IsChorenB(const Pattern &pattern) {
		return (
			(pattern[Side::Right][4] == PackPattern_n(Stone::Black, 5))
			|| ((pattern[Side::Right][3] == PackPattern_n(Stone::Black, 4)) && (pattern[Side::Left][0] == Stone::Black))
			|| ((pattern[Side::Right][2] == PackPattern_n(Stone::Black, 3)) && (pattern[Side::Left][1] == PackPattern_n(Stone::Black, 2)))
			|| ((pattern[Side::Left][4] == PackPattern_n(Stone::Black, 5)))
			|| ((pattern[Side::Left][3] == PackPattern_n(Stone::Black, 4)) && (pattern[Side::Right][0] == Stone::Black))
			|| ((pattern[Side::Left][2] == PackPattern_n(Stone::Black, 3)) && (pattern[Side::Right][1] == PackPattern_n(Stone::Black, 2)))
		);
	}
	constexpr bool IsChorenW(const Pattern & /*pattern*/) {
		return false;
	}
}
