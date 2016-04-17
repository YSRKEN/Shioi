#pragma once
#include <utility>
#include <array>
#include "types.hpp"
#include "misc_functions.hpp"
#include "constant.hpp"
#include "PackedStone.hpp"
using std::array;
namespace Board_helper {
	constexpr std::pair<array<size_t, 2>, size_t> ToPositionByDirection(const Direction d, const size_t i, const size_t j) {
		//C++11 constexprではswitch文が使えないので致し方なし
		return (d < DiagR)
			? (d == Row)
				? std::pair<array<size_t, 2>, size_t>{ { { ToPosition(j, i), 0 }}, 1U }// Row(─)
				: std::pair<array<size_t, 2>, size_t>{ { { ToPosition(i, j), 0 } }, 1U }// Column(│)
			: (d == DiagR)
				? std::pair<array<size_t, 2>, size_t>{ { { ToPosition(i - j + 4, j), ToPosition(kBoardSize - 1 - j, i + j + 1)}}, 2U }// Diagonally right(／)
				: (d == DiagL) 
					? std::pair<array<size_t, 2>, size_t>{ { { ToPosition(i + j, j), ToPosition(j, i + j + 1) } }, 2U }// Diagonally left(＼)
					: std::pair<array<size_t, 2>, size_t>{};
	}
	constexpr std::pair<array<size_t, 2>, size_t> CalcOuterLoopMaxByDirection(const Direction d) {
		//C++11 constexprではswitch文が使えないので致し方なし
		return (d < DiagR)
			? std::pair<array<size_t, 2>, size_t>{ { { kBoardSize, 0 }}, 1U }// Row(─) Column(│)
			: (d == DiagR)
				? std::pair<array<size_t, 2>, size_t>{ { { kBoardSize - 4, kBoardSize - 5}}, 2U }// Diagonally right(／)
				: (d == DiagL) 
					? std::pair<array<size_t, 2>, size_t>{ { { kBoardSize - 4, kBoardSize - 5}}, 2U }// Diagonally left(＼)
					: std::pair<array<size_t, 2>, size_t>{};
	}
	constexpr std::pair<array<size_t, 2>, size_t> CalcInnerLoopMaxByDirection(const Direction d, const size_t i) {
		//C++11 constexprではswitch文が使えないので致し方なし
		return (d < DiagR)
			? std::pair<array<size_t, 2>, size_t>{ { { kBoardSize, 0 }}, 1U }// Row(─) Column(│)
			: (d == DiagR)
				? std::pair<array<size_t, 2>, size_t>{ { { i + 5, kBoardSize - i - 1}}, 2U }// Diagonally right(／)
				: (d == DiagL) 
					? std::pair<array<size_t, 2>, size_t>{ { { kBoardSize - i, kBoardSize - i - 1} }, 2U }// Diagonally left(＼)
					: std::pair<array<size_t, 2>, size_t>{};
	}
	// Check Cho-ren
	constexpr bool IsChorenB(const Pattern &pattern) {
		return (
			    (pattern[Side::Right][4] == Stone::Black**5_pack)
			|| ((pattern[Side::Right][3] == Stone::Black**4_pack) && (pattern[Side::Left][0]  == Stone::Black))
			|| ((pattern[Side::Right][2] == Stone::Black**3_pack) && (pattern[Side::Left][1]  == Stone::Black**2_pack))
			||  (pattern[Side::Left][4]  == Stone::Black**5_pack)
			|| ((pattern[Side::Left][3]  == Stone::Black**4_pack) && (pattern[Side::Right][0] == Stone::Black))
			|| ((pattern[Side::Left][2]  == Stone::Black**3_pack) && (pattern[Side::Right][1] == Stone::Black**2_pack))
		);
	}
	constexpr bool IsChorenW(const Pattern & /*pattern*/) {
		return false;
	}
}
