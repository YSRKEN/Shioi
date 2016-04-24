﻿#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <cassert>
#include "types.hpp"
#include "Board_helper.hpp"
#include "BookDB.hpp"
#include "constant_range_loop.hpp"
#include "PackedStone.hpp"
// using declaration
using std::cout;
using std::endl;
using std::string;
using std::tuple;
using std::vector;
using std::size_t;

size_t node = 0;

// definition in global area
std::random_device rd;
std::mt19937 mt(rd());

// misc function
inline size_t RandInt(const size_t n) {
	return std::uniform_int_distribution<size_t>{0, n - 1}(mt);
}
BookDB book;

// Board class
class Board {
	// values
	BaseBoard board_;
	Stone turn_;
	IterateTable kIterateTable;
	array<size_t, kBoardSize * kBoardSize> kTenGenDist;
	// Put board
	void PutBoard() {
		const static string kStoneString2[] = { "┼", "●", "○" };
		for (size_t y : rep(kBoardSize)) {
			for (size_t x : rep(kBoardSize)) {
				size_t p = x + y * kBoardSize;
				cout << kStoneString2[board_[p]];
			}
			cout << endl;
		}
	}
	bool IsGameEndImpl(const Stone turn, Direction d) {
		const auto outer_loop_max = Board_helper::CalcOuterLoopMaxByDirection(d);
		for (size_t c : rep(outer_loop_max.second)) {
			for (size_t i : rep(outer_loop_max.first[c])) {
				size_t len = 0;
				const auto inner_loop_max = Board_helper::CalcInnerLoopMaxByDirection(d, i);
				assert(inner_loop_max.second == outer_loop_max.second);
				for (size_t j : rep(inner_loop_max.first[c])) {
					const auto ps = Board_helper::ToPositionByDirection(d, i, j);
					assert(ps.second == outer_loop_max.second);
					const size_t p = ps.first[c];
					if (board_[p] == turn) {
						++len;
						if (len == 5) return true;
					}
					else {
						len = 0;
					}
				}
			}
		}
		return false;
	}
	// Check GameEnd
	bool IsGameEnd(const Stone turn) {
		for (Direction d = Direction::Row; d < Direction::Directions; d = static_cast<Direction>(static_cast<uint8_t>(d) + 1)) {
			if (IsGameEndImpl(turn, d)) return true;
		}
		return false;
	}
	bool IsGameEnd() {
		if (IsGameEnd(Stone::Black)) return true;
		if (IsGameEnd(Stone::White)) return true;
		return false;
	}
	// Count stone
	size_t CountStone() {
		size_t sum = 0;
		for (size_t p : rep(kBoardSize * kBoardSize)) {
			if (board_[p] != Stone::None) ++sum;
		}
		return sum;
	}
	// Get Opening move
	Result GetOpeningMove(const Stone /*turn*/) {
		size_t count = CountStone();
		if (count == 0) {
			return{ ToPosition(7, 7), true };
		}
		else if (board_[ToPosition(7, 7)] != Stone::Black) return{};
		else if (count == 1) {
			constexpr static size_t next_move[] = { ToPosition(7, 6), ToPosition(8, 6) };
			return{ next_move[RandInt(2)], true };
		}
		else if (count == 2) {
			// Ka-getsu
			if (board_[ToPosition(7, 6)] == Stone::White) return{ToPosition(8, 6), true};
			if (board_[ToPosition(8, 7)] == Stone::White) return{ToPosition(8, 8), true};
			if (board_[ToPosition(7, 8)] == Stone::White) return{ToPosition(6, 8), true};
			if (board_[ToPosition(6, 7)] == Stone::White) return{ToPosition(6, 6), true};
			// Ho-getsu
			if (board_[ToPosition(8, 6)] == Stone::White) return{ToPosition(8, 8), true};
			if (board_[ToPosition(8, 8)] == Stone::White) return{ToPosition(6, 8), true};
			if (board_[ToPosition(6, 8)] == Stone::White) return{ToPosition(6, 6), true};
			if (board_[ToPosition(6, 6)] == Stone::White) return{ToPosition(8, 6), true};
		}
		return{};
	}
	// Find Go-ren move
	Result FindGorenMove(const Stone turn) {
		// Row(─)
		for (size_t i : rep(kBoardSize)) {
			for (size_t j : rep(kBoardSize)) {
				size_t p = ToPosition(j, i);
				if (board_[p] != Stone::None) continue;
				size_t rlengh = 0;
				for (int k = j - 1; k >= 0; --k) {
					if (board_[ToPosition(k, i)] != turn) break;
					++rlengh;
				}
				size_t llengh = 0;
				for (size_t k : rep(j + 1, kBoardSize)) {
					if (board_[ToPosition(k, i)] != turn) break;
					++llengh;
				}
				if (turn == Stone::Black) {
					if (rlengh + llengh == 4) return Result(p, true);
				}
				else {
					if (rlengh + llengh >= 4) return Result(p, true);
				}
			}
		}
		// Column(│)
		for (size_t i : rep(kBoardSize)) {
			for (size_t j : rep(kBoardSize)) {
				size_t p = ToPosition(i, j);
				if (board_[p] != Stone::None) continue;
				size_t rlengh = 0;
				for (int k = j - 1; k >= 0; --k) {
					if (board_[ToPosition(i, k)] != turn) break;
					++rlengh;
				}
				size_t llengh = 0;
				for (size_t k : rep(j + 1, kBoardSize)) {
					if (board_[ToPosition(i, k)] != turn) break;
					++llengh;
				}
				if (turn == Stone::Black) {
					if (rlengh + llengh == 4) return Result(p, true);
				}
				else {
					if (rlengh + llengh >= 4) return Result(p, true);
				}
			}
		}
		// Diagonally right(／)
		for (size_t i : rep(4_sz, kBoardSize)) {
			for (size_t j : rep(i)) {
				size_t p = ToPosition(i - j, j);
				if (board_[p] != Stone::None) continue;
				size_t rlengh = 0;
				for (int k = j - 1; k >= 0; --k) {
					if (board_[ToPosition(i - k, k)] != turn) break;
					++rlengh;
				}
				size_t llengh = 0;
				for (size_t k = j + 1; k <= i; ++k) {
					if (board_[ToPosition(i - k, k)] != turn) break;
					++llengh;
				}
				if (turn == Stone::Black) {
					if (rlengh + llengh == 4) return Result(p, true);
				}
				else {
					if (rlengh + llengh >= 4) return Result(p, true);
				}
			}
		}
		for (size_t i : rep(1_sz, kBoardSize - 4_sz)) {
			for (size_t j : rep(kBoardSize - i)) {
				size_t p = ToPosition(kBoardSize - 1 - j, i + j);
				if (board_[p] != Stone::None) continue;
				size_t rlengh = 0;
				for (int k = j - 1; k >= 0; --k) {
					if (board_[ToPosition(kBoardSize - 1 - k, i + k)] != turn) break;
					++rlengh;
				}
				size_t llengh = 0;
				for (size_t k : rep(j + 1, kBoardSize - i)) {
					if (board_[ToPosition(kBoardSize - 1 - k, i + k)] != turn) break;
					++llengh;
				}
				if (turn == Stone::Black) {
					if (rlengh + llengh == 4) return Result(p, true);
				}
				else {
					if (rlengh + llengh >= 4) return Result(p, true);
				}
			}
		}
		// Diagonally left(＼)
		for (size_t i : rep(kBoardSize - 4_sz)) {
			for (size_t j : rep(kBoardSize - i)) {
				size_t p = ToPosition(i + j, j);
				if (board_[p] != Stone::None) continue;
				size_t rlengh = 0;
				for (int k = j - 1; k >= 0; --k) {
					if (board_[ToPosition(i + k, k)] != turn) break;
					++rlengh;
				}
				size_t llengh = 0;
				for (size_t k : rep(j + 1, kBoardSize - i)) {
					if (board_[ToPosition(i + k, k)] != turn) break;
					++llengh;
				}
				if (turn == Stone::Black) {
					if (rlengh + llengh == 4) return Result(p, true);
				}
				else {
					if (rlengh + llengh >= 4) return Result(p, true);
				}
			}
		}
		for (size_t i : rep(1_sz, kBoardSize - 4_sz)) {
			for (size_t j : rep(kBoardSize - i)) {
				size_t p = ToPosition(j, i + j);
				if (board_[p] != Stone::None) continue;
				size_t rlengh = 0;
				for (int k = j - 1; k >= 0; --k) {
					if (board_[ToPosition(k, i + k)] != turn) break;
					++rlengh;
				}
				size_t llengh = 0;
				for (size_t k : rep(j + 1, kBoardSize - i)) {
					if (board_[ToPosition(k, i + k)] != turn) break;
					++llengh;
				}
				if (turn == Stone::Black) {
					if (rlengh + llengh == 4) return Result(p, true);
				}
				else {
					if (rlengh + llengh >= 4) return Result(p, true);
				}
			}
		}
		return Result(0, false);
	}
	// Get right and left pattern
	Pattern GetPatternB(const size_t position, const Direction dir) {
		const auto& pattern_length = kIterateTable[position][dir];
		Pattern pattern;
		for (auto s : { Side::Left, Side::Right }) {
			for (size_t i : rep(5)) {
				const int sign = (s == Side::Right) ? 1 : -1;
				pattern[s][i] = (pattern_length[s] <= i) 
					? ((i) ? pattern[s][i - 1U] | Stone::None : Stone::None)/* | Stone::None */
					: (this->board_ | PackPattern(position, dir, sign, sign * (i + 1)) | Normalize())/*.pop_back()*/;
			}
		}
		return pattern;
	}
	Pattern GetPatternW(const size_t position, const Direction dir) {
		const auto& pattern_length = kIterateTable[position][dir];
		Pattern pattern;
		for (auto s : { Side::Left, Side::Right }) {
			for (size_t i : rep(4)) {
				const int sign = (s == Side::Right) ? 1 : -1;
				pattern[s][i] = (pattern_length[s] <= i) 
					? ((i) ? pattern[s][i - 1U] | Stone::None : Stone::None)/* | Stone::None */
					: this->board_ | PackPattern(position, dir, sign, sign * (i + 1));
			}
		}
		return pattern;
	}
	// Matching Pattern
	bool MatchPatternB(const Pattern &pattern, const size_t /*position*/, const Direction /*dir*/, const Side side) const noexcept {
		return (pattern[side][0] == Stone::None);
	}
	bool MatchPatternB(const Pattern &pattern, const size_t position, const Direction dir, const array<PackedStone, 2>& s1) const noexcept {
		//auto hoge1 = (s1[Side::Left] | Stone::None);
		//auto hoge2 = (s1[Side::Right] | Stone::None);
		return (
			kIterateTable[position][dir][Side::Left] >= s1[Side::Left].size() && pattern[Side::Left][s1[Side::Left].size()] == (s1[Side::Left] | Stone::None)
			&& kIterateTable[position][dir][Side::Right] >= s1[Side::Right].size() && pattern[Side::Right][s1[Side::Right].size()] == (s1[Side::Right] | Stone::None)
		);
	}
	bool MatchPatternB(const Pattern &pattern, const size_t position, const Direction dir, const Side side, const array<PackedStone, 1>& s1) const noexcept {
		return (
			kIterateTable[position][dir][side] >= s1[0].size() && pattern[side][s1[0].size()] == (s1[0] | Stone::None)
			&& pattern[!side][0] == Stone::None
			);
	}
	bool MatchPatternW(const Pattern &pattern, const size_t position, const Direction dir, const array<PackedStone, 2>& s1) const noexcept {
		return (
			kIterateTable[position][dir][Side::Left] >= s1[Side::Left].size() && pattern[Side::Left][s1[Side::Left].size() - 1] == (s1[Side::Left])
			&& kIterateTable[position][dir][Side::Right] >= s1[Side::Right].size() && pattern[Side::Right][s1[Side::Right].size() - 1] == (s1[Side::Right])
			);
	}
	bool MatchPatternW(const Pattern &pattern, const size_t position, const Direction dir, const Side side, const PackedStone s1) const noexcept {
		return (kIterateTable[position][dir][side] >= s1.size() && pattern[side][s1.size() - 1] == s1);
	}
	// Count Ren
	RenCount CountRenB(const Pattern &pattern, const size_t position, const Direction dir) {
		/* Count "Straight Shi-Shi" */
		/* 1. BO|BBB|OB   "Ryoutou-no-ShiShi"
		 * 2. BBO|BB|OBB  "Chouda-no-ShiShi"
		 * 3. BBBO|B|OBBB "Souryu-no-ShiShi"
		 * 一直線上に出来る四々で、両頭の四々・長蛇の四々・双龍の四々
		 */
		namespace p = packed_stone;
		{
			//BO|BBB|OB
			if (MatchPatternB(pattern, position, dir, {{ p::nb, p::bbnb }})) return RenCount(0, 2, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bnb, p::bnb }})) return RenCount(0, 2, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bbnb, p::nb }})) return RenCount(0, 2, 0);
			//BBO|BB|OBB
			if (MatchPatternB(pattern, position, dir, {{ p::nbb, p::bnbb }})) return RenCount(0, 2, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bnbb, p::nbb }})) return RenCount(0, 2, 0);
			//BBBO|B|OBBB
			if (MatchPatternB(pattern, position, dir, {{ p::nbbb, p::nbbb }})) return RenCount(0, 2, 0);
		}
		// Count Shi-ren
		{
			if (MatchPatternB(pattern, position, dir, {{ p::n, p::bbbn }})) return RenCount(1, 0, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bn, p::bbn }})) return RenCount(1, 0, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bbbn, p::n }})) return RenCount(1, 0, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bbn, p::bn }})) return RenCount(1, 0, 0);
		}
		// Count Katsu-Shi
		{
			//11110
			if (MatchPatternB(pattern, position, dir, {{ p::bbb, p::n }})) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bb, p::bn }})) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::b, p::bbn }})) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Right, {{ p::bbbn }})) return RenCount(0, 1, 0);
			//11101
			if (MatchPatternB(pattern, position, dir, Side::Right, {{ p::nbbb }})) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bb, p::nb }})) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::b, p::bnb }})) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Right, {{ p::bbnb }})) return RenCount(0, 1, 0);
			//11011
			if (MatchPatternB(pattern, position, dir, Side::Right, {{ p::bnbb }})) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::nbb, p::b }})) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, {{ p::bnbb }})) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::b, p::nbb }})) return RenCount(0, 1, 0);
			//10111
			if (MatchPatternB(pattern, position, dir, Side::Left, {{ p::nbbb }})) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::nb, p::bb }})) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bnb, p::b }})) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, {{ p::bbnb }})) return RenCount(0, 1, 0);
			//01111
			if (MatchPatternB(pattern, position, dir, {{ p::n, p::bbb }})) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bn, p::bb }})) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bbn, p::b }})) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, {{ p::bbbn }})) return RenCount(0, 1, 0);
		}
		// Count Katsu-San
		{
			int offset1 = 0, offset2 = 0;
			while (true) {
				//011100
				if (MatchPatternB(pattern, position, dir, {{ p::bbn, p::nn }})) {
					offset1 = 1; offset2 = -3; break;
				}
				if (MatchPatternB(pattern, position, dir, {{ p::bn, p::bnn }})) {
					offset1 = 2; offset2 = -2; break;
				}
				if (MatchPatternB(pattern, position, dir, {{ p::n, p::bbnn }})) {
					offset1 = 3; offset2 = -1; break;
				}
				//011010
				if (MatchPatternB(pattern, position, dir, {{ p::nbbn, p::n }})) {
					offset1 = -1; break;
				}
				if (MatchPatternB(pattern, position, dir, {{ p::bn, p::nbn }})) {
					offset1 = 1; break;
				}
				if (MatchPatternB(pattern, position, dir, {{ p::n, p::bnbn }})) {
					offset1 = 2; break;
				}
				//010110
				if (MatchPatternB(pattern, position, dir, {{ p::n, p::nbbn }})) {
					offset1 = 1; break;
				}
				if (MatchPatternB(pattern, position, dir, {{ p::nbn, p::bn }})) {
					offset1 = -1; break;
				}
				if (MatchPatternB(pattern, position, dir, {{ p::bnbn, p::n }})) {
					offset1 = -2; break;
				}
				//001110
				if (MatchPatternB(pattern, position, dir, {{ p::nn, p::bbn }})) {
					offset1 = -1; offset2 = 3; break;
				}
				if (MatchPatternB(pattern, position, dir, {{ p::bnn, p::bn }})) {
					offset1 = -2; offset2 = 2; break;
				}
				if (MatchPatternB(pattern, position, dir, {{ p::bbnn, p::n }})) {
					offset1 = -3; offset2 = 1; break;
				}
				break;
			}
			auto GetPosition = [&position, &dir](int count) -> int {return position + kPositionoffset[dir] * count; };
			if (offset2 != 0) {
				int next_pos = GetPosition(offset2);
				if (next_pos >= 0 && static_cast<std::size_t>(next_pos) < kBoardSize * kBoardSize) {
					board_[position] = Stone::Black;
					auto valid_flg = IsValidMove(next_pos);
					board_[position] = Stone::None;
					if (valid_flg) return RenCount(0, 0, 1);
				}
			}
			if (offset1 != 0) {
				int next_pos = GetPosition(offset1);
				if (next_pos >= 0 && static_cast<std::size_t>(next_pos) < kBoardSize * kBoardSize) {
					board_[position] = Stone::Black;
					auto valid_flg = IsValidMove(next_pos);
					board_[position] = Stone::None;
					if (valid_flg) return RenCount(0, 0, 1);
				}
			}
		}
		return RenCount(0, 0, 0);
	}
	RenCount CountRenW(const Pattern &pattern, const size_t position, const Direction dir) {
		/* Count "Straight Shi-Shi" */
		/* 1. BO|BBB|OB   "Ryoutou-no-ShiShi"
		 * 2. BBO|BB|OBB  "Chouda-no-ShiShi"
		 * 3. BBBO|B|OBBB "Souryu-no-ShiShi"
		 * 一直線上に出来る四々で、両頭の四々・長蛇の四々・双龍の四々
		 */
		namespace p = packed_stone;
		{
			//BO|BBB|OB
			if (MatchPatternW(pattern, position, dir, {{ p::nw, p::wwnw }})) return RenCount(0, 2, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::wnw, p::wnw }})) return RenCount(0, 2, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::wwnw, p::nw }})) return RenCount(0, 2, 0);
			//BBO|BB|OBB
			if (MatchPatternW(pattern, position, dir, {{ p::nww, p::wnww }})) return RenCount(0, 2, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::wnww, p::nww }})) return RenCount(0, 2, 0);
			//BBBO|B|OBBB
			if (MatchPatternW(pattern, position, dir, {{ p::nwww, p::nwww }})) return RenCount(0, 2, 0);
		}
		// Count Shi-ren
		{
			if (MatchPatternW(pattern, position, dir, {{ p::n, p::wwwn }})) return RenCount(1, 0, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::wn, p::wwn }})) return RenCount(1, 0, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::wwwn, p::n }})) return RenCount(1, 0, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::wwn, p::wn }})) return RenCount(1, 0, 0);
		}
		// Count Katsu-Shi
		{
			//11110
			if (MatchPatternW(pattern, position, dir, {{ p::www, p::n }})) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::ww, p::wn }})) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::w, p::wwn }})) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Right, p::wwwn)) return RenCount(0, 1, 0);
			//11101
			if (MatchPatternW(pattern, position, dir, Side::Left, p::nwww)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::ww, p::nw }})) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::w, p::wnw }})) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Right, p::wwnw)) return RenCount(0, 1, 0);
			//11011
			if (MatchPatternW(pattern, position, dir, Side::Left, p::wnww)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::nww, p::w }})) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Right, p::wnww)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::w, p::nww }})) return RenCount(0, 1, 0);
			//10111
			if (MatchPatternW(pattern, position, dir, Side::Right, p::nwww)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::nw, p::ww }})) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::wnw, p::w }})) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, p::wwnw)) return RenCount(0, 1, 0);
			//01111
			if (MatchPatternW(pattern, position, dir, {{ p::n, p::www }})) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::wn, p::ww }})) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::wwn, p::w }})) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, p::wwwn)) return RenCount(0, 1, 0);
		}
		// Count Katsu-san
		{
			//011100
			if (MatchPatternW(pattern, position, dir, {{ p::wwn, p::nn }})) return RenCount(0, 0, 1);
			if (MatchPatternW(pattern, position, dir, {{ p::wn, p::wnn }})) return RenCount(0, 0, 1);
			if (MatchPatternW(pattern, position, dir, {{ p::n, p::wwnn }})) return RenCount(0, 0, 1);
			//011010
			if (MatchPatternW(pattern, position, dir, {{ p::nwwn, p::n }})) return RenCount(0, 0, 1);
			if (MatchPatternW(pattern, position, dir, {{ p::wn, p::nwn }})) return RenCount(0, 0, 1);
			if (MatchPatternW(pattern, position, dir, {{ p::n, p::wnwn }})) return RenCount(0, 0, 1);
			//010110
			if (MatchPatternW(pattern, position, dir, {{ p::n, p::nwwn }})) return RenCount(0, 0, 1);
			if (MatchPatternW(pattern, position, dir, {{ p::nwn, p::wn }})) return RenCount(0, 0, 1);
			if (MatchPatternW(pattern, position, dir, {{ p::wnwn, p::n }})) return RenCount(0, 0, 1);
			//001110
			if (MatchPatternW(pattern, position, dir, {{ p::nn, p::wwn }})) return RenCount(0, 0, 1);
			if (MatchPatternW(pattern, position, dir, {{ p::wnn, p::wn }})) return RenCount(0, 0, 1);
			if (MatchPatternW(pattern, position, dir, {{ p::wwnn, p::n }})) return RenCount(0, 0, 1);
		}
		return RenCount(0, 0, 0);
	}
	// Count Ren and Block-point
	RenCount CountRenB2(const Pattern &pattern, const size_t position, const Direction dir, size_t &block_position) {
		/* Count "Straight Shi-Shi"
		* 1. BO|BBB|OB   "Ryoutou-no-ShiShi"
		* 2. BBO|BB|OBB  "Chouda-no-ShiShi"
		* 3. BBBO|B|OBBB "Souryu-no-ShiShi"
		* 一直線上に出来る四々で、両頭の四々・長蛇の四々・双龍の四々
		*/
		namespace p = packed_stone;
		{
			//BO|BBB|OB
			if (MatchPatternB(pattern, position, dir, {{ p::nb, p::bbnb }})) return RenCount(0, 2, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bnb, p::bnb }})) return RenCount(0, 2, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bbnb, p::nb }})) return RenCount(0, 2, 0);
			//BBO|BB|OBB
			if (MatchPatternB(pattern, position, dir, {{ p::nbb, p::bnbb }})) return RenCount(0, 2, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bnbb, p::nbb }})) return RenCount(0, 2, 0);
			//BBBO|B|OBBB
			if (MatchPatternB(pattern, position, dir, {{ p::nbbb, p::nbbb }})) return RenCount(0, 2, 0);
		}
		// Count Shi-ren
		{
			if (MatchPatternB(pattern, position, dir, {{ p::n, p::bbbn }})) return RenCount(1, 0, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bn, p::bbn }})) return RenCount(1, 0, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bbbn, p::n }})) return RenCount(1, 0, 0);
			if (MatchPatternB(pattern, position, dir, {{ p::bbn, p::bn }})) return RenCount(1, 0, 0);
		}
		// Count Katsu-Shi
		auto GetPosition = [&position, &dir](int count) -> int {return position + kPositionoffset[dir] * count; };
		{
			//11110
			if (MatchPatternB(pattern, position, dir, {{ p::bbb, p::n }})) {
				block_position = GetPosition(1) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, {{ p::bb, p::bn }})) {
				block_position = GetPosition(2) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, {{ p::b, p::bbn }})) {
				block_position = GetPosition(3) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Right, {{ p::bbbn }})) {
				block_position = GetPosition(4) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			//11101
			if (MatchPatternB(pattern, position, dir, Side::Left, {{ p::nbbb }})) {
				block_position = GetPosition(-1) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, {{ p::bb, p::nb }})) {
				block_position = GetPosition(1) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, {{ p::b, p::bnb }})) {
				block_position = GetPosition(2) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Right, {{ p::bbnb }})) {
				block_position = GetPosition(3) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			//11011
			if (MatchPatternB(pattern, position, dir, Side::Right, {{ p::bnbb }})) {
				block_position = GetPosition(2) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, { { Stone::Black, p::nbb } })) {
				block_position = GetPosition(1) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, {{ p::nbb, p::b }})) {
				block_position = GetPosition(-1) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Left, {{ p::bnbb }})) {
				block_position = GetPosition(-2) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			//10111
			if (MatchPatternB(pattern, position, dir, Side::Right, {{ p::nbbb }})) {
				block_position = GetPosition(1) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, {{ p::nb, p::bb }})) {
				block_position = GetPosition(-1) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, {{ p::bnb, p::b }})) {
				block_position = GetPosition(-2) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Left, {{ p::bbnb }})) {
				block_position = GetPosition(-3) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			//01111
			if (MatchPatternB(pattern, position, dir, {{ p::n, p::bbb }})) {
				block_position = GetPosition(-1) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, {{ p::bn, p::bb }})) {
				block_position = GetPosition(-2) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, {{ p::bbn, p::b }})) {
				block_position = GetPosition(-3) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Left, {{ p::bbbn }})) {
				block_position = GetPosition(-4) | limit2(0_sz, kBoardSize * kBoardSize);
				return RenCount(0, 1, 0);
			}
		}
		// Count Katsu-San
		{
			int offset1 = 0, offset2 = 0;
			while (true) {
				//011100
				if (MatchPatternB(pattern, position, dir, {{ p::bbn, p::nn }})) {
					offset1 = 1; offset2 = -3; break;
				}
				if (MatchPatternB(pattern, position, dir, {{ p::bn, p::bnn }})) {
					offset1 = 2; offset2 = -2; break;
				}
				if (MatchPatternB(pattern, position, dir, {{ p::n, p::bbnn }})) {
					offset1 = 3; offset2 = -1; break;
				}
				//011010
				if (MatchPatternB(pattern, position, dir, {{ p::nbbn, p::n }})) {
					offset1 = -1; break;
				}
				if (MatchPatternB(pattern, position, dir, {{ p::bn, p::nbn }})) {
					offset1 = 1; break;
				}
				if (MatchPatternB(pattern, position, dir, {{ p::n, p::bnbn }})) {
					offset1 = 2; break;
				}
				//010110
				if (MatchPatternB(pattern, position, dir, {{ p::n, p::nbbn }})) {
					offset1 = 1; break;
				}
				if (MatchPatternB(pattern, position, dir, {{ p::nbn, p::bn }})) {
					offset1 = -1; break;
				}
				if (MatchPatternB(pattern, position, dir, {{ p::bnbn, p::n }})) {
					offset1 = -2; break;
				}
				//001110
				if (MatchPatternB(pattern, position, dir, {{ p::nn, p::bbn }})) {
					offset1 = -1; offset2 = 3; break;
				}
				if (MatchPatternB(pattern, position, dir, {{ p::bnn, p::bn }})) {
					offset1 = -2; offset2 = 2; break;
				}
				if (MatchPatternB(pattern, position, dir, {{ p::bbnn, p::n }})) {
					offset1 = -3; offset2 = 1; break;
				}
				break;
			}
			if (offset2 != 0) {
				int next_pos = GetPosition(offset2);
				if (next_pos >= 0 && static_cast<std::size_t>(next_pos) < kBoardSize * kBoardSize) {
					board_[position] = Stone::Black;
					auto valid_flg = IsValidMove(next_pos);
					board_[position] = Stone::None;
					if (valid_flg) return RenCount(0, 0, 1);
				}
			}
			if (offset1 != 0) {
				int next_pos = GetPosition(offset1);
				if (next_pos >= 0 && static_cast<std::size_t>(next_pos) < kBoardSize * kBoardSize) {
					board_[position] = Stone::Black;
					auto valid_flg = IsValidMove(next_pos);
					board_[position] = Stone::None;
					if (valid_flg) return RenCount(0, 0, 1);
				}
			}
		}
		return RenCount(0, 0, 0);
	}
	RenCount CountRenW2(const Pattern &pattern, const size_t position, const Direction dir) {
		/* Count "Straight Shi-Shi"
		* 1. BO|BBB|OB   "Ryoutou-no-ShiShi"
		* 2. BBO|BB|OBB  "Chouda-no-ShiShi"
		* 3. BBBO|B|OBBB "Souryu-no-ShiShi"
		* 一直線上に出来る四々で、両頭の四々・長蛇の四々・双龍の四々
		*/
		namespace p = packed_stone;
		{
			//BO|BBB|OB
			if (MatchPatternW(pattern, position, dir, {{ p::nw, p::wwnw }})) return RenCount(0, 2, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::wnw, p::wnw }})) return RenCount(0, 2, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::wwnw, p::nw }})) return RenCount(0, 2, 0);
			//BBO|BB|OBB
			if (MatchPatternW(pattern, position, dir, {{ p::nww, p::wnww }})) return RenCount(0, 2, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::wnww, p::nww }})) return RenCount(0, 2, 0);
			//BBBO|B|OBBB
			if (MatchPatternW(pattern, position, dir, {{ p::nwww, p::nwww }})) return RenCount(0, 2, 0);
		}
		// Count Shi-ren
		{
			if (MatchPatternW(pattern, position, dir, {{ p::n, p::wwwn }})) return RenCount(1, 0, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::wn, p::wwn }})) return RenCount(1, 0, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::wwwn, p::n }})) return RenCount(1, 0, 0);
			if (MatchPatternW(pattern, position, dir, {{ p::wwn, p::wn }})) return RenCount(1, 0, 0);
		}
		// Count Katsu-Shi
		{
			auto GetPosition = [&position, &dir](int count) -> int {return position + kPositionoffset[dir] * count; };
			//11110
			if (MatchPatternW(pattern, position, dir, {{ p::www, p::n }})) {
				return RenCount(0, 1, GetPosition(1) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, {{ p::ww, p::wn }})) {
				return RenCount(0, 1, GetPosition(2) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, {{ p::w, p::wwn }})) {
				return RenCount(0, 1, GetPosition(3) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Right, p::wwwn)) {
				return RenCount(0, 1, GetPosition(4) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			//11101
			if (MatchPatternW(pattern, position, dir, Side::Left, p::nwww)) {
				return RenCount(0, 1, GetPosition(-1) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, {{ p::ww, p::nw }})) {
				return RenCount(0, 1, GetPosition(1) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, {{ p::w, p::wnw }})) {
				return RenCount(0, 1, GetPosition(2) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Right, p::wwnw)) {
				return RenCount(0, 1, GetPosition(3) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			//11011
			if (MatchPatternW(pattern, position, dir, Side::Left, p::wnww)) {
				return RenCount(0, 1, GetPosition(-2) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, {{ p::nww, p::w }})) {
				return RenCount(0, 1, GetPosition(-1) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Right, p::wnww)) {
				return RenCount(0, 1, GetPosition(2) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, {{ p::w, p::nww }})) {
				return RenCount(0, 1, GetPosition(1) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			//10111
			if (MatchPatternW(pattern, position, dir, Side::Right, p::nwww)) {
				return RenCount(0, 1, GetPosition(1) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, {{ p::nw, p::ww }})) {
				return RenCount(0, 1, GetPosition(-1) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, {{ p::wnw, p::w }})) {
				return RenCount(0, 1, GetPosition(-2) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Left, p::wwnw)) {
				return RenCount(0, 1, GetPosition(-3) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			//01111
			if (MatchPatternW(pattern, position, dir, {{ p::n, p::www }})) {
				return RenCount(0, 1, GetPosition(-1) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, {{ p::wn, p::ww }})) {
				return RenCount(0, 1, GetPosition(-2) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, {{ p::wwn, p::w }})) {
				return RenCount(0, 1, GetPosition(-3) | limit2(0_sz, kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Left, p::wwwn)) {
				return RenCount(0, 1, GetPosition(-4) | limit2(0_sz, kBoardSize * kBoardSize));
			}
		}
		return RenCount(0, 0, 0);
	}
	// Judge valid move(Black-side only)
	bool IsValidMove(const size_t p) {
		size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
		for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
			auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
			if (Board_helper::IsChorenB(move_pattern)) return false;
			size_t s4s, s4n, s3;
			std::tie(s4s, s4n, s3) = CountRenB(move_pattern, p, static_cast<Direction>(dir));
			sum_4_strong += s4s;
			sum_4_normal += s4n;
			sum_3 += s3;
		}
		if (sum_4_strong + sum_4_normal >= 2 || sum_3 >= 2) return false;
		return true;
	}
	// Find Shi-Oi tsume
	bool IsShioiMove(const Stone turn, const size_t block_position, const size_t depth) {
		// Block Katsu-Shi
		if (block_position >= kBoardSize * kBoardSize) {
			return true;
		}
		auto enemy_stone = EnemyTurn(turn);
		if (enemy_stone == Stone::Black && !IsValidMove(block_position)) {
			//PutBoard();
			return true;
		}
		board_[block_position] = enemy_stone;
		// If you must block enemy's Shi-ren or Katsu-shi, this Shioi's challenge is failed.
		auto result = FindGorenMove(enemy_stone);
		if (result.second) {
			board_[block_position] = Stone::None;
			return false;
		}
		//depth limit
		if (depth == 0) {
			board_[block_position] = Stone::None;
			return false;
		}
		// Find Next Shi-ren or Katsu-shi
		auto range = GetRange();
		for (size_t y : rep(range[1], range[3])) {
			for (size_t x : rep(range[0], range[2])) {
				size_t p = ToPosition(x, y);
			if (board_[p] != Stone::None) continue;
			if (turn == Stone::Black) {
				bool cho_ren_flg = false;
				size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
				size_t block_position2 = kBoardSize * kBoardSize;
				for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
					auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
					if (Board_helper::IsChorenB(move_pattern)) {
						cho_ren_flg = true;
						break;
					}
					//auto ren_count = CountRenB(move_pattern, p, static_cast<Direction>(dir));
					size_t s4s, s4n, s3;
					std::tie(s4s, s4n, s3) = CountRenB2(move_pattern, p, static_cast<Direction>(dir), block_position2);
					sum_4_strong += s4s;
					sum_4_normal += s4n;
					sum_3 += s3;
				}
				if (cho_ren_flg || sum_4_strong + sum_4_normal >= 2 || sum_3 >= 2) continue;
				if (sum_4_strong > 0) {
					board_[block_position] = Stone::None;
					return true;
				}
				if (sum_4_normal == 1) {
					board_[p] = Stone::Black;
					auto score = IsShioiMove(Stone::Black, block_position2, depth - 1);
					board_[p] = Stone::None;
					if (score) {
						board_[block_position] = Stone::None;
						return true;
					}
					continue;
				}
			}
			else {
				size_t block_position2 = kBoardSize * kBoardSize;
				size_t sum_4_strong = 0, sum_4_normal = 0;
				for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
					auto move_pattern = GetPatternW(p, static_cast<Direction>(dir));
					if (Board_helper::IsChorenW(move_pattern)) return true;
					size_t s4s, s4n, blk;
					std::tie(s4s, s4n, blk) = CountRenW2(move_pattern, p, static_cast<Direction>(dir));
					sum_4_strong += s4s;
					sum_4_normal += s4n;
					if (s4n) block_position2 = blk;
				}
				if (sum_4_strong > 0 || sum_4_normal > 1) {
					board_[block_position] = Stone::None;
					return true;
				}
				if (sum_4_normal == 1) {
					board_[p] = Stone::White;
					auto score = IsShioiMove(Stone::White, block_position2, depth - 1);
					board_[p] = Stone::None;
					if (score) {
						board_[block_position] = Stone::None;
						return true;
					}
					continue;
				}
			}
		}
		}

		board_[block_position] = Stone::None;
		return false;
	}
	Result FindShioiMove(const Stone turn, const size_t depth) {
		auto range = GetRange();
		if (turn == Stone::Black) {
			for (size_t y : rep(range[1], range[3])) {
				for (size_t x : rep(range[0], range[2])){
					size_t p = ToPosition(x, y);
					if (board_[p] != Stone::None) continue;
					bool cho_ren_flg = false;
					size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
					size_t block_position = kBoardSize * kBoardSize;
					for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
						auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
						if (Board_helper::IsChorenB(move_pattern)) {
							cho_ren_flg = true;
							break;
						}
						size_t s4s, s4n, s3;
						std::tie(s4s, s4n, s3) = CountRenB2(move_pattern, p, static_cast<Direction>(dir), block_position);
						sum_4_strong += s4s;
						sum_4_normal += s4n;
						sum_3 += s3;
					}
					if (cho_ren_flg || sum_4_strong + sum_4_normal >= 2 || sum_3 >= 2) continue;
					if (sum_4_strong > 0) return Result(p, true);
					if (sum_4_normal == 1) {
						board_[p] = Stone::Black;
						auto score = IsShioiMove(Stone::Black, block_position, depth);
						board_[p] = Stone::None;
						if (score) return Result(p, true);
						continue;
					}
				}
			}
		}else{
			for (size_t y : rep(range[1], range[3])) {
				for (size_t x : rep(range[0], range[2])) {
					size_t p = ToPosition(x, y);
					if (board_[p] != Stone::None) continue;
					size_t block_position = kBoardSize * kBoardSize;
					size_t sum_4_strong = 0, sum_4_normal = 0;
					for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
						auto move_pattern = GetPatternW(p, static_cast<Direction>(dir));
						if (Board_helper::IsChorenW(move_pattern)) return Result(p, true);
						size_t s4s, s4n, blk;
						std::tie(s4s, s4n, blk) = CountRenW2(move_pattern, p, static_cast<Direction>(dir));
						sum_4_strong += s4s;
						sum_4_normal += s4n;
						if (s4n) block_position = blk;
					}
					if (sum_4_strong > 0) return Result(p, true);
					if (sum_4_normal > 1) return Result(p, true);
					if (sum_4_normal == 1) {
						board_[p] = Stone::White;
						auto score = IsShioiMove(Stone::White, block_position, depth);
						board_[p] = Stone::None;
						if (score) return Result(p, true);
						continue;
					}
				}
			}
		}
		return Result(0, false);
	}
	// Find Oi tsume
	bool IsOiteMove(const Stone turn, const size_t depth) {
		auto range = GetRange();
		if (turn == Stone::Black) {
			for (size_t y : rep(range[1], range[3])) {
				for (size_t x : rep(range[0], range[2])) {
					size_t p = ToPosition(x, y);
					if (board_[p] != Stone::None) continue;
					bool cho_ren_flg = false;
					size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
					size_t block_position = kBoardSize * kBoardSize;
					for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
						auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
						if (Board_helper::IsChorenB(move_pattern)) {
							cho_ren_flg = true;
							break;
						}
						size_t s4s, s4n, s3;
						std::tie(s4s, s4n, s3) = CountRenB2(move_pattern, p, static_cast<Direction>(dir), block_position);
						sum_4_strong += s4s;
						sum_4_normal += s4n;
						sum_3 += s3;
					}
					if (cho_ren_flg || sum_4_strong + sum_4_normal >= 2 || sum_3 >= 2) continue;
					// Shi-ren
					if (sum_4_strong > 0) return true;
					// Other Oite
					if (depth > 0) {
						board_[p] = Stone::Black;
						auto result = FindShioiMove(turn, kShioiDepth2);
						if (result.second) {
							auto flg = IsBlockMove(Stone::White, depth);
							board_[p] = Stone::None;
							if (!flg) return true;
						}
						else {
							board_[p] = Stone::None;
						}
					}
				}
			}
		}
		else {
			for (size_t y : rep(range[1], range[3])) {
				for (size_t x : rep(range[0], range[2])) {
					size_t p = ToPosition(x, y);
					if (board_[p] != Stone::None) continue;
					size_t block_position = kBoardSize * kBoardSize;
					size_t sum_4_strong = 0, sum_4_normal = 0;
					for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
						auto move_pattern = GetPatternW(p, static_cast<Direction>(dir));
						if (Board_helper::IsChorenW(move_pattern)) return true;
						size_t s4s, s4n, blk;
						std::tie(s4s, s4n, blk) = CountRenW2(move_pattern, p, static_cast<Direction>(dir));
						sum_4_strong += s4s;
						sum_4_normal += s4n;
						if (s4n) block_position = blk;
					}
					// Shi-ren or Shi-Shi
					if (sum_4_strong > 0) return true;
					if (sum_4_normal > 1) return true;
					// Other Oite
					if (depth > 0) {
						board_[p] = Stone::White;
						auto result = FindShioiMove(turn, kShioiDepth2);
						if (result.second) {
							auto flg = IsBlockMove(Stone::Black, depth);
							board_[p] = Stone::None;
							if (!flg) return true;
						}
						else {
							board_[p] = Stone::None;
						}
					}
				}
			}
		}
		return false;
	}
	bool IsBlockMove(const Stone turn, const size_t depth) {
		auto range = GetRange();
		if (turn == Stone::Black) {
			for (size_t y : rep(range[1], range[3])) {
				for (size_t x : rep(range[0], range[2])) {
					size_t p = ToPosition(x, y);
					if (board_[p] != Stone::None) continue;
					bool cho_ren_flg = false;
					size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
					size_t block_position = kBoardSize * kBoardSize;
					for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
						auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
						if (Board_helper::IsChorenB(move_pattern)) {
							cho_ren_flg = true;
							break;
						}
						size_t s4s, s4n, s3;
						std::tie(s4s, s4n, s3) = CountRenB2(move_pattern, p, static_cast<Direction>(dir), block_position);
						sum_4_strong += s4s;
						sum_4_normal += s4n;
						sum_3 += s3;
					}
					if (cho_ren_flg || sum_4_strong + sum_4_normal >= 2 || sum_3 >= 2) continue;
					// Shi-ren
					if (sum_4_strong > 0) return true;
					// Katsu-Shi or Other move
					board_[p] = turn;
					if (sum_4_normal == 1) {
						if (block_position >= kBoardSize * kBoardSize) {
							board_[p] = Stone::None;
							return true;
						}
						auto enemy_stone = EnemyTurn(turn);
						board_[block_position] = enemy_stone;
						auto result = FindGorenMove(enemy_stone);
						if (result.second) {
							board_[block_position] = Stone::None;
							board_[p] = Stone::None;
							continue;
						}
						result = FindShioiMove(EnemyTurn(turn), kShioiDepth2);
						//auto flg = IsOiteMove(enemy_stone, depth - 1);
						board_[block_position] = Stone::None;
						board_[p] = Stone::None;
						if (/*!flg &&*/ !result.second) return true;
					}else{
						auto result = FindShioiMove(turn, kShioiDepth2);
						if (result.second) {
							board_[p] = Stone::None;
							return true;
						}
						result = FindShioiMove(EnemyTurn(turn), kShioiDepth2);
						//auto flg = IsOiteMove(EnemyTurn(turn), depth - 1);
						board_[p] = Stone::None;
						if (/*!flg &&*/ !result.second) return true;
					}
				}
			}
		}else{
			for (size_t y : rep(range[1], range[3])) {
				for (size_t x : rep(range[0], range[2])) {
					size_t p = ToPosition(x, y);
					if (board_[p] != Stone::None) continue;
					size_t block_position = kBoardSize * kBoardSize;
					size_t sum_4_strong = 0, sum_4_normal = 0;
					for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
						auto move_pattern = GetPatternW(p, static_cast<Direction>(dir));
						if (Board_helper::IsChorenW(move_pattern)) {
							return true;
						}
						size_t s4s, s4n, blk;
						std::tie(s4s, s4n, blk) = CountRenW2(move_pattern, p, static_cast<Direction>(dir));
						sum_4_strong += s4s;
						sum_4_normal += s4n;
						if (s4n) block_position = blk;
					}
					// Shi-ren or Shi-Shi
					if (sum_4_strong > 0) {
						return true;
					}
					if (sum_4_normal > 1) {
						return true;
					}
					// Katsu-Shi or Other move
					board_[p] = turn;
					if (sum_4_normal == 1) {
						if (block_position >= kBoardSize * kBoardSize) {
							board_[p] = Stone::None;
							return true;
						}
						auto enemy_stone = EnemyTurn(turn);
						if (!IsValidMove(block_position)) {
							board_[p] = Stone::None;
							return true;
						}
						board_[block_position] = enemy_stone;
						auto result = FindGorenMove(enemy_stone);
						if (result.second) {
							board_[block_position] = Stone::None;
							board_[p] = Stone::None;
							continue;
						}
						result = FindShioiMove(enemy_stone, kShioiDepth2);
						//auto flg = IsOiteMove(enemy_stone, depth - 1);
						board_[block_position] = Stone::None;
						board_[p] = Stone::None;
						if (/*!flg &&*/ !result.second){
							return true;
						}
					}
					else {
						auto result = FindShioiMove(turn, kShioiDepth2);
						if (result.second) {
							board_[p] = Stone::None;
							return true;
						}
						result = FindShioiMove(EnemyTurn(turn), kShioiDepth2);
						//auto flg = IsOiteMove(EnemyTurn(turn), depth - 1);
						board_[p] = Stone::None;
						if (/*!flg &&*/ !result.second) {
							return true;
						}
					}
				}
			}
		}
		return false;
	}
	Result FindOiteMove(const Stone turn, const size_t depth) {
		auto range = GetRange();
		if (turn == Stone::Black) {
			for (size_t y : rep(range[1], range[3])) {
				for (size_t x : rep(range[0], range[2])) {
					size_t p = ToPosition(x, y);
					//p = StringToPosition("f8");
					if (board_[p] != Stone::None) continue;
					bool cho_ren_flg = false;
					size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
					size_t block_position = kBoardSize * kBoardSize;
					for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
						auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
						if (Board_helper::IsChorenB(move_pattern)) {
							cho_ren_flg = true;
							break;
						}
						size_t s4s, s4n, s3;
						std::tie(s4s, s4n, s3) = CountRenB2(move_pattern, p, static_cast<Direction>(dir), block_position);
						sum_4_strong += s4s;
						sum_4_normal += s4n;
						sum_3 += s3;
					}
					/*if (p == StringToPosition("f8")) {
						auto move_pattern1 = GetPatternB(p, Direction::Column);
						size_t block_position1 = kBoardSize * kBoardSize;
						size_t block_position2 = kBoardSize * kBoardSize;
						auto count1 = CountRenB2(move_pattern1, p, Direction::Column, block_position1);
						auto move_pattern2 = GetPatternB(p, Direction::DiagL);
						auto count2 = CountRenB2(move_pattern2, p, Direction::DiagL, block_position2);
						continue;
					}*/
					if (cho_ren_flg || sum_4_strong + sum_4_normal >= 2 || sum_3 >= 2) continue;
					// Shi-ren
					if (sum_4_strong > 0) return Result(p, true);
					// Other Oite
					board_[p] = Stone::Black;
					auto result = FindShioiMove(turn, kShioiDepth2);
					if (result.second) {
						auto flg = IsBlockMove(Stone::White, depth);
						board_[p] = Stone::None;
						if(!flg) return Result(p, true);
					}
					else {
						board_[p] = Stone::None;
					}
				}
			}
		}
		else {
			for (size_t y : rep(range[1], range[3])) {
				for (size_t x : rep(range[0], range[2])) {
					size_t p = ToPosition(x, y);
					if (board_[p] != Stone::None) continue;
					size_t block_position = kBoardSize * kBoardSize;
					size_t sum_4_strong = 0, sum_4_normal = 0;
					for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
						auto move_pattern = GetPatternW(p, static_cast<Direction>(dir));
						if (Board_helper::IsChorenW(move_pattern)) return Result(p, true);
						size_t s4s, s4n, blk;
						std::tie(s4s, s4n, blk) = CountRenW2(move_pattern, p, static_cast<Direction>(dir));
						sum_4_strong += s4s;
						sum_4_normal += s4n;
						if (s4n) block_position = blk;
					}
					// Shi-ren or Shi-Shi
					if (sum_4_strong > 0) return Result(p, true);
					if (sum_4_normal > 1) return Result(p, true);
					// Other Oite
					board_[p] = Stone::White;
					auto result = FindShioiMove(turn, kShioiDepth2);
					if (result.second) {
						auto flg = IsBlockMove(Stone::Black, depth);
						board_[p] = Stone::None;
						if (!flg) return Result(p, true);
					}
					else {
						board_[p] = Stone::None;
					}
				}
			}
		}
		return Result(0, false);
	}
	// Get Range(for Prospective pruning)
	array<size_t, 4> GetRange() const noexcept {
		//                       left,       top,    right, bottom
		array<size_t, 4> range{{ kBoardSize, kBoardSize, 0, 0 }};
		//外接四角形を求める
		for (size_t y : rep(kBoardSize)) {
			for (size_t x : rep(kBoardSize)) {
				size_t p = x + y * kBoardSize;
				if (board_[p] == Stone::None) continue;
				range[0] = std::min(range[0], x);
				//range[1] = std::min(range[1], y);
				range[2] = std::max(range[2], x);
				range[3] = std::max(range[3], y);
			}
			if (kBoardSize == range[1] && kBoardSize != range[0]) range[1] = y;//いくら分岐予測効くとはいえ毎ループstd::minは呼びたくない
		}

		if (range[0] <= 2) range[0] = 0; else range[0] -= 2;
		if (range[1] <= 2) range[1] = 0; else range[1] -= 2;
		if (range[2] + 2 >= kBoardSize) range[2] = kBoardSize - 1; else range[2] += 2;
		if (range[3] + 2 >= kBoardSize) range[3] = kBoardSize - 1; else range[3] += 2;
		return range;
	}
	// AlphaBeta Method
	int NegaMax(const Stone turn, const size_t depth, int /*alpha*/, int beta) {
		if (FindGorenMove(turn).second) return beta;
		int max_score = -kScoreInf2;
		if (depth == 0) {
			// Find enemy's Shioi(Pre-search)
			if (turn == Stone::Black) {
				auto range = GetRange();
				for (size_t y : rep(range[1], range[3])) {
					for (size_t x : rep(range[0], range[2])) {
						size_t p = ToPosition(x, y);
						if (board_[p] != Stone::None) continue;
						// Judge valid move
						bool cho_ren_flg = false;
						size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
						for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
							auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
							if (Board_helper::IsChorenB(move_pattern)) {
								cho_ren_flg = true;
								break;
							}
							size_t s4s, s4n, s3;
							std::tie(s4s, s4n, s3) = CountRenB(move_pattern, p, static_cast<Direction>(dir));
							sum_4_strong += s4s;
							sum_4_normal += s4n;
							sum_3 += s3;
						}
						if (cho_ren_flg || sum_4_strong + sum_4_normal >= 2 || sum_3 >= 2) continue;
						if (sum_4_strong == 1) return beta;
						int score;
						/* // Find enemy's Shioi
						board_[p] = turn;
						auto result = FindShioiMove(EnemyTurn(turn), kShioiDepth3);
						board_[p] = Stone::None;
						if (result.second) {
							// Don't block Shioi
							score = -kScoreInf;
						}
						else {*/
							// Normal Score
							score = CalcScore(p, turn);
						//}
						if (max_score < score) {
							max_score = score;
							if (max_score >= beta) {
								return beta;
							}
						}
					}
				}
			}
			else {
				auto range = GetRange();
				for (size_t y : rep(range[1], range[3])) {
					for (size_t x : rep(range[0], range[2])) {
						size_t p = ToPosition(x, y);
						if (board_[p] != Stone::None) continue;
						size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
						for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
							auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
							if (Board_helper::IsChorenB(move_pattern)) return beta;
							size_t s4s, s4n, s3;
							std::tie(s4s, s4n, s3) = CountRenB(move_pattern, p, static_cast<Direction>(dir));
							sum_4_strong += s4s;
							sum_4_normal += s4n;
							sum_3 += s3;
						}
						if (sum_4_strong >= 1 || sum_4_normal >= 2) return beta;
						int score;
						/*// Find enemy's Shioi
						board_[p] = turn;
						auto result = FindShioiMove(EnemyTurn(turn), kShioiDepth3);
						board_[p] = Stone::None;
						if (result.second) {
							// Don't block Shioi
							score = -kScoreInf;
						}
						else {*/
							// Normal Score
							score = CalcScore(p, turn);
						//}
						if (max_score < score) {
							max_score = score;
							if (max_score >= beta) {
								return beta;
							}
						}
					}
				}
			}
			return max_score;
		}
		else {
			vector<Score> next_move2;
			auto range = GetRange();
			if (turn == Stone::Black) {
				for (size_t y : rep(range[1], range[3])) {
					for (size_t x : rep(range[0], range[2])) {
						size_t p = ToPosition(x, y);
						if (board_[p] != Stone::None) continue;
						// Judge valid move
						bool cho_ren_flg = false;
						size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
						for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
							auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
							if (Board_helper::IsChorenB(move_pattern)) {
								cho_ren_flg = true;
								break;
							}
							size_t s4s, s4n, s3;
							std::tie(s4s, s4n, s3) = CountRenB(move_pattern, p, static_cast<Direction>(dir));
							sum_4_strong += s4s;
							sum_4_normal += s4n;
							sum_3 += s3;
						}
						if (cho_ren_flg || sum_4_strong + sum_4_normal >= 2 || sum_3 >= 2) continue;
						if (sum_4_strong == 1) return kScoreInf;
						/*// Find enemy's Shioi
						board_[p] = Stone::Black;
						auto result = FindShioiMove(Stone::White, kShioiDepth3);
						board_[p] = Stone::None;
						if (result.second) {
							// Don't block Shioi
							next_move2.emplace_back(p, -kScoreInf);
						}
						else {*/
							// Normal Score
							next_move2.emplace_back(p, kTenGenDist[p]);
						//}
					}
				}
			}
			else {
				for (size_t y : rep(range[1], range[3])) {
					for (size_t x : rep(range[0], range[2])) {
						size_t p = ToPosition(x, y);
						if (board_[p] != Stone::None) continue;
						size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
						for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
							auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
							if (Board_helper::IsChorenB(move_pattern)) return kScoreInf;
							size_t s4s, s4n, s3;
							std::tie(s4s, s4n, s3) = CountRenB(move_pattern, p, static_cast<Direction>(dir));
							sum_4_strong += s4s;
							sum_4_normal += s4n;
							sum_3 += s3;
						}
						if (sum_4_strong >= 1 || sum_4_normal >= 2) return kScoreInf;
						/*// Find enemy's Shioi
						board_[p] = Stone::White;
						auto result = FindShioiMove(Stone::Black, kShioiDepth3);
						board_[p] = Stone::None;
						if (result.second) {
							// Don't block Shioi
							next_move2.emplace_back(p, -kScoreInf);
						}
						else {*/
							// Normal Score
							next_move2.emplace_back(p, kTenGenDist[p]);
						//}
					}
				}
			}
			std::sort(next_move2.begin(), next_move2.end(), [](const Score &a, const Score &b) {return a.second > b.second; });
			for (auto &it : next_move2) {
				board_[it.first] = turn;
				auto score = -NegaMax(EnemyTurn(turn), depth - 1, -kScoreInf2, kScoreInf2) + CalcScore(it.first, turn);
				board_[it.first] = Stone::None;
				if (max_score < score) {
					max_score = score;
					if (max_score >= beta) {
						return beta;
					}
				}
			}
			return max_score;
		}
	}
	int CalcScore(const size_t position, const Stone turn) {
		int score = 0;
		for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
			if (kIterateTable[position][dir][Side::Left] < 1 || kIterateTable[position][dir][Side::Right] < 1) continue;
			Pattern move_pattern = (turn == Stone::Black ? GetPatternB(position, static_cast<Direction>(dir)) : GetPatternW(position, static_cast<Direction>(dir)));
			size_t length = 0;
			bool flg1[] = { false, false };
			for (auto it : { Side::Left, Side::Right }) {
				if (move_pattern[it][0] != turn) {
					flg1[it] = (move_pattern[it][0] == Stone::None);
					continue;
				}
				PackedStone match = turn;
				for (size_t k = 1; k < kIterateTable[position][dir][it] - 1; ++k) {
					if (move_pattern[it][k] != (match | turn)) {
						flg1[it] = (move_pattern[it][k] == match | Stone::None);
						length += k;
						break;
					}
					match = turn | match;
				}
			}
			if (length < 3) {
				size_t cnt = ((flg1[Side::Left] ? 1 : 0) + (flg1[Side::Right] ? 1 : 0));
				score += length * length * cnt * cnt;
			}
		}
		return score;
	}
	// Find Normal move
	Result FindNormalMove(const size_t depth, bool debug_flg = false) {
		vector<size_t> next_move;
		next_move.reserve(kBoardSize * kBoardSize);
		int max_score = -kScoreInf2;
		if (depth == 0) {
			if (turn_ == Stone::Black) {
				for (size_t p : rep(kBoardSize * kBoardSize)) {
					if (board_[p] != Stone::None) continue;
					// Judge valid move
					bool cho_ren_flg = false;
					size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
					for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
						auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
						if (Board_helper::IsChorenB(move_pattern)) {
							cho_ren_flg = true;
							break;
						}
						size_t s4s, s4n, s3;
						std::tie(s4s, s4n, s3) = CountRenB(move_pattern, p, static_cast<Direction>(dir));
						sum_4_strong += s4s;
						sum_4_normal += s4n;
						sum_3 += s3;
					}
					if (cho_ren_flg || sum_4_strong + sum_4_normal >= 2 || sum_3 >= 2) continue;
					if (sum_4_strong == 1) return Result(p, true);
					// Find enemy's Shioi
					board_[p] = turn_;
					auto result = FindShioiMove(EnemyTurn(turn_), kShioiDepth2);
					board_[p] = Stone::None;
					// Don't block Shioi : Normal Score
					const int score = (result.second) ? -kScoreInf : CalcScore(p, turn_);
					if (max_score < score) {
						max_score = score;
						if (debug_flg) std::cerr << endl << "  " << score << " : ";
						next_move.clear();
					}
					if (max_score == score) {
						if (debug_flg) std::cerr << PositionToString(p) << ", ";
						next_move.push_back(p);
					}
				}
			}else{
				for (size_t p : rep(kBoardSize * kBoardSize)) {
					if (board_[p] != Stone::None) continue;
					size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
					for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
						auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
						if (Board_helper::IsChorenB(move_pattern)) return Result(p, true);
						size_t s4s, s4n, s3;
						std::tie(s4s, s4n, s3) = CountRenB(move_pattern, p, static_cast<Direction>(dir));
						sum_4_strong += s4s;
						sum_4_normal += s4n;
						sum_3 += s3;
					}
					if (sum_4_strong >= 1 || sum_4_normal >= 2) return Result(p, true);
					// Find enemy's Shioi
					board_[p] = turn_;
					auto result = FindShioiMove(EnemyTurn(turn_), kShioiDepth2);
					board_[p] = Stone::None;
					// Don't block Shioi : Normal Score
					const int score = (result.second) ? -kScoreInf : CalcScore(p, turn_);
					if (max_score < score) {
						max_score = score;
						if (debug_flg) std::cerr << endl << "  " << score << " : ";
						next_move.clear();
					}
					if (max_score == score) {
						if (debug_flg) std::cerr << PositionToString(p) << ", ";
						next_move.push_back(p);
					}
				}
			}
		}
		else {
			vector<Score> next_move2;
			auto range = GetRange();
			if (turn_ == Stone::Black) {
				for (size_t y : rep(range[1], range[3])) {
					for (size_t x : rep(range[0], range[2])) {
						size_t p = ToPosition(x, y);
						if (board_[p] != Stone::None) continue;
						// Judge valid move
						bool cho_ren_flg = false;
						size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
						for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
							auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
							if (Board_helper::IsChorenB(move_pattern)) {
								cho_ren_flg = true;
								break;
							}
							size_t s4s, s4n, s3;
							std::tie(s4s, s4n, s3) = CountRenB(move_pattern, p, static_cast<Direction>(dir));
							sum_4_strong += s4s;
							sum_4_normal += s4n;
							sum_3 += s3;
						}
						if (cho_ren_flg || sum_4_strong + sum_4_normal >= 2 || sum_3 >= 2) continue;
						if (sum_4_strong == 1) return Result(p, true);
						// Find enemy's Shioi
						board_[p] = Stone::Black;
						auto result = FindShioiMove(Stone::White, kShioiDepth2);
						board_[p] = Stone::None;
						if (result.second) {
							// Don't block Shioi
							next_move2.emplace_back(p, -kScoreInf);
						}
						else {
							// Normal Score
							next_move2.emplace_back(p, kTenGenDist[p]);
						}
					}
				}
			}
			else {
				for (size_t y : rep(range[1], range[3])) {
					for (size_t x : rep(range[0], range[2])) {
						size_t p = ToPosition(x, y);
						if (board_[p] != Stone::None) continue;
						size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
						for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
							auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
							if (Board_helper::IsChorenB(move_pattern)) return Result(p, true);
							size_t s4s, s4n, s3;
							std::tie(s4s, s4n, s3) = CountRenB(move_pattern, p, static_cast<Direction>(dir));
							sum_4_strong += s4s;
							sum_4_normal += s4n;
							sum_3 += s3;
						}
						if (sum_4_strong >= 1 || sum_4_normal >= 2) return Result(p, true);
						// Find enemy's Shioi
						board_[p] = Stone::White;
						auto result = FindShioiMove(Stone::Black, kShioiDepth2);
						board_[p] = Stone::None;
						if (result.second) {
							// Don't block Shioi
							next_move2.emplace_back(p, -kScoreInf);
						}
						else {
							// Normal Score
							next_move2.emplace_back(p, kTenGenDist[p]);
						}
					}
				}
			}
			std::sort(next_move2.begin(), next_move2.end(), [](const Score &a, const Score &b) {return a.second > b.second; });
			for (auto &it : next_move2) {
				board_[it.first] = turn_;
				int score = -NegaMax(EnemyTurn(turn_), depth - 1, -kScoreInf2, kScoreInf2) + CalcScore(it.first, turn_);
				board_[it.first] = Stone::None;
				if (score == kScoreInf) return Result(it.first, true);
				if (max_score < score) {
					max_score = score;
					if (debug_flg) std::cerr << endl << "  " << score << " : ";
					next_move.clear();
				}
				if (max_score == score) {
					if (debug_flg) std::cerr << PositionToString(it.first) << ", ";
					next_move.push_back(it.first);
				}
			}
		}
		if (max_score == -kScoreInf) return Result(kBoardSize * kBoardSize, false);
		return (next_move.size() == 0 ? Result(0, false) : Result(next_move[RandInt(next_move.size())], true));
	}
	// Find Random tsume
	Result FindRandomMove(const Stone /*turn*/) {
		vector<Score> position_list;
		for (size_t p : rep(kBoardSize * kBoardSize)) {
			if (board_[p] != Stone::None) continue;
			if (turn_ == Stone::Black) {
				bool cho_ren_flg = false;
				size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
				for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
					auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
					if (Board_helper::IsChorenB(move_pattern)) {
						cho_ren_flg = true;
						break;
					}
					size_t s4s, s4n, s3;
					std::tie(s4s, s4n, s3) = CountRenB(move_pattern, p, static_cast<Direction>(dir));
					sum_4_strong += s4s;
					sum_4_normal += s4n;
					sum_3 += s3;
				}
				if (cho_ren_flg || sum_4_strong + sum_4_normal >= 2 || sum_3 >= 2) continue;
			}
			position_list.emplace_back(p, 0);
		}
		if (position_list.size() > 0) return Result(position_list[RandInt(position_list.size())].first, true);
		return Result(0, false);
	}
