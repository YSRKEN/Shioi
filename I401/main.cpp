#include<algorithm>
#include<array>
#include<cstdint>
#include<iostream>
#include<random>
#include<string>
#include<tuple>
#include<vector>

// using declaration
using std::array;
using std::cout;
using std::endl;
using std::string;
using std::tuple;
using std::vector;

// const value declaration
const size_t kBoardSize = 15;
const size_t kSearchWidth = 5;
const int kScoreInf = 1000;
const int kScoreInf2 = kScoreInf + 1;
const string kPositionStringX = "abcdefghijklmno";
const string kPositionStringY = "123456789ABCDEF";
const size_t kPositionoffset[] = {1, kBoardSize, kBoardSize - 1, kBoardSize + 1};
enum Stone : uint8_t {
	None,
	Black,
	White,
	Other	//Outside of board
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
constexpr size_t PackPattern(const Stone s1, const Stone s2) {
	return (s1 << 2) + s2;
}
constexpr size_t PackPattern(const Stone s1, const Stone s2, const Stone s3) {
	return (s1 << 4) + (s2 << 2) + s3;
}
constexpr size_t PackPattern(const Stone s1, const Stone s2, const Stone s3, const Stone s4) {
	return (s1 << 6) + (s2 << 4) + (s3 << 2) + s4;
}
constexpr size_t PackPattern(const Stone s1, const Stone s2, const Stone s3, const Stone s4, const Stone s5) {
	return (s1 << 8) + (s2 << 6) + (s3 << 4) + (s4 << 2) + s5;
}
constexpr size_t PackPatternAdd(const size_t s1, const Stone s2) {
	return (s1 << 2) + s2;
}

// Board class
class Board {
	// values
	array<Stone, kBoardSize * kBoardSize> board_;
	Stone turn_;
	IterateTable kIterateTable;
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
	// Check GameEnd
	bool IsGameEnd(const Stone turn) {
		// Row(─)
		for (size_t i = 0; i < kBoardSize; ++i) {
			size_t len = 0;
			for (size_t j = 0; j < kBoardSize; ++j) {
				size_t p = ToPosition(j, i);
				if (board_[p] == turn) {
					++len;
					if (len == 5) return true;
				}else{
					len = 0;
				}
			}
		}
		// Column(│)
		for (size_t i = 0; i < kBoardSize; ++i) {
			size_t len = 0;
			for (size_t j = 0; j < kBoardSize; ++j) {
				size_t p = ToPosition(i, j);
				if (board_[p] == turn) {
					++len;
					if (len == 5) return true;
				}
				else {
					len = 0;
				}
			}
		}
		// Diagonally right(／)
		for (size_t i = 4; i < kBoardSize; ++i) {
			size_t len = 0;
			for (size_t j = 0; j <= i; ++j) {
				size_t p = ToPosition(i - j, j);
				if (board_[p] == turn) {
					++len;
					if (len == 5) return true;
				}
				else {
					len = 0;
				}
			}
		}
		for (size_t i = 1; i < kBoardSize - 4; ++i) {
			size_t len = 0;
			for (size_t j = 0; j < kBoardSize - i; ++j) {
				size_t p = ToPosition(kBoardSize - 1 - j, i + j);
				if (board_[p] == turn) {
					++len;
					if (len == 5) return true;
				}
				else {
					len = 0;
				}
			}
		}
		// Diagonally left(＼)
		for (size_t i = 0; i < kBoardSize - 4; ++i) {
			size_t len = 0;
			for (size_t j = 0; j < kBoardSize - i; ++j) {
				size_t p = ToPosition(i + j, j);
				if (board_[p] == turn) {
					++len;
					if (len == 5) return true;
				}
				else {
					len = 0;
				}
			}
		}
		for (size_t i = 1; i < kBoardSize - 4; ++i) {
			size_t len = 0;
			for (size_t j = 0; j < kBoardSize - i; ++j) {
				size_t p = ToPosition(j, i + j);
				if (board_[p] == turn) {
					++len;
					if (len == 5) return true;
				}
				else {
					len = 0;
				}
			}
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
			return Result(ToPosition(7, 7), true);
		}
		else if (count == 1) {
			if (board_[ToPosition(7, 7)] == Stone::Black) {
				const static size_t next_move[] = { ToPosition(7, 6), ToPosition(8, 6) };
				return Result(next_move[RandInt(2)], true);
			}
		}
		else if (count == 2) {
			if (board_[ToPosition(7, 7)] == Stone::Black) {
				// Ka-getsu
				if (board_[ToPosition(7, 6)] == Stone::White) return Result(ToPosition(8, 6), true);
				if (board_[ToPosition(8, 7)] == Stone::White) return Result(ToPosition(8, 8), true);
				if (board_[ToPosition(7, 8)] == Stone::White) return Result(ToPosition(6, 8), true);
				if (board_[ToPosition(6, 7)] == Stone::White) return Result(ToPosition(6, 6), true);
				// Ho-getsu
				if (board_[ToPosition(8, 6)] == Stone::White) return Result(ToPosition(8, 8), true);
				if (board_[ToPosition(8, 8)] == Stone::White) return Result(ToPosition(6, 8), true);
				if (board_[ToPosition(6, 8)] == Stone::White) return Result(ToPosition(6, 6), true);
				if (board_[ToPosition(6, 6)] == Stone::White) return Result(ToPosition(8, 6), true);
			}
		}
		return Result(0, false);
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
			const auto get_value = GetBoardValue(position, dir, count);
			return (get_value != Stone::Black ? Stone::Other : Stone::Black);
		};
		// Right pattern
		if (right_pattern_length == 0) {
			pattern[Side::Right][0] = Stone::Other;
			pattern[Side::Right][1] = PackPatternAdd(pattern[Side::Right][0], Stone::Other);
			pattern[Side::Right][2] = PackPatternAdd(pattern[Side::Right][1], Stone::Other);
			pattern[Side::Right][3] = PackPatternAdd(pattern[Side::Right][2], Stone::Other);
			pattern[Side::Right][4] = PackPatternAdd(pattern[Side::Right][3], Stone::Other);
		}
		else{
			pattern[Side::Right][0] = GetBoardValue2(position, dir, 1);
			if (right_pattern_length == 1) {
				pattern[Side::Right][1] = PackPatternAdd(pattern[Side::Right][0], Stone::Other);
				pattern[Side::Right][2] = PackPatternAdd(pattern[Side::Right][1], Stone::Other);
				pattern[Side::Right][3] = PackPatternAdd(pattern[Side::Right][2], Stone::Other);
				pattern[Side::Right][4] = PackPatternAdd(pattern[Side::Right][3], Stone::Other);
			}
			else{
				pattern[Side::Right][1] = PackPattern(GetBoardValue(position, dir, 1), GetBoardValue2(position, dir, 2));
				if (right_pattern_length == 2) {
					pattern[Side::Right][2] = PackPatternAdd(pattern[Side::Right][1], Stone::Other);
					pattern[Side::Right][3] = PackPatternAdd(pattern[Side::Right][2], Stone::Other);
					pattern[Side::Right][4] = PackPatternAdd(pattern[Side::Right][3], Stone::Other);
				}
				else{
					pattern[Side::Right][2] = PackPattern(GetBoardValue(position, dir, 1),
						GetBoardValue(position, dir, 2), GetBoardValue2(position, dir, 3));
					if (right_pattern_length == 3) {
						pattern[Side::Right][3] = PackPatternAdd(pattern[Side::Right][2], Stone::Other);
						pattern[Side::Right][4] = PackPatternAdd(pattern[Side::Right][3], Stone::Other);
					}
					else{
						pattern[Side::Right][3] = PackPattern(GetBoardValue(position, dir, 1), GetBoardValue(position, dir, 2),
							GetBoardValue(position, dir, 3), GetBoardValue2(position, dir, 4));
						if (right_pattern_length == 4) {
							pattern[Side::Right][4] = PackPatternAdd(pattern[Side::Right][3], Stone::Other);
						}
						else {
							pattern[Side::Right][4] = PackPattern(GetBoardValue(position, dir, 1), GetBoardValue(position, dir, 2),
								GetBoardValue(position, dir, 3), GetBoardValue(position, dir, 4), GetBoardValue2(position, dir, 5));
						}
					}
				}
			}
		}
		// Left pattern
		if (left_pattern_length == 0) {
			pattern[Side::Left][0] = Stone::Other;
			pattern[Side::Left][1] = PackPatternAdd(pattern[Side::Left][0], Stone::Other);
			pattern[Side::Left][2] = PackPatternAdd(pattern[Side::Left][1], Stone::Other);
			pattern[Side::Left][3] = PackPatternAdd(pattern[Side::Left][2], Stone::Other);
			pattern[Side::Left][4] = PackPatternAdd(pattern[Side::Left][3], Stone::Other);
		}
		else {
			pattern[Side::Left][0] = GetBoardValue2(position, dir, -1);
			if (left_pattern_length == 1) {
				pattern[Side::Left][1] = PackPatternAdd(pattern[Side::Left][0], Stone::Other);
				pattern[Side::Left][2] = PackPatternAdd(pattern[Side::Left][1], Stone::Other);
				pattern[Side::Left][3] = PackPatternAdd(pattern[Side::Left][2], Stone::Other);
				pattern[Side::Left][4] = PackPatternAdd(pattern[Side::Left][3], Stone::Other);
			}
			else {
				pattern[Side::Left][1] = PackPattern(GetBoardValue(position, dir, -1), GetBoardValue2(position, dir, -2));
				if (left_pattern_length == 2) {
					pattern[Side::Left][2] = PackPatternAdd(pattern[Side::Left][1], Stone::Other);
					pattern[Side::Left][3] = PackPatternAdd(pattern[Side::Left][2], Stone::Other);
					pattern[Side::Left][4] = PackPatternAdd(pattern[Side::Left][3], Stone::Other);
				}
				else {
					pattern[Side::Left][2] = PackPattern(GetBoardValue(position, dir, -1),
						GetBoardValue(position, dir, -2), GetBoardValue2(position, dir, -3));
					if (left_pattern_length == 3) {
						pattern[Side::Left][3] = PackPatternAdd(pattern[Side::Left][2], Stone::Other);
						pattern[Side::Left][4] = PackPatternAdd(pattern[Side::Left][3], Stone::Other);
					}
					else {
						pattern[Side::Left][3] = PackPattern(GetBoardValue(position, dir, -1), GetBoardValue(position, dir, -2),
							GetBoardValue(position, dir, -3), GetBoardValue2(position, dir, -4));
						if (left_pattern_length == 4) {
							pattern[Side::Left][4] = PackPatternAdd(pattern[Side::Left][3], Stone::Other);
						}
						else {
							pattern[Side::Left][4] = PackPattern(GetBoardValue(position, dir, -1), GetBoardValue(position, dir, -2),
								GetBoardValue(position, dir, -3), GetBoardValue(position, dir, -4), GetBoardValue2(position, dir, -5));
						}
					}
				}
			}
		}
		return pattern;
	}
	Pattern GetPatternW(const size_t position, const Direction dir) {
		const size_t right_pattern_length = kIterateTable[position][dir][Side::Right];
		const size_t  left_pattern_length = kIterateTable[position][dir][Side::Left];
		Pattern pattern;
		auto GetBoardValue = [this](size_t position, Direction dir, int count) -> Stone {return board_[position + kPositionoffset[dir] * count]; };
		pattern[Side::Right][0] = (right_pattern_length <= 0 ? Stone::Other
			: GetBoardValue(position, dir, 1));
		pattern[Side::Right][1] = (right_pattern_length <= 1 ? Stone::Other
			: PackPattern(GetBoardValue(position, dir, 1), GetBoardValue(position, dir, 2)));
		pattern[Side::Right][2] = (right_pattern_length <= 2 ? Stone::Other
			: PackPattern(GetBoardValue(position, dir, 1), GetBoardValue(position, dir, 2), GetBoardValue(position, dir, 3)));
		pattern[Side::Right][3] = (right_pattern_length <= 3 ? Stone::Other
			: PackPattern(GetBoardValue(position, dir, 1), GetBoardValue(position, dir, 2),
				GetBoardValue(position, dir, 3), GetBoardValue(position, dir, 4)));

		pattern[Side::Left][0] = (left_pattern_length <= 0 ? Stone::Other
			: GetBoardValue(position, dir, -1));
		pattern[Side::Left][1] = (left_pattern_length <= 1 ? Stone::Other
			: PackPattern(GetBoardValue(position, dir, -1), GetBoardValue(position, dir, -2)));
		pattern[Side::Left][2] = (left_pattern_length <= 2 ? Stone::Other
			: PackPattern(GetBoardValue(position, dir, -1), GetBoardValue(position, dir, -2), GetBoardValue(position, dir, -3)));
		pattern[Side::Left][3] = (left_pattern_length <= 3 ? Stone::Other
			: PackPattern(GetBoardValue(position, dir, -1), GetBoardValue(position, dir, -2),
				GetBoardValue(position, dir, -3), GetBoardValue(position, dir, -4)));
		return pattern;
	}
	// Check Cho-ren
	bool IsChorenB(const Pattern &pattern) {
		if (pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::Black, Stone::Black, Stone::Black, Stone::Black)) return true;
		if (pattern[Side::Right][3] == PackPattern(Stone::Black, Stone::Black, Stone::Black, Stone::Black)
			&& pattern[Side::Left][0] == Stone::Black) return true;
		if (pattern[Side::Right][2] == PackPattern(Stone::Black, Stone::Black, Stone::Black)
			&& pattern[Side::Left][1] == PackPattern(Stone::Black, Stone::Black)) return true;
		if (pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::Black, Stone::Black, Stone::Black, Stone::Black)) return true;
		if (pattern[Side::Left][3] == PackPattern(Stone::Black, Stone::Black, Stone::Black, Stone::Black)
			&& pattern[Side::Right][0] == Stone::Black) return true;
		if (pattern[Side::Left][2] == PackPattern(Stone::Black, Stone::Black, Stone::Black)
			&& pattern[Side::Right][1] == PackPattern(Stone::Black, Stone::Black)) return true;
		return false;
	}
	bool IsChorenW(const Pattern &pattern) {

		return false;
	}
	// Count Ren
	RenCount CountRenB(const Pattern &pattern, const size_t position, const Direction dir) {
		/* Count "Straight Shi-Shi"
		* 1. BO|BBB|OB   "Ryoutou-no-ShiShi"
		* 2. BBO|BB|OBB  "Chouda-no-ShiShi"
		* 3. BBBO|B|OBBB "Souryu-no-ShiShi"
		* 一直線上に出来る四々で、両頭の四々・長蛇の四々・双龍の四々
		*/
		{
			//BO|BBB|OB
			if (pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::Black, Stone::None)
				&& pattern[Side::Left][2] == PackPattern(Stone::None, Stone::Black, Stone::None)) return RenCount(0, 2, 0);
			if (pattern[Side::Right][3] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::None)
				&& pattern[Side::Left][3] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::None)) return RenCount(0, 2, 0);
			if (pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::Black, Stone::None)
				&& pattern[Side::Right][2] == PackPattern(Stone::None, Stone::Black, Stone::None)) return RenCount(0, 2, 0);
			//BBO|BB|OBB
			if (pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::Black, Stone::None)
				&& pattern[Side::Left][3] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 2, 0);
			if (pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::Black, Stone::None)
				&& pattern[Side::Right][3] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 2, 0);
			//BBBO|B|OBBB
			if (pattern[Side::Right][4] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::Black, Stone::None)
				&& pattern[Side::Left][4] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 2, 0);
		}

		// Count Shi-ren
		{
			if (pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::Black, Stone::Black, Stone::None, Stone::None)
				&& pattern[Side::Left][1] == PackPattern(Stone::None, Stone::None)) return RenCount(1, 0, 0);
			if (pattern[Side::Right][3] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::None)
				&& pattern[Side::Left][2] == PackPattern(Stone::Black, Stone::None, Stone::None)) return RenCount(1, 0, 0);
			if (pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::Black, Stone::Black, Stone::None, Stone::None)
				&& pattern[Side::Right][1] == PackPattern(Stone::None, Stone::None)) return RenCount(1, 0, 0);
			if (pattern[Side::Left][3] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::None)
				&& pattern[Side::Right][2] == PackPattern(Stone::Black, Stone::None, Stone::None)) return RenCount(1, 0, 0);
		}

		// Count Katsu-Shi
		{
			//11110
			if (pattern[Side::Right][1] == PackPattern(Stone::None, Stone::None)
				&& pattern[Side::Left][3] == PackPattern(Stone::Black, Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 1, 0);
			if (pattern[Side::Right][2] == PackPattern(Stone::Black, Stone::None, Stone::None)
				&& pattern[Side::Left][2] == PackPattern(Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 1, 0);
			if (pattern[Side::Right][3] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::None)
				&& pattern[Side::Left][1] == PackPattern(Stone::Black, Stone::None)) return RenCount(0, 1, 0);
			if (pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::Black, Stone::Black, Stone::None, Stone::None)
				&& pattern[Side::Left][0] == Stone::None) return RenCount(0, 1, 0);
			//11101
			if (pattern[Side::Right][0] == Stone::None
				&& pattern[Side::Left][4] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 1, 0);
			if (pattern[Side::Right][2] == PackPattern(Stone::None, Stone::Black, Stone::None)
				&& pattern[Side::Left][2] == PackPattern(Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 1, 0);
			if (pattern[Side::Right][3] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::None)
				&& pattern[Side::Left][1] == PackPattern(Stone::Black, Stone::None)) return RenCount(0, 1, 0);
			if (pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::Black, Stone::None)
				&& pattern[Side::Left][0] == Stone::None) return RenCount(0, 1, 0);
			//11011
			if (pattern[Side::Right][0] == Stone::None
				&& pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 1, 0);
			if (pattern[Side::Right][1] == PackPattern(Stone::Black, Stone::None)
				&& pattern[Side::Left][3] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 1, 0);
			if (pattern[Side::Left][0] == Stone::None
				&& pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 1, 0);
			if (pattern[Side::Left][1] == PackPattern(Stone::Black, Stone::None)
				&& pattern[Side::Right][3] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 1, 0);
			//10111
			if (pattern[Side::Left][0] == Stone::None
				&& pattern[Side::Right][4] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 1, 0);
			if (pattern[Side::Left][2] == PackPattern(Stone::None, Stone::Black, Stone::None)
				&& pattern[Side::Right][2] == PackPattern(Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 1, 0);
			if (pattern[Side::Left][3] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::None)
				&& pattern[Side::Right][1] == PackPattern(Stone::Black, Stone::None)) return RenCount(0, 1, 0);
			if (pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::Black, Stone::None)
				&& pattern[Side::Right][0] == Stone::None) return RenCount(0, 1, 0);
			//01111
			if (pattern[Side::Left][1] == PackPattern(Stone::None, Stone::None)
				&& pattern[Side::Right][3] == PackPattern(Stone::Black, Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 1, 0);
			if (pattern[Side::Left][2] == PackPattern(Stone::Black, Stone::None, Stone::None)
				&& pattern[Side::Right][2] == PackPattern(Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 1, 0);
			if (pattern[Side::Left][3] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::None)
				&& pattern[Side::Right][1] == PackPattern(Stone::Black, Stone::None)) return RenCount(0, 1, 0);
			if (pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::Black, Stone::Black, Stone::None, Stone::None)
				&& pattern[Side::Right][0] == Stone::None) return RenCount(0, 1, 0);
		}

		// Count Katsu-San
		{
			int offset1 = 0, offset2 = 0;
			while (true) {
				//011100
				if (pattern[Side::Right][2] == PackPattern(Stone::None, Stone::None, Stone::None)
					&& pattern[Side::Left][3] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::None)) {
					offset1 = 1; offset2 = -3; break;
				}
				if (pattern[Side::Right][3] == PackPattern(Stone::Black, Stone::None, Stone::None, Stone::None)
					&& pattern[Side::Left][2] == PackPattern(Stone::Black, Stone::None, Stone::None)) {
					offset1 = 2; offset2 = -2; break;
				}
				if (pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::None, Stone::None)
					&& pattern[Side::Left][1] == PackPattern(Stone::None, Stone::None)) {
					offset1 = 3; offset2 = -1; break;
				}
				//011010
				if (pattern[Side::Right][1] == PackPattern(Stone::None, Stone::None)
					&& pattern[Side::Left][4] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::None, Stone::None)) {
					offset1 = -1; break;
				}
				if (pattern[Side::Right][3] == PackPattern(Stone::None, Stone::Black, Stone::None, Stone::None)
					&& pattern[Side::Left][2] == PackPattern(Stone::Black, Stone::None, Stone::None)) {
					offset1 = 1; break;
				}
				if (pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::None, Stone::None)
					&& pattern[Side::Left][1] == PackPattern(Stone::None, Stone::None)) {
					offset1 = 2; break;
				}
				//010110
				if (pattern[Side::Left][1] == PackPattern(Stone::None, Stone::None)
					&& pattern[Side::Right][4] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::None, Stone::None)) {
					offset1 = 1; break;
				}
				if (pattern[Side::Left][3] == PackPattern(Stone::None, Stone::Black, Stone::None, Stone::None)
					&& pattern[Side::Right][2] == PackPattern(Stone::Black, Stone::None, Stone::None)) {
					offset1 = -1; break;
				}
				if (pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::None, Stone::None)
					&& pattern[Side::Right][1] == PackPattern(Stone::None, Stone::None)) {
					offset1 = -2; break;
				}
				//001110
				if (pattern[Side::Left][2] == PackPattern(Stone::None, Stone::None, Stone::None)
					&& pattern[Side::Right][3] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::None)) {
					offset1 = -1; offset2 = 3; break;
				}
				if (pattern[Side::Left][3] == PackPattern(Stone::Black, Stone::None, Stone::None, Stone::None)
					&& pattern[Side::Right][2] == PackPattern(Stone::Black, Stone::None, Stone::None)) {
					offset1 = -2; offset2 = 2; break;
				}
				if (pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::None, Stone::None)
					&& pattern[Side::Right][1] == PackPattern(Stone::None, Stone::None)) {
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
	RenCount CountRenW(const Pattern &pattern) {
		/* Count "Straight Shi-Shi"
		* 1. BO|BBB|OB   "Ryoutou-no-ShiShi"
		* 2. BBO|BB|OBB  "Chouda-no-ShiShi"
		* 3. BBBO|B|OBBB "Souryu-no-ShiShi"
		* 一直線上に出来る四々で、両頭の四々・長蛇の四々・双龍の四々
		*/
		if (pattern[Side::Right][3] == PackPattern(Stone::White, Stone::White, Stone::None, Stone::White)
			&& pattern[Side::Left][1] == PackPattern(Stone::None, Stone::White)) return RenCount(0, 2, 0);
		if (pattern[Side::Right][2] == PackPattern(Stone::White, Stone::None, Stone::White)
			&& pattern[Side::Left][2] == PackPattern(Stone::White, Stone::None, Stone::White)) return RenCount(0, 2, 0);
		if (pattern[Side::Left][3] == PackPattern(Stone::White, Stone::White, Stone::None, Stone::White)
			&& pattern[Side::Right][1] == PackPattern(Stone::None, Stone::White)) return RenCount(0, 2, 0);
		//
		if (pattern[Side::Right][3] == PackPattern(Stone::White, Stone::None, Stone::White, Stone::White)
			&& pattern[Side::Left][2] == PackPattern(Stone::None, Stone::White, Stone::White)) return RenCount(0, 2, 0);
		if (pattern[Side::Left][3] == PackPattern(Stone::White, Stone::None, Stone::White, Stone::White)
			&& pattern[Side::Right][2] == PackPattern(Stone::None, Stone::White, Stone::White)) return RenCount(0, 2, 0);
		//
		if (pattern[Side::Right][3] == PackPattern(Stone::None, Stone::White, Stone::White, Stone::White)
			&& pattern[Side::Left][3] == PackPattern(Stone::None, Stone::White, Stone::White, Stone::White)) return RenCount(0, 2, 0);

		// Count Shi-ren
		if (pattern[Side::Right][3] == PackPattern(Stone::White, Stone::White, Stone::White, Stone::None)
			&& pattern[Side::Left][0] == Stone::None) return RenCount(1, 0, 0);
		if (pattern[Side::Right][2] == PackPattern(Stone::White, Stone::White, Stone::None)
			&& pattern[Side::Left][1] == PackPattern(Stone::White, Stone::None)) return RenCount(1, 0, 0);
		if (pattern[Side::Left][3] == PackPattern(Stone::White, Stone::White, Stone::White, Stone::None)
			&& pattern[Side::Right][0] == Stone::None) return RenCount(1, 0, 0);
		if (pattern[Side::Left][2] == PackPattern(Stone::White, Stone::White, Stone::None)
			&& pattern[Side::Right][1] == PackPattern(Stone::White, Stone::None)) return RenCount(1, 0, 0);

		// Count Katsu-Shi
		//11110
		if (pattern[Side::Right][0] == Stone::None
			&& pattern[Side::Left][2] == PackPattern(Stone::White, Stone::White, Stone::White)) return RenCount(1, 0, 0);
		if (pattern[Side::Right][1] == PackPattern(Stone::White, Stone::None)
			&& pattern[Side::Left][1] == PackPattern(Stone::White, Stone::White)) return RenCount(1, 0, 0);
		if (pattern[Side::Right][2] == PackPattern(Stone::White, Stone::White, Stone::None)
			&& pattern[Side::Left][0] == Stone::White) return RenCount(1, 0, 0);
		if (pattern[Side::Right][3] == PackPattern(Stone::White, Stone::White, Stone::White, Stone::None)) return RenCount(1, 0, 0);
		//11101
		if (pattern[Side::Left][3] == PackPattern(Stone::None, Stone::White, Stone::White, Stone::White)) return RenCount(1, 0, 0);
		if (pattern[Side::Right][1] == PackPattern(Stone::None, Stone::White)
			&& pattern[Side::Left][1] == PackPattern(Stone::White, Stone::White)) return RenCount(1, 0, 0);
		if (pattern[Side::Right][2] == PackPattern(Stone::White, Stone::None, Stone::White)
			&& pattern[Side::Left][0] == Stone::White) return RenCount(1, 0, 0);
		if (pattern[Side::Right][3] == PackPattern(Stone::White, Stone::White, Stone::None, Stone::White)) return RenCount(1, 0, 0);
		//11011
		if (pattern[Side::Left][3] == PackPattern(Stone::White, Stone::None, Stone::White, Stone::White)) return RenCount(1, 0, 0);
		if (pattern[Side::Right][0] == Stone::White
			&& pattern[Side::Left][2] == PackPattern(Stone::None, Stone::White, Stone::White)) return RenCount(1, 0, 0);
		if (pattern[Side::Right][3] == PackPattern(Stone::White, Stone::None, Stone::White, Stone::White)) return RenCount(1, 0, 0);
		if (pattern[Side::Left][0] == Stone::White
			&& pattern[Side::Right][2] == PackPattern(Stone::None, Stone::White, Stone::White)) return RenCount(1, 0, 0);
		//10111
		if (pattern[Side::Right][3] == PackPattern(Stone::None, Stone::White, Stone::White, Stone::White)) return RenCount(1, 0, 0);
		if (pattern[Side::Left][1] == PackPattern(Stone::None, Stone::White)
			&& pattern[Side::Right][1] == PackPattern(Stone::White, Stone::White)) return RenCount(1, 0, 0);
		if (pattern[Side::Left][2] == PackPattern(Stone::White, Stone::None, Stone::White)
			&& pattern[Side::Right][0] == Stone::White) return RenCount(1, 0, 0);
		if (pattern[Side::Left][3] == PackPattern(Stone::White, Stone::White, Stone::None, Stone::White)) return RenCount(1, 0, 0);
		//01111
		if (pattern[Side::Left][0] == Stone::None
			&& pattern[Side::Right][2] == PackPattern(Stone::White, Stone::White, Stone::White)) return RenCount(1, 0, 0);
		if (pattern[Side::Left][1] == PackPattern(Stone::White, Stone::None)
			&& pattern[Side::Right][1] == PackPattern(Stone::White, Stone::White)) return RenCount(1, 0, 0);
		if (pattern[Side::Left][2] == PackPattern(Stone::White, Stone::White, Stone::None)
			&& pattern[Side::Right][0] == Stone::White) return RenCount(1, 0, 0);
		if (pattern[Side::Left][3] == PackPattern(Stone::White, Stone::White, Stone::White, Stone::None)) return RenCount(1, 0, 0);

		// Count Katsu-san
		//011100
		if (pattern[Side::Right][1] == PackPattern(Stone::None, Stone::None)
			&& pattern[Side::Left][2] == PackPattern(Stone::White, Stone::White, Stone::None)) return RenCount(0, 0, 1);
		if (pattern[Side::Right][2] == PackPattern(Stone::White, Stone::None, Stone::None)
			&& pattern[Side::Left][1] == PackPattern(Stone::White, Stone::None)) return RenCount(0, 0, 1);
		if (pattern[Side::Right][3] == PackPattern(Stone::White, Stone::White, Stone::None, Stone::None)
			&& pattern[Side::Left][0] == Stone::None) return RenCount(0, 0, 1);
		//011010
		if (pattern[Side::Right][0] == Stone::None
			&& pattern[Side::Left][3] == PackPattern(Stone::None, Stone::White, Stone::White, Stone::None)) return RenCount(0, 0, 1);
		if (pattern[Side::Right][2] == PackPattern(Stone::None, Stone::White, Stone::None)
			&& pattern[Side::Left][1] == PackPattern(Stone::White, Stone::None)) return RenCount(0, 0, 1);
		if (pattern[Side::Right][3] == PackPattern(Stone::White, Stone::None, Stone::White, Stone::None)
			&& pattern[Side::Left][0] == Stone::None) return RenCount(0, 0, 1);
		//010110
		if (pattern[Side::Left][0] == Stone::None
			&& pattern[Side::Right][3] == PackPattern(Stone::None, Stone::White, Stone::White, Stone::None)) return RenCount(0, 0, 1);
		if (pattern[Side::Left][2] == PackPattern(Stone::None, Stone::White, Stone::None)
			&& pattern[Side::Right][1] == PackPattern(Stone::White, Stone::None)) return RenCount(0, 0, 1);
		if (pattern[Side::Left][3] == PackPattern(Stone::White, Stone::None, Stone::White, Stone::None)
			&& pattern[Side::Right][0] == Stone::None) return RenCount(0, 0, 1);
		//001110
		if (pattern[Side::Left][1] == PackPattern(Stone::None, Stone::None)
			&& pattern[Side::Right][2] == PackPattern(Stone::White, Stone::White, Stone::None)) return RenCount(0, 0, 1);
		if (pattern[Side::Left][2] == PackPattern(Stone::White, Stone::None, Stone::None)
			&& pattern[Side::Right][1] == PackPattern(Stone::White, Stone::None)) return RenCount(0, 0, 1);
		if (pattern[Side::Left][3] == PackPattern(Stone::White, Stone::White, Stone::None, Stone::None)
			&& pattern[Side::Right][0] == Stone::None) return RenCount(0, 0, 1);

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
			if (pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::Black, Stone::None)
				&& pattern[Side::Left][2] == PackPattern(Stone::None, Stone::Black, Stone::None)) return RenCount(0, 2, 0);
			if (pattern[Side::Right][3] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::None)
				&& pattern[Side::Left][3] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::None)) return RenCount(0, 2, 0);
			if (pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::Black, Stone::None)
				&& pattern[Side::Right][2] == PackPattern(Stone::None, Stone::Black, Stone::None)) return RenCount(0, 2, 0);
			//BBO|BB|OBB
			if (pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::Black, Stone::None)
				&& pattern[Side::Left][3] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 2, 0);
			if (pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::Black, Stone::None)
				&& pattern[Side::Right][3] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 2, 0);
			//BBBO|B|OBBB
			if (pattern[Side::Right][4] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::Black, Stone::None)
				&& pattern[Side::Left][4] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::Black, Stone::None)) return RenCount(0, 2, 0);
		}

		// Count Shi-ren
		{
			if (pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::Black, Stone::Black, Stone::None, Stone::Other)
				&& pattern[Side::Left][1] == PackPattern(Stone::None, Stone::Other)) return RenCount(1, 0, 0);
			if (pattern[Side::Right][3] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::Other)
				&& pattern[Side::Left][2] == PackPattern(Stone::Black, Stone::None, Stone::Other)) return RenCount(1, 0, 0);
			if (pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::Black, Stone::Black, Stone::None, Stone::Other)
				&& pattern[Side::Right][1] == PackPattern(Stone::None, Stone::Other)) return RenCount(1, 0, 0);
			if (pattern[Side::Left][3] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::Other)
				&& pattern[Side::Right][2] == PackPattern(Stone::Black, Stone::None, Stone::Other)) return RenCount(1, 0, 0);
		}

		// Count Katsu-Shi
		auto GetPosition = [&position, &dir](int count) -> int {return position + kPositionoffset[dir] * count; };
		{
			//11110
			if (pattern[Side::Right][1] == PackPattern(Stone::None, Stone::Other)
				&& pattern[Side::Left][3] == PackPattern(Stone::Black, Stone::Black, Stone::Black, Stone::Other)) {
				int bp = GetPosition(1);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (pattern[Side::Right][2] == PackPattern(Stone::Black, Stone::None, Stone::Other)
				&& pattern[Side::Left][2] == PackPattern(Stone::Black, Stone::Black, Stone::Other)) {
				int bp = GetPosition(2);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (pattern[Side::Right][3] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::Other)
				&& pattern[Side::Left][1] == PackPattern(Stone::Black, Stone::Other)) {
				int bp = GetPosition(3);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::Black, Stone::Black, Stone::None, Stone::Other)
				&& pattern[Side::Left][0] == Stone::Other) {
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
			if (pattern[Side::Right][0] == Stone::Other
				&& pattern[Side::Left][4] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::Black, Stone::Other)) {
				int bp = GetPosition(-1);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (pattern[Side::Right][2] == PackPattern(Stone::None, Stone::Black, Stone::Other)
				&& pattern[Side::Left][2] == PackPattern(Stone::Black, Stone::Black, Stone::Other)) {
				int bp = GetPosition(1);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (pattern[Side::Right][3] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::Other)
				&& pattern[Side::Left][1] == PackPattern(Stone::Black, Stone::Other)) {
				int bp = GetPosition(2);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::Black, Stone::Other)
				&& pattern[Side::Left][0] == Stone::Other) {
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
			if (pattern[Side::Right][0] == Stone::Other
				&& pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::Black, Stone::Other)) {
				int bp = GetPosition(-2);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (pattern[Side::Right][1] == PackPattern(Stone::Black, Stone::Other)
				&& pattern[Side::Left][3] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::Other)) {
				int bp = GetPosition(-1);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (pattern[Side::Left][0] == Stone::Other
				&& pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::Black, Stone::Other)) {
				int bp = GetPosition(2);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (pattern[Side::Left][1] == PackPattern(Stone::Black, Stone::Other)
				&& pattern[Side::Right][3] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::Other)) {
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
			if (pattern[Side::Left][0] == Stone::Other
				&& pattern[Side::Right][4] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::Black, Stone::Other)) {
				int bp = GetPosition(1);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (pattern[Side::Left][2] == PackPattern(Stone::None, Stone::Black, Stone::Other)
				&& pattern[Side::Right][2] == PackPattern(Stone::Black, Stone::Black, Stone::Other)) {
				int bp = GetPosition(-1);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (pattern[Side::Left][3] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::Other)
				&& pattern[Side::Right][1] == PackPattern(Stone::Black, Stone::Other)) {
				int bp = GetPosition(-2);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::Black, Stone::Other)
				&& pattern[Side::Right][0] == Stone::Other) {
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
			if (pattern[Side::Left][1] == PackPattern(Stone::None, Stone::Other)
				&& pattern[Side::Right][3] == PackPattern(Stone::Black, Stone::Black, Stone::Black, Stone::Other)) {
				int bp = GetPosition(-1);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (pattern[Side::Left][2] == PackPattern(Stone::Black, Stone::None, Stone::Other)
				&& pattern[Side::Right][2] == PackPattern(Stone::Black, Stone::Black, Stone::Other)) {
				int bp = GetPosition(-2);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (pattern[Side::Left][3] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::Other)
				&& pattern[Side::Right][1] == PackPattern(Stone::Black, Stone::Other)) {
				int bp = GetPosition(-3);
				if (bp >= 0 && bp < kBoardSize * kBoardSize) {
					block_position = bp;
				}
				else {
					block_position = kBoardSize * kBoardSize;
				}
				return RenCount(0, 1, 0);
			}
			if (pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::Black, Stone::Black, Stone::None, Stone::Other)
				&& pattern[Side::Right][0] == Stone::Other) {
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
				if (pattern[Side::Right][2] == PackPattern(Stone::None, Stone::None, Stone::None)
					&& pattern[Side::Left][3] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::None)) {
					offset1 = 1; offset2 = -3; break;
				}
				if (pattern[Side::Right][3] == PackPattern(Stone::Black, Stone::None, Stone::None, Stone::None)
					&& pattern[Side::Left][2] == PackPattern(Stone::Black, Stone::None, Stone::None)) {
					offset1 = 2; offset2 = -2; break;
				}
				if (pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::None, Stone::None)
					&& pattern[Side::Left][1] == PackPattern(Stone::None, Stone::None)) {
					offset1 = 3; offset2 = -1; break;
				}
				//011010
				if (pattern[Side::Right][1] == PackPattern(Stone::None, Stone::None)
					&& pattern[Side::Left][4] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::None, Stone::None)) {
					offset1 = -1; break;
				}
				if (pattern[Side::Right][3] == PackPattern(Stone::None, Stone::Black, Stone::None, Stone::None)
					&& pattern[Side::Left][2] == PackPattern(Stone::Black, Stone::None, Stone::None)) {
					offset1 = 1; break;
				}
				if (pattern[Side::Right][4] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::None, Stone::None)
					&& pattern[Side::Left][1] == PackPattern(Stone::None, Stone::None)) {
					offset1 = 2; break;
				}
				//010110
				if (pattern[Side::Left][1] == PackPattern(Stone::None, Stone::None)
					&& pattern[Side::Right][4] == PackPattern(Stone::None, Stone::Black, Stone::Black, Stone::None, Stone::None)) {
					offset1 = 1; break;
				}
				if (pattern[Side::Left][3] == PackPattern(Stone::None, Stone::Black, Stone::None, Stone::None)
					&& pattern[Side::Right][2] == PackPattern(Stone::Black, Stone::None, Stone::None)) {
					offset1 = -1; break;
				}
				if (pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::None, Stone::Black, Stone::None, Stone::None)
					&& pattern[Side::Right][1] == PackPattern(Stone::None, Stone::None)) {
					offset1 = -2; break;
				}
				//001110
				if (pattern[Side::Left][2] == PackPattern(Stone::None, Stone::None, Stone::None)
					&& pattern[Side::Right][3] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::None)) {
					offset1 = -1; offset2 = 3; break;
				}
				if (pattern[Side::Left][3] == PackPattern(Stone::Black, Stone::None, Stone::None, Stone::None)
					&& pattern[Side::Right][2] == PackPattern(Stone::Black, Stone::None, Stone::None)) {
					offset1 = -2; offset2 = 2; break;
				}
				if (pattern[Side::Left][4] == PackPattern(Stone::Black, Stone::Black, Stone::None, Stone::None, Stone::None)
					&& pattern[Side::Right][1] == PackPattern(Stone::None, Stone::None)) {
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
		if (pattern[Side::Right][3] == PackPattern(Stone::White, Stone::White, Stone::None, Stone::White)
			&& pattern[Side::Left][1] == PackPattern(Stone::None, Stone::White)) return RenCount(0, 2, 0);
		if (pattern[Side::Right][2] == PackPattern(Stone::White, Stone::None, Stone::White)
			&& pattern[Side::Left][2] == PackPattern(Stone::White, Stone::None, Stone::White)) return RenCount(0, 2, 0);
		if (pattern[Side::Left][3] == PackPattern(Stone::White, Stone::White, Stone::None, Stone::White)
			&& pattern[Side::Right][1] == PackPattern(Stone::None, Stone::White)) return RenCount(0, 2, 0);
		//
		if (pattern[Side::Right][3] == PackPattern(Stone::White, Stone::None, Stone::White, Stone::White)
			&& pattern[Side::Left][2] == PackPattern(Stone::None, Stone::White, Stone::White)) return RenCount(0, 2, 0);
		if (pattern[Side::Left][3] == PackPattern(Stone::White, Stone::None, Stone::White, Stone::White)
			&& pattern[Side::Right][2] == PackPattern(Stone::None, Stone::White, Stone::White)) return RenCount(0, 2, 0);
		//
		if (pattern[Side::Right][3] == PackPattern(Stone::None, Stone::White, Stone::White, Stone::White)
			&& pattern[Side::Left][3] == PackPattern(Stone::None, Stone::White, Stone::White, Stone::White)) return RenCount(0, 2, 0);

		// Count Shi-ren
		if (pattern[Side::Right][3] == PackPattern(Stone::White, Stone::White, Stone::White, Stone::None)
			&& pattern[Side::Left][0] == Stone::None) return RenCount(1, 0, 0);
		if (pattern[Side::Right][2] == PackPattern(Stone::White, Stone::White, Stone::None)
			&& pattern[Side::Left][1] == PackPattern(Stone::White, Stone::None)) return RenCount(1, 0, 0);
		if (pattern[Side::Left][3] == PackPattern(Stone::White, Stone::White, Stone::White, Stone::None)
			&& pattern[Side::Right][0] == Stone::None) return RenCount(1, 0, 0);
		if (pattern[Side::Left][2] == PackPattern(Stone::White, Stone::White, Stone::None)
			&& pattern[Side::Right][1] == PackPattern(Stone::White, Stone::None)) return RenCount(1, 0, 0);

		// Count Katsu-Shi
		auto GetPosition = [&position, &dir](int count) -> size_t {return position + kPositionoffset[dir] * count; };
		//11110
		if (pattern[Side::Right][0] == Stone::None
			&& pattern[Side::Left][2] == PackPattern(Stone::White, Stone::White, Stone::White)) return RenCount(1, 0, GetPosition(1));
		if (pattern[Side::Right][1] == PackPattern(Stone::White, Stone::None)
			&& pattern[Side::Left][1] == PackPattern(Stone::White, Stone::White)) return RenCount(1, 0, GetPosition(2));
		if (pattern[Side::Right][2] == PackPattern(Stone::White, Stone::White, Stone::None)
			&& pattern[Side::Left][0] == Stone::White) return RenCount(1, 0, GetPosition(3));
		if (pattern[Side::Right][3] == PackPattern(Stone::White, Stone::White, Stone::White, Stone::None)) return RenCount(1, 0, GetPosition(4));
		//11101
		if (pattern[Side::Left][3] == PackPattern(Stone::None, Stone::White, Stone::White, Stone::White)) return RenCount(1, 0, GetPosition(-1));
		if (pattern[Side::Right][1] == PackPattern(Stone::None, Stone::White)
			&& pattern[Side::Left][1] == PackPattern(Stone::White, Stone::White)) return RenCount(1, 0, GetPosition(1));
		if (pattern[Side::Right][2] == PackPattern(Stone::White, Stone::None, Stone::White)
			&& pattern[Side::Left][0] == Stone::White) return RenCount(1, 0, GetPosition(2));
		if (pattern[Side::Right][3] == PackPattern(Stone::White, Stone::White, Stone::None, Stone::White)) return RenCount(1, 0, GetPosition(3));
		//11011
		if (pattern[Side::Left][3] == PackPattern(Stone::White, Stone::None, Stone::White, Stone::White)) return RenCount(1, 0, GetPosition(-2));
		if (pattern[Side::Right][0] == Stone::White
			&& pattern[Side::Left][2] == PackPattern(Stone::None, Stone::White, Stone::White)) return RenCount(1, 0, GetPosition(-1));
		if (pattern[Side::Right][3] == PackPattern(Stone::White, Stone::None, Stone::White, Stone::White)) return RenCount(1, 0, GetPosition(2));
		if (pattern[Side::Left][0] == Stone::White
			&& pattern[Side::Right][2] == PackPattern(Stone::None, Stone::White, Stone::White)) return RenCount(1, 0, GetPosition(1));
		//10111
		if (pattern[Side::Right][3] == PackPattern(Stone::None, Stone::White, Stone::White, Stone::White)) return RenCount(1, 0, GetPosition(1));
		if (pattern[Side::Left][1] == PackPattern(Stone::None, Stone::White)
			&& pattern[Side::Right][1] == PackPattern(Stone::White, Stone::White)) return RenCount(1, 0, GetPosition(-1));
		if (pattern[Side::Left][2] == PackPattern(Stone::White, Stone::None, Stone::White)
			&& pattern[Side::Right][0] == Stone::White) return RenCount(1, 0, GetPosition(-2));
		if (pattern[Side::Left][3] == PackPattern(Stone::White, Stone::White, Stone::None, Stone::White)) return RenCount(1, 0, GetPosition(-3));
		//01111
		if (pattern[Side::Left][0] == Stone::None
			&& pattern[Side::Right][2] == PackPattern(Stone::White, Stone::White, Stone::White)) return RenCount(1, 0, GetPosition(-1));
		if (pattern[Side::Left][1] == PackPattern(Stone::White, Stone::None)
			&& pattern[Side::Right][1] == PackPattern(Stone::White, Stone::White)) return RenCount(1, 0, GetPosition(-2));
		if (pattern[Side::Left][2] == PackPattern(Stone::White, Stone::White, Stone::None)
			&& pattern[Side::Right][0] == Stone::White) return RenCount(1, 0, GetPosition(-3));
		if (pattern[Side::Left][3] == PackPattern(Stone::White, Stone::White, Stone::White, Stone::None)) return RenCount(1, 0, GetPosition(-4));
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
		for (size_t p = 0; p < kBoardSize * kBoardSize; ++p) {
			if (board_[p] != Stone::None) continue;
			if (turn_ == Stone::Black) {
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
				if (p == 4*15+1) {
					auto move_pattern1 = GetPatternB(p, static_cast<Direction>(0));
					auto move_pattern2 = GetPatternB(p, static_cast<Direction>(1));
					auto move_pattern3 = GetPatternB(p, static_cast<Direction>(2));
					auto move_pattern4 = GetPatternB(p, static_cast<Direction>(3));
					continue;
				}
				if (sum_4_strong + sum_4_normal > 0) {
					cout << PositionToString(p) << " " << sum_4_strong << " " << sum_4_normal << endl;
				}
				if (cho_ren_flg || sum_4_strong + sum_4_normal >= 2 || sum_3 >= 2) continue;
				if (sum_4_strong > 0) return Result(p, true);
				if (sum_4_normal == 1) {
					board_[p] = Stone::Black;
					auto score = IsShioiMove(Stone::Black, block_position, depth);
					board_[p] = Stone::None;
					if(score) return Result(p, true);
					continue;
				}
			}else{
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
		for (size_t y = 0; y < kBoardSize; ++y) {
			for (size_t x = 0; x < kBoardSize; ++x) {
				auto p = ToPosition(x, y);
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
		auto result = GetOpeningMove(turn_);
		if (result.second) return result.first;
		// If you can make Go-ren, you must do.
		result = FindGorenMove(turn_);
		if (result.second) return result.first;
		// If enemy can make Go-ren, you must block it.
		result = FindGorenMove(EnemyTurn(turn_));
		if (result.second) return result.first;
		// Search of quadruplex's problem(Shioi Tsume)
		result = FindShioiMove(EnemyTurn(turn_), 225);
		if (result.second) return result.first;
		// Random move(test code)
		std::cerr << "Random" << endl;
		result = FindRandomMove(EnemyTurn(turn_));
		if (result.second) return result.first;
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

/* 既出の不具合：
ここで黒が即座に棒四(b5, 61)にしない
-------------------*-------------*-------------*-----------------------------------------------O-----------------O------------------------------O-------------------------------------------------------------------------------- *
*/
