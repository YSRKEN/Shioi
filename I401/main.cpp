#include<algorithm>
#include<array>
#include<cmath>
#include<cstdint>
#include<iostream>
#include<random>
#include<string>
#include<tuple>
#include<vector>
#include<cassert>
// using declaration
using std::array;
using std::cout;
using std::endl;
using std::string;
using std::tuple;
using std::vector;
using std::size_t;

// const value declaration
constexpr size_t kBoardSize = 15;
constexpr size_t kSearchWidth = 5;
constexpr size_t kShioiDepth1 = 20, kShioiDepth2 = 3;
constexpr int kScoreInf = 1000;
constexpr int kScoreInf2 = kScoreInf + 1;
const string kPositionStringX = "abcdefghijklmno";
const string kPositionStringY = "123456789ABCDEF";
constexpr size_t kPositionoffset[] = {1, kBoardSize, kBoardSize - 1, kBoardSize + 1};
enum Stone : uint8_t {
	None,
	Black,
	White
};
enum Direction : uint8_t {
	// Row(─) R[1, 0] L[-1, 0]
	Row,
	// Column(│) R[0, 1] L[0, -1]
	Column,
	// Diagonally right(／) R[-1, 1] L[1, -1]
	DiagR,
	// Diagonally left(＼) R[1, 1] L[-1, -1]
	DiagL,
	Directions
};
enum Side : uint8_t {
	Right,
	Left,
	Sides
};

size_t node = 0;

// typedef declaration
typedef std::pair<size_t, int> Score;
typedef std::pair<size_t, bool> Result;
typedef array<array<size_t, 5>, 2> Pattern;
typedef tuple<size_t, size_t, size_t> RenCount;
typedef array<array<array<size_t, Side::Sides>, Direction::Directions>, kBoardSize * kBoardSize> IterateTable;

// definition in global area
std::random_device rd;
std::mt19937 mt(rd());