public:
	// constructor
	Board(const char board_text[], const char turn_text[]) 
		: turn_(turn_text[0] | toStone("Can't read turn data."))
	{
		// Read board-text
		if (strlen(board_text) < kBoardSize * kBoardSize) {
			throw std::invalid_argument("Too short board-text size!");
		}
		for (size_t p : rep(kBoardSize * kBoardSize)) {
			board_[p] = board_text[p] | toStone("Can't read board data.");
		}
		// Initialize IterateTable
		constexpr size_t kTengen = kBoardSize / 2;
		for (size_t y : rep(kBoardSize)) {
			for (size_t x : rep(kBoardSize)) {
				const auto p = ToPosition(x, y);
				kTenGenDist[p] = kTengen - std::max(safe_dist(x, kTengen), safe_dist(y, kTengen));
				// Row(─)
				kIterateTable[p][Direction::Row][Side::Right] = std::min(kBoardSize - x - 1, kSearchWidth);
				kIterateTable[p][Direction::Row][Side::Left ] = std::min(x, kSearchWidth);
				// Column(│)
				kIterateTable[p][Direction::Column][Side::Right] = std::min(kBoardSize - y - 1, kSearchWidth);
				kIterateTable[p][Direction::Column][Side::Left ] = std::min(y, kSearchWidth);
				// Diagonally right(／)
				kIterateTable[p][Direction::DiagR][Side::Right] = min(x, kBoardSize - y - 1, kSearchWidth);
				kIterateTable[p][Direction::DiagR][Side::Left] = min(kBoardSize - x - 1, y, kSearchWidth);
				// Diagonally left(＼)
				kIterateTable[p][Direction::DiagL][Side::Right] = min(kBoardSize - x - 1, kBoardSize - y - 1, kSearchWidth);
				kIterateTable[p][Direction::DiagL][Side::Left] = min(x, y, kSearchWidth);
			}
		}
	}
	// Thinking next move
	int NextMove(const size_t depth, bool debug_flg = false) {
		//PutBoard();
		// If the game is end, you don't move.
		if (IsGameEnd()) return -1;
		// Opening move
		if (debug_flg) std::cerr << "Opening" << endl;
		auto result = GetOpeningMove(turn_);
		if (result.second) return result.first;
		// Book move
		if (debug_flg) std::cerr << "Book" << endl;
		auto book_data = book.GetBookData(board_);
		if (book_data.size() != 0) return book_data[RandInt(book_data.size())];
		// If you can make Go-ren, you must do.
		if (debug_flg) std::cerr << "Goren" << endl;
		result = FindGorenMove(turn_);
		if (result.second) return result.first;
		// If enemy can make Go-ren, you must block it.
		if (debug_flg) std::cerr << "Stop Goren" << endl;
		result = FindGorenMove(EnemyTurn(turn_));
		if (result.second) {
			if(turn_ == Stone::White || IsValidMove(result.first)) return result.first;
		}
		// Search of quadruplex's problem(Shioi Tsume)
		if (debug_flg) std::cerr << "Shioi" << endl;
		result = FindShioiMove(turn_, kShioiDepth1);
		if (result.second) return result.first;
		// Search of attack's problem (Oi Tsume)
		if (debug_flg) std::cerr << "Oite" << endl;
		result = FindOiteMove(turn_, kOiteDepth1);
		if (result.second) return result.first;
		// Normal move search
		if (debug_flg) std::cerr << "Normal" << endl;
		result = FindNormalMove(depth, debug_flg);
		if (result.second) return result.first;
		if (result.first >= kBoardSize * kBoardSize) return -2;
		return -1;
	}
	// Test Code
	void Test() {
		auto result = FindNormalMove(0, true);
		return;
	}
};