// misc function
string PositionToString(const size_t p) {
	return kPositionStringX.substr(p % kBoardSize, 1) + kPositionStringY.substr(p / kBoardSize, 1);
}
constexpr inline size_t ToPosition(const size_t x, const size_t y) {
	return x + y * kBoardSize;
}
constexpr inline Stone EnemyTurn(const Stone turn) {
	return (turn == Stone::Black ? Stone::White : Stone::Black);
}
inline size_t RandInt(const size_t n) {
	return std::uniform_int_distribution<size_t>{0, n - 1}(mt);
}
namespace detail {
	struct GetBoardValue_helper {
		size_t pos;
		Direction direction;
		int count;
	};
	constexpr Stone operator|(const std::array<Stone, kBoardSize * kBoardSize>& board, const GetBoardValue_helper& info) {
		return board[info.pos + kPositionoffset[info.direction] * info.count];
	}
	struct StoneNormalizer_helper {};
	constexpr Stone operator| (Stone value, StoneNormalizer_helper) {
		return (value != Stone::White) ? value : Stone::None;
	}
	struct PackPattern_helper {
		int start;
		int stop;
		size_t pos;
		Direction direction;
	};
	constexpr GetBoardValue_helper Get(size_t position, Direction dir, int count) {
		return{ position, dir, count };
	}
	constexpr StoneNormalizer_helper Normalize() { return{}; }
	size_t operator| (const array<Stone, kBoardSize * kBoardSize>& board, const PackPattern_helper& info) {
		using std::abs;
		if (abs(info.start) < abs(info.stop)) return 0;
		size_t re = 0;
		int i;
		for (i = info.start; abs(i) <= abs(info.stop) - 1; i += (i > 0) ? 1 : -1, re <<= 2U) re += board | Get(info.pos, info.direction, i);
		re += board | Get(info.pos, info.direction, i) | Normalize();
		return re;
	}
}
using detail::Get;
using detail::Normalize;
constexpr detail::PackPattern_helper PackPattern(size_t position, Direction dir, int start, int stop) {
	return{ start, stop, position, dir };
}
constexpr size_t operator|(Stone l, Stone r) { return (l << 2) + r; }
constexpr size_t operator|(size_t l, Stone r) { return (l << 2) + r; }
namespace detail {
	constexpr size_t PackPattern_n_impl(const size_t tmp, const Stone s, const size_t rest_count) {
		return (rest_count - 1) ? PackPattern_n_impl((tmp << 2) + s, s, rest_count - 1) : (tmp << 2) + s;
	}
}
constexpr size_t PackPattern_n(const Stone s, size_t n) {
	return (n * 2 > sizeof(size_t)) ? std::numeric_limits<size_t>::max() : detail::PackPattern_n_impl(0U, s, n);
}
namespace Board_helper {
	constexpr std::pair<array<size_t, 2>, size_t> ToPositionByDirection(const Direction d, const size_t i, const size_t j) {
		//C++11 constexprではswitch文が使えないので致し方なし
		return (d < DiagR)
			? (d == Row)
				? std::pair<array<size_t, 2>, size_t>{ {{ ToPosition(j, i), 0 }}, 1U }// Row(─)
				: std::pair<array<size_t, 2>, size_t>{ {{ ToPosition(i, j), 0 }}, 1U }// Column(│)
			: (d == DiagR)
				? std::pair<array<size_t, 2>, size_t>{ {{ ToPosition(i - j, j), ToPosition(kBoardSize - 1 - j, i + j)}}, 2U }// Diagonally right(／)
				: std::pair<array<size_t, 2>, size_t>{ {{ ToPosition(i + j, j), ToPosition(j, i + j)}}, 2U };// Diagonally left(＼)
	}
	constexpr std::pair<array<size_t, 2>, size_t> CalcOuterLoopMaxByDirection(const Direction d) {
		//C++11 constexprではswitch文が使えないので致し方なし
		return (d < DiagR)
			? std::pair<array<size_t, 2>, size_t>{ {{ kBoardSize, 0 }}, 1U }// Row(─) Column(│)
			: (d == DiagR)
				? std::pair<array<size_t, 2>, size_t>{ {{ kBoardSize, kBoardSize - 4}}, 2U }// Diagonally right(／)
				: std::pair<array<size_t, 2>, size_t>{ {{ kBoardSize - 4, kBoardSize - 4 }}, 2U };// Diagonally left(＼)
	}
	std::pair<array<size_t, 2>, size_t> CalcInnerLoopMaxByDirection(const Direction d, const size_t i) {
		switch (d) {
		case Row:
		case Column:
			return std::pair<array<size_t, 2>, size_t>{ { { kBoardSize, 0 }}, 1U };
		case DiagR:
			return std::pair<array<size_t, 2>, size_t>{ { { i + 1, kBoardSize }}, 2U };
		case DiagL:
			return std::pair<array<size_t, 2>, size_t>{ { { kBoardSize, kBoardSize }}, 2U };
		default:
			return{};
		}
	}
}
// Board class
class Board {
	// values
	array<Stone, kBoardSize * kBoardSize> board_;
	Stone turn_;
	IterateTable kIterateTable;
	array<size_t, kBoardSize * kBoardSize> kTenGenDist;
	// Put board
	void PutBoard() {
		const static string kStoneString2[] = { "┼", "●", "○" };
		for (size_t y = 0; y < kBoardSize; ++y) {
			for (size_t x = 0; x < kBoardSize; ++x) {
				size_t p = x + y * kBoardSize;
				cout << kStoneString2[board_[p]];
			}
			cout << endl;
		}
	}
	bool IsGameEndImpl(const Stone turn, Direction d) {
		const auto outer_loop_max = Board_helper::CalcOuterLoopMaxByDirection(d);
		for (size_t c = 0; c < outer_loop_max.second; ++c) {
			for (size_t i = 0; i < outer_loop_max.first[c]; ++i) {
				size_t len = 0;
				const auto inner_loop_max = Board_helper::CalcInnerLoopMaxByDirection(d, i);
				assert(inner_loop_max.second == outer_loop_max.second);
				for (size_t j = 0; j < inner_loop_max.first[c]; ++j) {
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
		for (size_t p = 0; p < kBoardSize * kBoardSize; ++p) {
			if (board_[p] != Stone::None) ++sum;
		}
		return sum;
	}
	// Get Opening move
	Result GetOpeningMove(const Stone turn) {
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
		for (size_t i = 0; i < kBoardSize; ++i) {
			for (size_t j = 0; j < kBoardSize; ++j) {
				size_t p = ToPosition(j, i);
				if (board_[p] != Stone::None) continue;
				size_t rlengh = 0;
				for (int k = j - 1; k >= 0; --k) {
					if (board_[ToPosition(k, i)] != turn) break;
					++rlengh;
				}
				size_t llengh = 0;
				for (size_t k = j + 1; k < kBoardSize; ++k) {
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
		for (size_t i = 0; i < kBoardSize; ++i) {
			for (size_t j = 0; j < kBoardSize; ++j) {
				size_t p = ToPosition(i, j);
				if (board_[p] != Stone::None) continue;
				size_t rlengh = 0;
				for (int k = j - 1; k >= 0; --k) {
					if (board_[ToPosition(i, k)] != turn) break;
					++rlengh;
				}
				size_t llengh = 0;
				for (size_t k = j + 1; k < kBoardSize; ++k) {
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
		for (size_t i = 4; i < kBoardSize; ++i) {
			for (size_t j = 0; j <= i; ++j) {
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
		for (size_t i = 1; i < kBoardSize - 4; ++i) {
			for (size_t j = 0; j < kBoardSize - i; ++j) {
				size_t p = ToPosition(kBoardSize - 1 - j, i + j);
				if (board_[p] != Stone::None) continue;
				size_t rlengh = 0;
				for (int k = j - 1; k >= 0; --k) {
					if (board_[ToPosition(kBoardSize - 1 - k, i + k)] != turn) break;
					++rlengh;
				}
				size_t llengh = 0;
				for (size_t k = j + 1; k < kBoardSize - i; ++k) {
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
		for (size_t i = 0; i < kBoardSize - 4; ++i) {
			for (size_t j = 0; j < kBoardSize - i; ++j) {
				size_t p = ToPosition(i + j, j);
				if (board_[p] != Stone::None) continue;
				size_t rlengh = 0;
				for (int k = j - 1; k >= 0; --k) {
					if (board_[ToPosition(i + k, k)] != turn) break;
					++rlengh;
				}
				size_t llengh = 0;
				for (size_t k = j + 1; k < kBoardSize - i; ++k) {
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
		for (size_t i = 1; i < kBoardSize - 4; ++i) {
			for (size_t j = 0; j < kBoardSize - i; ++j) {
				size_t p = ToPosition(j, i + j);
				if (board_[p] != Stone::None) continue;
				size_t rlengh = 0;
				for (int k = j - 1; k >= 0; --k) {
					if (board_[ToPosition(k, i + k)] != turn) break;
					++rlengh;
				}
				size_t llengh = 0;
				for (size_t k = j + 1; k < kBoardSize - i; ++k) {
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
		const size_t right_pattern_length = kIterateTable[position][dir][Side::Right];
		const size_t  left_pattern_length = kIterateTable[position][dir][Side::Left ];
		Pattern pattern;
		auto GetBoardValue = [this](size_t position, Direction dir, int count) -> Stone {return board_[position + kPositionoffset[dir] * count]; };
		auto GetBoardValue2 = [&GetBoardValue](size_t position, Direction dir, int count) -> Stone {
			auto value = GetBoardValue(position, dir, count);
			return (value != Stone::White ? value : Stone::None);
		};
		pattern[Side::Right][0] = (right_pattern_length <= 0 ? Stone::None : GetBoardValue2(position, dir, 1));
		pattern[Side::Right][1] = (right_pattern_length <= 1) ? pattern[Side::Right][0] | Stone::None
			: this->board_ | PackPattern(position, dir, 1, 2);
		pattern[Side::Right][2] = (right_pattern_length <= 2) ? pattern[Side::Right][1] | Stone::None
			: this->board_ | PackPattern(position, dir, 1, 3);
		pattern[Side::Right][3] = (right_pattern_length <= 3) ? pattern[Side::Right][2] | Stone::None
			: this->board_ | PackPattern(position, dir, 1, 4);
		pattern[Side::Right][4] = (right_pattern_length <= 4) ? pattern[Side::Right][3] | Stone::None
			: this->board_ | PackPattern(position, dir, 1, 5);

		pattern[Side::Left][0] = (left_pattern_length <= 0 ? Stone::None : GetBoardValue2(position, dir, -1));
		pattern[Side::Left][1] = (left_pattern_length <= 1) ? pattern[Side::Left][0] | Stone::None
			: this->board_ | PackPattern(position, dir, -1, -2);
		pattern[Side::Left][2] = (left_pattern_length <= 2) ? pattern[Side::Left][1] | Stone::None
			: this->board_ | PackPattern(position, dir, -1, -3);
		pattern[Side::Left][3] = (left_pattern_length <= 3) ? pattern[Side::Left][2] | Stone::None
			: this->board_ | PackPattern(position, dir, -1, -4);
		pattern[Side::Left][4] = (left_pattern_length <= 4) ? pattern[Side::Left][3] | Stone::None
			: this->board_ | PackPattern(position, dir, -1, -4);
		return pattern;
	}
	Pattern GetPatternW(const size_t position, const Direction dir) {
		const size_t right_pattern_length = kIterateTable[position][dir][Side::Right];
		const size_t  left_pattern_length = kIterateTable[position][dir][Side::Left];
		Pattern pattern;
		auto GetBoardValue = [this](size_t position, Direction dir, int count) -> Stone {return board_[position + kPositionoffset[dir] * count]; };
		pattern[Side::Right][0] = (right_pattern_length <= 0 ? Stone::None
			: GetBoardValue(position, dir, 1));
		pattern[Side::Right][1] = (right_pattern_length <= 1) ? pattern[Side::Right][0] | Stone::None
			: this->board_ | PackPattern(position, dir, 1, 2);
		pattern[Side::Right][2] = (right_pattern_length <= 2) ? pattern[Side::Right][1] | Stone::None
			: this->board_ | PackPattern(position, dir, 1, 3);
		pattern[Side::Right][3] = (right_pattern_length <= 3) ? pattern[Side::Right][2] | Stone::None
			: this->board_ | PackPattern(position, dir, 1, 4);

		pattern[Side::Left][0] = (left_pattern_length <= 0 ? Stone::None
			: GetBoardValue(position, dir, -1));
		pattern[Side::Left][1] = (left_pattern_length <= 1) ? pattern[Side::Left][0] | Stone::None
			: this->board_ | PackPattern(position, dir, -1, -2);
		pattern[Side::Left][2] = (left_pattern_length <= 2) ? pattern[Side::Left][1] | Stone::None
			: this->board_ | PackPattern(position, dir, -1, -3);
		pattern[Side::Left][3] = (left_pattern_length <= 3) ? pattern[Side::Left][2] | Stone::None
			: this->board_ | PackPattern(position, dir, -1, -4);
		return pattern;
	}
	// Check Cho-ren
	bool IsChorenB(const Pattern &pattern) {
		if (pattern[Side::Right][4] == PackPattern_n(Stone::Black, 5)) return true;
		if (pattern[Side::Right][3] == PackPattern_n(Stone::Black, 4)
			&& pattern[Side::Left][0] == Stone::Black) return true;
		if (pattern[Side::Right][2] == PackPattern_n(Stone::Black, 3)
			&& pattern[Side::Left][1] == PackPattern_n(Stone::Black, 2)) return true;
		if (pattern[Side::Left][4] == PackPattern_n(Stone::Black, 5)) return true;
		if (pattern[Side::Left][3] == PackPattern_n(Stone::Black, 4)
			&& pattern[Side::Right][0] == Stone::Black) return true;
		if (pattern[Side::Left][2] == PackPattern_n(Stone::Black, 3)
			&& pattern[Side::Right][1] == PackPattern_n(Stone::Black, 2)) return true;
		return false;
	}
	bool IsChorenW(const Pattern &pattern) {
		return false;
	}
	// Matching Pattern
	bool MatchPatternB(const Pattern &pattern, const size_t position, const Direction dir, const Side side) const noexcept {
		return (pattern[side][0] == Stone::None);
	}
	bool MatchPatternB(const Pattern &pattern, const size_t position, const Direction dir, const Side side, const size_t s1) const noexcept {
		return (kIterateTable[position][dir][side] >= 1 && pattern[side][1] == (s1 | Stone::None));
	}
	bool MatchPatternW(const Pattern &pattern, const size_t position, const Direction dir, const Side side, const size_t s1) const noexcept {
		return (kIterateTable[position][dir][side] >= 1 && pattern[side][0] == s1);
	}
	// Count Ren
	RenCount CountRenB(const Pattern &pattern, const size_t position, const Direction dir) {
		/* Count "Straight Shi-Shi" */
		/* 1. BO|BBB|OB   "Ryoutou-no-ShiShi"
		 * 2. BBO|BB|OBB  "Chouda-no-ShiShi"
		 * 3. BBBO|B|OBBB "Souryu-no-ShiShi"
		 * 一直線上に出来る四々で、両頭の四々・長蛇の四々・双龍の四々
		 */
		{
			//BO|BBB|OB
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::Black)) return RenCount(0, 2, 0);
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None | Stone::Black)) return RenCount(0, 2, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::Black)) return RenCount(0, 2, 0);
			//BBO|BB|OBB
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None | Stone::Black | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::Black | Stone::Black)) return RenCount(0, 2, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None | Stone::Black | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::Black | Stone::Black)) return RenCount(0, 2, 0);
			//BBBO|B|OBBB
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::Black | Stone::Black | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::Black | Stone::Black | Stone::Black)) return RenCount(0, 2, 0);
		}
		// Count Shi-ren
		{
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None)) return RenCount(1, 0, 0);
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None)) return RenCount(1, 0, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::None)) return RenCount(1, 0, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None)) return RenCount(1, 0, 0);
		}
		// Count Katsu-Shi
		{
			//11110
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::Black)) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black)) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black)) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Left)) return RenCount(0, 1, 0);
			//11101
			if (MatchPatternB(pattern, position, dir, Side::Right)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::Black | Stone::Black | Stone::Black)) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black)) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black)) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Left)) return RenCount(0, 1, 0);
			//11011
			if (MatchPatternB(pattern, position, dir, Side::Right)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None | Stone::Black | Stone::Black)) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::Black | Stone::Black)) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None | Stone::Black | Stone::Black)) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::Black | Stone::Black)) return RenCount(0, 1, 0);
			//10111
			if (MatchPatternB(pattern, position, dir, Side::Left)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::Black | Stone::Black | Stone::Black)) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black)) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black)) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Right)) return RenCount(0, 1, 0);
			//01111
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::Black)) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black)) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black)) return RenCount(0, 1, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Right)) return RenCount(0, 1, 0);
		}
		// Count Katsu-San
		{
			int offset1 = 0, offset2 = 0;
			while (true) {
				//011100
				if (MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::None)) {
					offset1 = 1; offset2 = -3; break;
				}
				if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None)) {
					offset1 = 2; offset2 = -2; break;
				}
				if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::None | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None)) {
					offset1 = 3; offset2 = -1; break;
				}
				//011010
				if (MatchPatternB(pattern, position, dir, Side::Right, Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::Black | Stone::Black | Stone::None)) {
					offset1 = -1; break;
				}
				if (MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::Black | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None)) {
					offset1 = 1; break;
				}
				if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None | Stone::Black | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None)) {
					offset1 = 2; break;
				}
				//010110
				if (MatchPatternB(pattern, position, dir, Side::Left, Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::Black | Stone::Black | Stone::None)) {
					offset1 = 1; break;
				}
				if (MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::Black | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None)) {
					offset1 = -1; break;
				}
				if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None | Stone::Black | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Right, Stone::None)) {
					offset1 = -2; break;
				}
				//001110
				if (MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::None)) {
					offset1 = -1; offset2 = 3; break;
				}
				if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None)) {
					offset1 = -2; offset2 = 2; break;
				}
				if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::None | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Right, Stone::None)) {
					offset1 = -3; offset2 = 1; break;
				}
				break;
			}
			auto GetPosition = [&position, &dir](int count) -> int {return position + kPositionoffset[dir] * count; };
			if (offset2 != 0) {
				int next_pos = GetPosition(offset2);
				if (next_pos >= 0 && next_pos < kBoardSize * kBoardSize) {
					board_[position] = Stone::Black;
					auto valid_flg = IsValidMove(next_pos);
					board_[position] = Stone::None;
					if (valid_flg) return RenCount(0, 0, 1);
				}
			}
			if (offset1 != 0) {
				int next_pos = GetPosition(offset1);
				if (next_pos >= 0 && next_pos < kBoardSize * kBoardSize) {
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
		{
			//BO|BBB|OB
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White | Stone::None | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::None | Stone::White)) return RenCount(0, 2, 0);
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::None | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::None | Stone::White)) return RenCount(0, 2, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White | Stone::None | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::None | Stone::White)) return RenCount(0, 2, 0);
			//BBO|BB|OBB
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::None | Stone::White | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::None | Stone::White | Stone::White)) return RenCount(0, 2, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::None | Stone::White | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::None | Stone::White | Stone::White)) return RenCount(0, 2, 0);
			//BBBO|B|OBBB
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::None | Stone::White | Stone::White | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::None | Stone::White | Stone::White | Stone::White)) return RenCount(0, 2, 0);
		}
		// Count Shi-ren
		{
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White | Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::None)) return RenCount(1, 0, 0);
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::None)) return RenCount(1, 0, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White | Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::None)) return RenCount(1, 0, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::None)) return RenCount(1, 0, 0);
		}
		// Count Katsu-Shi
		{
			//11110
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White | Stone::White)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White | Stone::White | Stone::None)) return RenCount(0, 1, 0);
			//11101
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::None | Stone::White | Stone::White | Stone::White)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::None | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::None | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White | Stone::None | Stone::White)) return RenCount(0, 1, 0);
			//11011
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::None | Stone::White | Stone::White)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::None | Stone::White | Stone::White)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::None | Stone::White | Stone::White)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::None | Stone::White | Stone::White)) return RenCount(0, 1, 0);
			//10111
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::None | Stone::White | Stone::White | Stone::White)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::None | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::None | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::White)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White | Stone::None | Stone::White)) return RenCount(0, 1, 0);
			//01111
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White | Stone::White)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::White)) return RenCount(0, 1, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White | Stone::White | Stone::None)) return RenCount(0, 1, 0);
		}
		// Count Katsu-san
		{
			//011100
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::None | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White | Stone::None)) return RenCount(0, 0, 1);
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::None | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::None)) return RenCount(0, 0, 1);
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White | Stone::None | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::None)) return RenCount(0, 0, 1);
			//011010
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::None | Stone::White | Stone::White | Stone::None)) return RenCount(0, 0, 1);
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::None | Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::None)) return RenCount(0, 0, 1);
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::None | Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::None)) return RenCount(0, 0, 1);
			//010110
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::None | Stone::White | Stone::White | Stone::None)) return RenCount(0, 0, 1);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::None | Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::None)) return RenCount(0, 0, 1);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::None | Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::None)) return RenCount(0, 0, 1);
			//001110
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::None | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White | Stone::None)) return RenCount(0, 0, 1);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::None | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::None)) return RenCount(0, 0, 1);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White | Stone::None | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::None)) return RenCount(0, 0, 1);
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
		{
			//BO|BBB|OB
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::Black)) return RenCount(0, 2, 0);
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None | Stone::Black)) return RenCount(0, 2, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::Black)) return RenCount(0, 2, 0);
			//BBO|BB|OBB
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None | Stone::Black | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::Black | Stone::Black)) return RenCount(0, 2, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None | Stone::Black | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::Black | Stone::Black)) return RenCount(0, 2, 0);
			//BBBO|B|OBBB
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::Black | Stone::Black | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::Black | Stone::Black | Stone::Black)) return RenCount(0, 2, 0);
		}
		// Count Shi-ren
		{
			if(MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None)) return RenCount(1, 0, 0);
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None)) return RenCount(1, 0, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::None)) return RenCount(1, 0, 0);
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None)) return RenCount(1, 0, 0);
		}
		// Count Katsu-Shi
		auto GetPosition = [&position, &dir](int count) -> int {return position + kPositionoffset[dir] * count; };
		{
			//11110
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::Black)) {
				int bp = GetPosition(1);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black)) {
				int bp = GetPosition(2);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black)) {
				int bp = GetPosition(3);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Left)) {
				int bp = GetPosition(4);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			//11101
			if (MatchPatternB(pattern, position, dir, Side::Right)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::Black | Stone::Black | Stone::Black)) {
				int bp = GetPosition(-1);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black)) {
				int bp = GetPosition(1);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black)) {
				int bp = GetPosition(2);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Left)) {
				int bp = GetPosition(3);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			//11011
			if (MatchPatternB(pattern, position, dir, Side::Right)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None | Stone::Black | Stone::Black)) {
				int bp = GetPosition(-2);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::Black | Stone::Black)) {
				int bp = GetPosition(-1);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Left)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None | Stone::Black | Stone::Black)) {
				int bp = GetPosition(2);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::Black | Stone::Black)) {
				int bp = GetPosition(1);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			//10111
			if (MatchPatternB(pattern, position, dir, Side::Left)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::Black | Stone::Black | Stone::Black)) {
				int bp = GetPosition(1);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black)) {
				int bp = GetPosition(-1);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black)) {
				int bp = GetPosition(-2);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::None | Stone::Black)
				&& MatchPatternB(pattern, position, dir, Side::Right)) {
				int bp = GetPosition(-3);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			//01111
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::Black)) {
				int bp = GetPosition(-1);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black)) {
				int bp = GetPosition(-2);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black)) {
				int bp = GetPosition(-3);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::Black | Stone::None)
				&& MatchPatternB(pattern, position, dir, Side::Right)) {
				int bp = GetPosition(-4);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
		}
		// Count Katsu-San
		{
			int offset1 = 0, offset2 = 0;
			while (true) {
				//011100
				if (MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::None)) {
					offset1 = 1; offset2 = -3; break;
				}
				if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None)) {
					offset1 = 2; offset2 = -2; break;
				}
				if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::None | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None)) {
					offset1 = 3; offset2 = -1; break;
				}
				//011010
				if (MatchPatternB(pattern, position, dir, Side::Right, Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::Black | Stone::Black | Stone::None)) {
					offset1 = -1; break;
				}
				if (MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::Black | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None)) {
					offset1 = 1; break;
				}
				if (MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None | Stone::Black | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Left, Stone::None)) {
					offset1 = 2; break;
				}
				//010110
				if (MatchPatternB(pattern, position, dir, Side::Left, Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Right, Stone::None | Stone::Black | Stone::Black | Stone::None)) {
					offset1 = 1; break;
				}
				if (MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::Black | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None)) {
					offset1 = -1; break;
				}
				if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None | Stone::Black | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Right, Stone::None)) {
					offset1 = -2; break;
				}
				//001110
				if (MatchPatternB(pattern, position, dir, Side::Left, Stone::None | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::Black | Stone::None)) {
					offset1 = -1; offset2 = 3; break;
				}
				if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::None | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Right, Stone::Black | Stone::None)) {
					offset1 = -2; offset2 = 2; break;
				}
				if (MatchPatternB(pattern, position, dir, Side::Left, Stone::Black | Stone::Black | Stone::None | Stone::None)
					&& MatchPatternB(pattern, position, dir, Side::Right, Stone::None)) {
					offset1 = -3; offset2 = 1; break;
				}
				break;
			}
			if (offset2 != 0) {
				int next_pos = GetPosition(offset2);
				if (next_pos >= 0 && next_pos < kBoardSize * kBoardSize) {
					board_[position] = Stone::Black;
					auto valid_flg = IsValidMove(next_pos);
					board_[position] = Stone::None;
					if (valid_flg) return RenCount(0, 0, 1);
				}
			}
			if (offset1 != 0) {
				int next_pos = GetPosition(offset1);
				if (next_pos >= 0 && next_pos < kBoardSize * kBoardSize) {
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
		{
			//BO|BBB|OB
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White | Stone::None | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::None | Stone::White)) return RenCount(0, 2, 0);
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::None | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::None | Stone::White)) return RenCount(0, 2, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White | Stone::None | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::None | Stone::White)) return RenCount(0, 2, 0);
			//BBO|BB|OBB
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::None | Stone::White | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::None | Stone::White | Stone::White)) return RenCount(0, 2, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::None | Stone::White | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::None | Stone::White | Stone::White)) return RenCount(0, 2, 0);
			//BBBO|B|OBBB
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::None | Stone::White | Stone::White | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::None | Stone::White | Stone::White | Stone::White)) return RenCount(0, 2, 0);
		}
		// Count Shi-ren
		{
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White | Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::None)) return RenCount(1, 0, 0);
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::None)) return RenCount(1, 0, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White | Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::None)) return RenCount(1, 0, 0);
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::None)) return RenCount(1, 0, 0);
		}
		// Count Katsu-Shi
		{
			auto GetPosition = [&position, &dir](int count) -> int {return position + kPositionoffset[dir] * count; };
			//11110
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White | Stone::White)) {
				int bp = GetPosition(1);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White)) {
				int bp = GetPosition(2);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White)) {
				int bp = GetPosition(3);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White | Stone::White | Stone::None)) {
				int bp = GetPosition(4);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			//11101
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::None | Stone::White | Stone::White | Stone::White)) {
				int bp = GetPosition(-1);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::None | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White)) {
				int bp = GetPosition(1);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::None | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::White)) {
				int bp = GetPosition(2);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White | Stone::None | Stone::White)) {
				int bp = GetPosition(3);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			//11011
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::None | Stone::White | Stone::White)) {
				int bp = GetPosition(-2);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Left, Stone::None | Stone::White | Stone::White)) {
				int bp = GetPosition(-1);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::None | Stone::White | Stone::White)) {
				int bp = GetPosition(2);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::None | Stone::White | Stone::White)) {
				int bp = GetPosition(1);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			//10111
			if (MatchPatternW(pattern, position, dir, Side::Right, Stone::None | Stone::White | Stone::White | Stone::White)) {
				int bp = GetPosition(1);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::None | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White)) {
				int bp = GetPosition(-1);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::None | Stone::White)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::White)) {
				int bp = GetPosition(-2);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White | Stone::None | Stone::White)) {
				int bp = GetPosition(-3);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			//01111
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White | Stone::White)) {
				int bp = GetPosition(-1);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::White | Stone::White)) {
				int bp = GetPosition(-2);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White | Stone::None)
				&& MatchPatternW(pattern, position, dir, Side::Right, Stone::White)) {
				int bp = GetPosition(-3);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
			if (MatchPatternW(pattern, position, dir, Side::Left, Stone::White | Stone::White | Stone::White | Stone::None)) {
				int bp = GetPosition(-4);
				return RenCount(0, 1, (bp >= 0 && bp < kBoardSize * kBoardSize ? bp : kBoardSize * kBoardSize));
			}
		}
		return RenCount(0, 0, 0);
	}
	// Judge valid move(Black-side only)
	bool IsValidMove(const size_t p) {
		size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
		for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
			auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
			if (IsChorenB(move_pattern)) return false;
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
			//PutBoard();
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
		for (size_t p = 0; p < kBoardSize * kBoardSize; ++p) {
			if (board_[p] != Stone::None) continue;
			if (turn == Stone::Black) {
				bool cho_ren_flg = false;
				size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
				size_t block_position2;
				for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
					auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
					if (IsChorenB(move_pattern)) {
						cho_ren_flg = true;
						break;
					}
					auto ren_count = CountRenB(move_pattern, p, static_cast<Direction>(dir));
					size_t s4s, s4n, s3;
					std::tie(s4s, s4n, s3) = CountRenB2(move_pattern, p, static_cast<Direction>(dir), block_position2);
					sum_4_strong += s4s;
					sum_4_normal += s4n;
					sum_3 += s3;
				}
				if (cho_ren_flg || sum_4_strong + sum_4_normal >= 2 || sum_3 >= 2) continue;
				if (sum_4_strong > 0) {
					//PutBoard();
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
				size_t block_position2;
				size_t sum_4_strong = 0, sum_4_normal = 0;
				for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
					auto move_pattern = GetPatternW(p, static_cast<Direction>(dir));
					auto ren_count = CountRenW2(move_pattern, p, static_cast<Direction>(dir));
					if (IsChorenW(move_pattern)) return true;
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

		board_[block_position] = Stone::None;
		return false;
	}
	Result FindShioiMove(const Stone turn, const size_t depth) {
		if (turn == Stone::Black) {
			for (size_t p = 0; p < kBoardSize * kBoardSize; ++p) {
				if (board_[p] != Stone::None) continue;
				bool cho_ren_flg = false;
				size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
				size_t block_position;
				for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
					auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
					if (IsChorenB(move_pattern)) {
						cho_ren_flg = true;
						break;
					}
					size_t s4s, s4n, s3;
					std::tie(s4s, s4n, s3) = CountRenB2(move_pattern, p, static_cast<Direction>(dir), block_position);
					sum_4_strong += s4s;
					sum_4_normal += s4n;
					sum_3 += s3;
				}
				/*if (sum_4_strong + sum_4_normal > 0) {
				cout << PositionToString(p) << " " << sum_4_strong << " " << sum_4_normal << endl;
				}*/
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
		}else{
			for (size_t p = 0; p < kBoardSize * kBoardSize; ++p) {
				if (board_[p] != Stone::None) continue;
				size_t block_position;
				size_t sum_4_strong = 0, sum_4_normal = 0;
				for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
					auto move_pattern = GetPatternW(p, static_cast<Direction>(dir));
					auto ren_count = CountRenW2(move_pattern, p, static_cast<Direction>(dir));
					if (IsChorenW(move_pattern)) return Result(p, true);
					size_t s4s, s4n, blk;
					std::tie(s4s, s4n, blk) = CountRenW2(move_pattern, p, static_cast<Direction>(dir));
					sum_4_strong += s4s;
					sum_4_normal += s4n;
					if (s4n) block_position = blk;
				}
				/*if (sum_4_strong + sum_4_normal >= 1) {
				cout << PositionToString(p) << " " << sum_4_strong << " " << sum_4_normal << endl;
				}*/
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
		return Result(0, false);
	}
	// Get Range(for Prospective pruning)
	array<size_t, 4> GetRange() const noexcept {
		array<size_t, 4> range{ kBoardSize, kBoardSize, 0, 0 };
		for (size_t y = 0; y < kBoardSize; ++y) {
			for (size_t x = 0; x < kBoardSize; ++x) {
				size_t p = x + y * kBoardSize;
				if (board_[p] == Stone::None) continue;
				range[0] = std::min(range[0], x);
				range[1] = std::min(range[1], y);
				range[2] = std::max(range[2], x);
				range[3] = std::max(range[3], y);
			}
		}
		if (range[0] <= 2) range[0] = 0; else range[0] -= 2;
		if (range[1] <= 2) range[1] = 0; else range[1] -= 2;
		if (range[2] + 2 >= kBoardSize) range[2] = kBoardSize - 1; else range[2] += 2;
		if (range[3] + 2 >= kBoardSize) range[3] = kBoardSize - 1; else range[3] += 2;
		return range;
	}
	// AlphaBeta Method
	int NegaMax(const Stone turn, const size_t depth, int alpha, int beta) {
		if (FindGorenMove(turn).second) return beta;
		int max_score = -kScoreInf2;
		if (depth == 0) {
			// Find enemy's Shioi(Pre-search)
			if (turn == Stone::Black) {
				auto range = GetRange();
				for (size_t y = range[1]; y <= range[3]; ++y) {
					for (size_t x = range[0]; x <= range[2]; ++x) {
						size_t p = ToPosition(x, y);
						if (board_[p] != Stone::None) continue;
						// Judge valid move
						bool cho_ren_flg = false;
						size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
						for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
							auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
							if (IsChorenB(move_pattern)) {
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
							score = kTenGenDist[p];
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
				for (size_t y = range[1]; y <= range[3]; ++y) {
					for (size_t x = range[0]; x <= range[2]; ++x) {
						size_t p = ToPosition(x, y);
						if (board_[p] != Stone::None) continue;
						size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
						for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
							auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
							if (IsChorenB(move_pattern)) return beta;
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
							score = kTenGenDist[p];
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
				for (size_t y = range[1]; y <= range[3]; ++y) {
					for (size_t x = range[0]; x <= range[2]; ++x) {
						size_t p = ToPosition(x, y);
						if (board_[p] != Stone::None) continue;
						// Judge valid move
						bool cho_ren_flg = false;
						size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
						for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
							auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
							if (IsChorenB(move_pattern)) {
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
							next_move2.push_back(Score(p, -kScoreInf));
						}
						else {*/
							// Normal Score
							next_move2.push_back(Score(p, kTenGenDist[p]));
						//}
					}
				}
			}
			else {
				for (size_t y = range[1]; y <= range[3]; ++y) {
					for (size_t x = range[0]; x <= range[2]; ++x) {
						size_t p = ToPosition(x, y);
						if (board_[p] != Stone::None) continue;
						size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
						for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
							auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
							if (IsChorenB(move_pattern)) return kScoreInf;
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
							next_move2.push_back(Score(p, -kScoreInf));
						}
						else {*/
							// Normal Score
							next_move2.push_back(Score(p, kTenGenDist[p]));
						//}
					}
				}
			}
			std::sort(next_move2.begin(), next_move2.end(), [](const Score &a, const Score &b) {return a.second > b.second; });
			for (auto &it : next_move2) {
				board_[it.first] = turn;
				auto score = -NegaMax(EnemyTurn(turn), depth - 1, -kScoreInf2, kScoreInf2);
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
	// Find Normal move
	Result FindNormalMove(const size_t depth, bool debug_flg = false) {
		vector<size_t> next_move;
		int max_score = -kScoreInf2;
		if (depth == 0) {
			if (turn_ == Stone::Black) {
				for (size_t p = 0; p < kBoardSize * kBoardSize; ++p) {
					if (board_[p] != Stone::None) continue;
					// Judge valid move
					bool cho_ren_flg = false;
					size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
					for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
						auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
						if (IsChorenB(move_pattern)) {
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
					int score;
					board_[p] = turn_;
					auto result = FindShioiMove(EnemyTurn(turn_), kShioiDepth2);
					board_[p] = Stone::None;
					if (result.second) {
						// Don't block Shioi
						score = -kScoreInf;
					}
					else {
						// Normal Score
						score = kTenGenDist[p];
					}
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
				for (size_t p = 0; p < kBoardSize * kBoardSize; ++p) {
					if (board_[p] != Stone::None) continue;
					size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
					for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
						auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
						if (IsChorenB(move_pattern)) return Result(p, true);
						size_t s4s, s4n, s3;
						std::tie(s4s, s4n, s3) = CountRenB(move_pattern, p, static_cast<Direction>(dir));
						sum_4_strong += s4s;
						sum_4_normal += s4n;
						sum_3 += s3;
					}
					if (sum_4_strong >= 1 || sum_4_normal >= 2) return Result(p, true);
					// Find enemy's Shioi
					int score;
					board_[p] = turn_;
					auto result = FindShioiMove(EnemyTurn(turn_), kShioiDepth2);
					board_[p] = Stone::None;
					if (result.second) {
						// Don't block Shioi
						score = -kScoreInf;
					}
					else {
						// Normal Score
						score = kTenGenDist[p];
					}
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
				for (size_t y = range[1]; y <= range[3]; ++y) {
					for (size_t x = range[0]; x <= range[2]; ++x) {
						size_t p = ToPosition(x, y);
						if (board_[p] != Stone::None) continue;
						// Judge valid move
						bool cho_ren_flg = false;
						size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
						for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
							auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
							if (IsChorenB(move_pattern)) {
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
							next_move2.push_back(Score(p, -kScoreInf));
						}
						else {
							// Normal Score
							next_move2.push_back(Score(p, kTenGenDist[p]));
						}
					}
				}
			}
			else {
				for (size_t y = range[1]; y <= range[3]; ++y) {
					for (size_t x = range[0]; x <= range[2]; ++x) {
						size_t p = ToPosition(x, y);
						if (board_[p] != Stone::None) continue;
						size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
						for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
							auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
							if (IsChorenB(move_pattern)) return Result(p, true);
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
							next_move2.push_back(Score(p, -kScoreInf));
						}
						else {
							// Normal Score
							next_move2.push_back(Score(p, kTenGenDist[p]));
						}
					}
				}
			}
			std::sort(next_move2.begin(), next_move2.end(), [](const Score &a, const Score &b) {return a.second > b.second; });
			for (auto &it : next_move2) {
				board_[it.first] = turn_;
				int score = -NegaMax(EnemyTurn(turn_), depth - 1, -kScoreInf2, kScoreInf2) + kTenGenDist[it.first];
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
	Result FindRandomMove(const Stone turn) {
		vector<Score> position_list;
		for (size_t p = 0; p < kBoardSize * kBoardSize; ++p) {
			if (board_[p] != Stone::None) continue;
			if (turn_ == Stone::Black) {
				bool cho_ren_flg = false;
				size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
				for (uint8_t dir = 0; dir < Direction::Directions; ++dir) {
					auto move_pattern = GetPatternB(p, static_cast<Direction>(dir));
					if (IsChorenB(move_pattern)) {
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
			position_list.push_back(Score(p, 0));
		}
		if (position_list.size() > 0) return Result(position_list[RandInt(position_list.size())].first, true);
		return Result(0, false);
	}
public:
	// constructor
	Board(const char board_text[], const char turn_text[]) {
		const static string kStoneString = "-*O";
		// Read board-text
		const string board_text_str(board_text);
		if (board_text_str.size() < kBoardSize * kBoardSize) {
			throw std::exception("Too short board-text size!");
		}
		for (size_t p = 0; p < kBoardSize * kBoardSize; ++p) {
			auto stone_type = kStoneString.find_first_of(board_text[p], 0);
			if (stone_type == string::npos) {
				throw std::exception("Can't read board data.");
			}
			board_[p] = static_cast<Stone>(stone_type);
		}
		// Read turn-text
		auto turn_type = kStoneString.find_first_of(turn_text[0], 0);
		if (turn_type == string::npos) {
			throw std::exception("Can't read turn data.");
		}
		turn_ = static_cast<Stone>(turn_type);
		// Initialize IterateTable
		const int kTengen = kBoardSize / 2;
		for (size_t y = 0; y < kBoardSize; ++y) {
			for (size_t x = 0; x < kBoardSize; ++x) {
				auto p = ToPosition(x, y);
				kTenGenDist[p] = kTengen - std::max(std::abs(static_cast<int>(x) - kTengen), std::abs(static_cast<int>(y) - kTengen));
				// Row(─)
				kIterateTable[p][Direction::Row][Side::Right] = std::min(kBoardSize - x - 1, kSearchWidth);
				kIterateTable[p][Direction::Row][Side::Left ] = std::min(x, kSearchWidth);
				// Column(│)
				kIterateTable[p][Direction::Column][Side::Right] = std::min(kBoardSize - y - 1, kSearchWidth);
				kIterateTable[p][Direction::Column][Side::Left ] = std::min(y, kSearchWidth);
				// Diagonally right(／)
				kIterateTable[p][Direction::DiagR][Side::Right] = std::min(std::min(x, kBoardSize - y - 1), kSearchWidth);
				kIterateTable[p][Direction::DiagR][Side::Left ] = std::min(std::min(kBoardSize - x - 1, y), kSearchWidth);
				// Diagonally left(＼)
				kIterateTable[p][Direction::DiagL][Side::Right] = std::min(std::min(kBoardSize - x - 1, kBoardSize - y - 1), kSearchWidth);
				kIterateTable[p][Direction::DiagL][Side::Left ] = std::min(std::min(x, y), kSearchWidth);
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
		// Normal move search
		if (debug_flg) std::cerr << "Normal" << endl;
		result = FindNormalMove(depth, debug_flg);
		if (result.second) return result.first;
		if (result.first >= kBoardSize * kBoardSize) return -2;
		return -1;
	}
};

int main(int argc, char *argv[]) {
	// Read arguments
	if (argc < 4) {
		cout << "Usage: I401 board_text turn_text depth" << endl
			<< endl
			<< "board_text     : board data of stone or blank" << endl
			<< "turn_text      : turn data of stone or blank" << endl
			<< "depth          : depth of thinking" << endl
			<< "stone or blank : black(*), white(O), blank(-)" << endl
			<< "return value   : [move] >=0 | [can't move] -1 | [give up] -2" << endl;
		return -1;
	}
	try {
		Board board(argv[1], argv[2]);
		int depth = std::stoi(argv[3]);
		if (depth < 0) depth = 0;
		cout << board.NextMove(depth, (argc >= 5)) << endl;
	}
	catch (const std::exception& er) {
		std::cerr << er.what() << endl;
		cout << -1 << endl;
		return 1;
	}
	return 0;
}

/* 進捗：
・ベンチマーク問題(63449ms、gBが正着っぽい？)
h8,i7,i9,g9,g7,f6,h7,h9,i8,g8,g6,f5,j8,j9,hA,k7,f8,fA,f9,i6,iA,jA,f7,j7,j6,h6,gA,e8,e7,d7,kA,e6,g4,**,
---------------------------------------------------*-------------O-------------OO*OO*--------O****OOO--------O*O***----------*OO*O----------O***O*------------------------------------------------------------------------------- O
*/