int main(int argc, char *argv[]) {
	// Read arguments
	if (argc < 4) {
		cout << "Usage: I401 [board_text] [turn_text] [depth] (--debug)" << endl
			<< endl
			<< "board_text     : board data of stone or blank" << endl
			<< "turn_text      : turn data of stone or blank" << endl
			<< "depth          : depth of thinking" << endl
			<< R"(--debug(opt)   : put debug info to std::err if this string is "--debug")" << endl
			<< endl
			<< "stone or blank : black(*), white(O), blank(-)" << endl
			<< "return value   : " << endl
			<< R"(    When you specify "--debug", return >=0[move] | -1[can't move] | -2[give up].)" << endl
			<< "    On the other case, return 0[no error] or -1[error]" << endl;
		return -1;
	}
	try {
		Board board(argv[1], argv[2]);
		//board.Test();
		book = BookDB("book.csv");
		const int depth = argv[3] | to_i() | max(0);
		const bool is_debug = (5 <= argc && 0 == std::strcmp("--debug", argv[4]));
		const auto move = board.NextMove(depth, is_debug);
		cout << move << endl;
		if (is_debug) return move;
//		cout << PositionToString(move) << endl;
	}
	catch (const std::exception& er) {
		std::cerr << er.what() << endl;
		cout << -1 << endl;
		return -1;
	}
	return 0;
}
