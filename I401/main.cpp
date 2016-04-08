#include<algorithm>
#include<array>
#include<cmath>
#include<cstdint>
#include<iostream>
#include<random>
#include<string>
#include<tuple>
#include<vector>

using std::array;
using std::cout;
using std::endl;
using std::string;
using std::tuple;
using std::vector;

typedef tuple<size_t, int> Score;

std::random_device rd;
std::mt19937 mt(rd());

const size_t kBoardSize = 15;
const string kStoneString = "-*O";
const int kScoreInf = 100;
const int kScoreProhibit = -kScoreInf + 1;

enum Stone : uint8_t {
	None,
	Black,
	White,
	UnBlack
};

bool MatchArray(const array<Stone, kBoardSize> &pattern, const vector<Stone> &match_pattern) {
	for (size_t i = 0; i < match_pattern.size(); ++i) {
		auto &it = match_pattern[i];
		if (it == Stone::UnBlack) {
			if (pattern[i] == Stone::Black) return false;
		}
		else if (it != pattern[i]) return false;
	}
	return true;
}
bool HasGoren(const array<Stone, kBoardSize> &pattern, const Stone turn) {
	size_t len = 0;
	for (auto &it : pattern) {
		if (it == turn) {
			++len;
		}
		else {
			if (len == 5) return true;
			len = 0;
		}
	}
	return (len == 5);
}
bool HasGoren2(const array<Stone, kBoardSize> &pattern, const Stone turn) {
	size_t len = 0;
	for (auto &it : pattern) {
		if (it == turn) {
			++len;
		}
		else {
			if (len >= 5) return true;
			len = 0;
		}
	}
	return (len >= 5);
}
bool HasChoren(const array<Stone, kBoardSize> &pattern, const Stone turn) {
	size_t len = 0;
	for (auto &it : pattern) {
		if (it == turn) {
			++len;
		}
		else {
			if (len >= 6) return true;
			len = 0;
		}
	}
	return (len >= 6);
}
string PositionToString(const size_t p) {
	const static string kPositionString = "abcdefghijklmno";
	return kPositionString.substr(p % kBoardSize, 1) + std::to_string(p / kBoardSize + 1);
}
Stone ReverseTurn(const Stone turn) {
	return (turn == Stone::Black ? Stone::White : Stone::Black);
}
size_t RandInt(const size_t n) {
	return std::uniform_int_distribution<size_t>{0, n - 1}(mt);
}
inline constexpr size_t Ternary(const Stone s1) noexcept {
	return s1;
}
inline constexpr size_t Ternary(const Stone s1, const Stone s2) noexcept{
	return s1 * 3 + s2;
}
inline constexpr size_t Ternary(const Stone s1, const Stone s2, const Stone s3) noexcept {
	return s1 * 9 + s2 * 3 + s3;
}
inline constexpr size_t Ternary(const Stone s1, const Stone s2, const Stone s3, const Stone s4) noexcept {
	return s1 * 27 + s2 * 9 + s3 * 3 + s4;
}
inline constexpr size_t Ternary(const Stone s1, const Stone s2, const Stone s3, const Stone s4, const Stone s5) noexcept {
	return s1 * 81 + s2 * 27 + s3 * 9 + s4 * 3 + s5;
}

class Point {
	int x_, y_;
public:
	Point() {}
	Point(const int x, const int y) {
		x_ = x;
		y_ = y;
	}
	Point(const size_t p) {
		x_ = p % kBoardSize;
		y_ = p / kBoardSize;
	}
	bool IsInBoard() const noexcept {
		if (x_ < 0 || x_ >= kBoardSize || y_ < 0 || y_ >= kBoardSize) return false;
		return true;
	}
	size_t GetPos() const noexcept {
		return x_ + y_ * kBoardSize;
	}
	size_t TenGenDist() const noexcept {
		const int kTengen = kBoardSize / 2;
		return kTengen - std::max(abs(x_ - kTengen), abs(y_ - kTengen));
	}
	Point operator +(const Point &offset) const {
		return Point(x_ + offset.x_, y_ + offset.y_);
	}
	Point operator -(const Point &offset) const {
		return Point(x_ - offset.x_, y_ - offset.y_);
	}
	Point operator *(const int n) const {
		return Point(x_ * n, y_ * n);
	}
	Point& operator +=(const Point &offset) noexcept {
		x_ += offset.x_;
		y_ += offset.y_;
		return *this;
	}
	Point& operator -=(const Point &offset) noexcept {
		x_ -= offset.x_;
		y_ -= offset.y_;
		return *this;
	}
};

class Board {
	array<Stone, kBoardSize * kBoardSize> board_;
	Stone turn_;
public:
	Board(const char board_text[], const char turn_text[]) {
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
	}
	size_t CountStone() const noexcept {
		size_t sum = 0;
		for (size_t p = 0; p < kBoardSize * kBoardSize; ++p) {
			if (board_[p] != Stone::None) ++sum;
		}
		return sum;
	}
	string PutBoard() const {
		string output;
		output += kStoneString[turn_];
		output += "\n";
		for (size_t y = 0; y < kBoardSize; ++y) {
			for (size_t x = 0; x < kBoardSize; ++x) {
				size_t p = y * kBoardSize + x;
				output += kStoneString[board_[p]];
			}
			output += "\n";
		}
		return output;
	}
	array<size_t, 4> GetRange() const noexcept{
		array<size_t, 4> range{kBoardSize, kBoardSize, 0, 0};
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
	int NextMove(const size_t depth) {
		if (IsDraw() || IsGameSet()) return -1;
		//First move most be Ten-Gen(h8).
		if (CountStone() == 0) return (kBoardSize * 7 + 7);
		//Second move most be "Kansetsu"(i7) or "Chokusetsu"(i7).
		if (CountStone() == 1 && board_[kBoardSize * 7 + 7] == Stone::Black) {
			return (RandInt(2) == 0 ? (kBoardSize * 6 + 7) : (kBoardSize * 6 + 8));
		}
		// Calc NextMove's score
		if (depth == 0) {
			vector<size_t> next_move;
			int max_score = -kScoreInf - 1;
			auto range = GetRange();
			for (size_t y = range[1]; y <= range[3]; ++y) {
				for (size_t x = range[0]; x <= range[2]; ++x) {
					size_t p = x + y * kBoardSize;
					// Validation of this move
					if (board_[p] != Stone::None) continue;
					int score = CalcScore(p, turn_);
					if (score == kScoreProhibit) continue;
					if (score == kScoreInf) return p;
					// Refresh best move
					if (max_score < score) {
						max_score = score;
						next_move.clear();
					}
					if (max_score == score) {
						next_move.push_back(p);
					}
				}
			}
			return (next_move.size() == 0 ? -1 : next_move[RandInt(next_move.size())]);
		}
		else {
			vector<Score> pre_score;
			auto range = GetRange();
			for (size_t y = range[1]; y <= range[3]; ++y) {
				for (size_t x = range[0]; x <= range[2]; ++x) {
					size_t p = x + y * kBoardSize;
					// Validation of this move
					if (board_[p] != Stone::None) continue;
					int score = CalcScore(p, turn_);
					if (score == kScoreProhibit) continue;
					if (score == kScoreInf) return p;
					pre_score.push_back(Score(p, score));
				}
			}
			std::sort(pre_score.begin(), pre_score.end(), [](const Score &a, const Score &b) {return std::get<1>(a) > std::get<1>(b); });
			for (auto &it : pre_score) {
				board_[std::get<0>(it)] = turn_;
				std::get<1>(it) = -NegaMax(ReverseTurn(turn_), depth - 1, -kScoreInf - 1, kScoreInf + 1);
				board_[std::get<0>(it)] = Stone::None;
//				cout << PositionToString(std::get<0>(it)) << " " << std::get<1>(it) << endl;
				if (std::get<1>(it) == kScoreInf) return std::get<0>(it);
			}
			vector<size_t> next_move;
			int max_score = -kScoreInf - 1;
			for (auto &it : pre_score) {
				// Refresh best move
				if (max_score < std::get<1>(it)) {
					max_score = std::get<1>(it);
					next_move.clear();
				}
				if (max_score == std::get<1>(it)) {
					next_move.push_back(std::get<0>(it));
				}
			}
			return (next_move.size() == 0 ? -1 : next_move[RandInt(next_move.size())]);
		}
	}
	bool IsDraw() const {
		// If there aren't many stones on board, you don't check draw's judge.
		if (CountStone() < (kBoardSize - 2) * (kBoardSize * 2)) return false;
		// Fill board -> find "Ren" that's length longer than 5
		for (auto turn : { Stone::Black, Stone::White }) {
			Board temp = *this;
			for (size_t p = 0; p < kBoardSize * kBoardSize; ++p) {
				if (temp.board_[p] == Stone::None) temp.board_[p] = turn;
			}
			if (!temp.IsNogame(turn)) return false;
		}
		return true;
	}
	bool IsNogame(const Stone turn) {
		array<Point, 4> pos_offsets{ Point{ 1, 0 }, Point{ 1, 1 }, Point{ 0, 1 }, Point{ -1, 1 } };
		// Direction kinds
		for (size_t i = 0; i < 4; ++i) {
			Point &it_offset = pos_offsets[i];
			size_t loops;
			if (i == 0 || i == 2) loops = kBoardSize; else loops = kBoardSize * 2 - 1;
			for (size_t j = 0; j < loops; ++j) {
				// Get pattern
				array<Stone, kBoardSize> pattern;
				std::fill(pattern.begin(), pattern.end(), Stone::None);
				Point first_pos;
				switch (i) {
				case 0:
					first_pos = Point(0, j);
					break;
				case 1:
					if (j < kBoardSize) {
						first_pos = Point(kBoardSize - j - 1, 0);
					}
					else {
						first_pos = Point(0, j - kBoardSize + 1);
					}
					break;
				case 2:
					first_pos = Point(j, 0);
					break;
				case 3:
					if (j < kBoardSize) {
						first_pos = Point(j, 0);
					}
					else {
						first_pos = Point(kBoardSize - 1, j - kBoardSize + 1);
					}
					break;
				}
				size_t k; Point pos;
				for (k = 0, pos = first_pos; pos.IsInBoard(); ++k, pos += it_offset) {
					pattern[k] = board_[pos.GetPos()];
				}
				if (HasGoren2(pattern, turn)) return false;
			}
		}
		return true;
	}
	bool IsGameSet() {
		if (IsGameSet(Stone::Black)) return true;
		if (IsGameSet(Stone::White)) return true;
		return false;
	}
	int IsGameSet(const Stone turn) const noexcept {
		array<size_t, 4> pos_offsets{ Point{ 1, 0 }.GetPos(), Point{ 1, 1 }.GetPos(), Point{ 0, 1 }.GetPos(), Point{ -1, 1 }.GetPos() };
		// Search Go-ren and check Cho-ren
		bool choren_flg = false;
		for (size_t i = 0; i < 4; ++i) {
			size_t &it_offset = pos_offsets[i];
			size_t loops;
			if (i == 0 || i == 2) loops = kBoardSize; else loops = kBoardSize * 2 - 1;
			for (size_t j = 0; j < loops; ++j) {
				// Get pattern
				size_t max_iterate, first_pos;
				switch (i) {
				case 0:
					max_iterate = kBoardSize;
					first_pos = Point(0, j).GetPos();
					break;
				case 1:
					if (j < kBoardSize) {
						max_iterate = j + 1;
						first_pos = Point(kBoardSize - j - 1, 0).GetPos();
					}
					else {
						max_iterate = kBoardSize * 2 - j - 1;
						first_pos = Point(0, j - kBoardSize + 1).GetPos();
					}
					break;
				case 2:
					max_iterate = kBoardSize;
					first_pos = Point(j, 0).GetPos();
					break;
				case 3:
					if (j < kBoardSize) {
						max_iterate = j + 1;
						first_pos = Point(j, 0).GetPos();
					}
					else {
						max_iterate = kBoardSize * 2 - j - 1;
						first_pos = Point(kBoardSize - 1, j - kBoardSize + 1).GetPos();
					}
					break;
				}
				size_t k, len = 0, pos;
				for (k = 0, pos = first_pos; k < max_iterate; ++k, pos += it_offset) {
					if (board_[pos] == turn) {
						++len;
					}else{
						if (len == 5) {
							return kScoreInf;
						}else if(len >= 6){
							if (turn == Stone::White) return kScoreInf;
							choren_flg = true;
						}
						len = 0;
					}
				}
				if (len == 5) {
					return kScoreInf;
				}
				else if (len >= 6) {
					if (turn == Stone::White) return kScoreInf;
					choren_flg = true;
				}
			}
		}
		if (turn == Stone::Black && choren_flg) return -kScoreInf;
		return 0;
	}
	int CalcScore(const size_t position, const Stone turn, const bool gameset_check_flg = false)noexcept {
		// If this board is game end, return status
		if (gameset_check_flg) {
			if (IsDraw()) return 0;
			auto game_set_check = IsGameSet(turn);
			if (game_set_check != 0) return game_set_check;
			game_set_check = IsGameSet(ReverseTurn(turn));
			if (game_set_check != 0) return -game_set_check;
		}
		// Calc Score
		Point pos_move(position);
		size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
		array<Point, 4> pos_offsets{ Point{ 1, 0 }, Point{ 1, 1 }, Point{ 0, 1 }, Point{ -1, 1 } };
		for (auto &it_offset : pos_offsets) {
			// Get right and left pattern
			array<Stone, 5> right_pattern, left_pattern;
			std::fill(right_pattern.begin(), right_pattern.end(), Stone::None);
			std::fill(left_pattern.begin(), left_pattern.end(), Stone::None);
			Point pos;
			size_t i;
			for (i = 0, pos = pos_move + it_offset; i < 5 && pos.IsInBoard(); ++i, pos += it_offset) {
				right_pattern[i] = board_[pos.GetPos()];
			}
			for (i = 0, pos = pos_move - it_offset; i < 5 && pos.IsInBoard(); ++i, pos -= it_offset) {
				left_pattern[i] = board_[pos.GetPos()];
			}
			// Pattern matching
			if (turn == Stone::Black) {
				size_t right_pattern_1 = (right_pattern[0] != Stone::White ? right_pattern[0] : Stone::None);
				size_t right_pattern_2 = Ternary(right_pattern[0], (right_pattern[1] != Stone::White ? right_pattern[1] : Stone::None));
				size_t right_pattern_3 = Ternary(right_pattern[0], right_pattern[1], (right_pattern[2] != Stone::White ? right_pattern[2] : Stone::None));
				size_t right_pattern_4 = Ternary(right_pattern[0], right_pattern[1], right_pattern[2], (right_pattern[3] != Stone::White ? right_pattern[3] : Stone::None));
				size_t right_pattern_5 = Ternary(right_pattern[0], right_pattern[1], right_pattern[2], right_pattern[3], (right_pattern[4] != Stone::White ? right_pattern[4] : Stone::None));
				size_t left_pattern_1 = (left_pattern[0] != Stone::White ? left_pattern[0] : Stone::None);
				size_t left_pattern_2 = Ternary(left_pattern[0], (left_pattern[1] != Stone::White ? left_pattern[1] : Stone::None));
				size_t left_pattern_3 = Ternary(left_pattern[0], left_pattern[1], (left_pattern[2] != Stone::White ? left_pattern[2] : Stone::None));
				size_t left_pattern_4 = Ternary(left_pattern[0], left_pattern[1], left_pattern[2], (left_pattern[3] != Stone::White ? left_pattern[3] : Stone::None));
				size_t left_pattern_5 = Ternary(left_pattern[0], left_pattern[1], left_pattern[2], left_pattern[3], (left_pattern[4] != Stone::White ? left_pattern[4] : Stone::None));
				// Quintuplex(Go-ren)
				if (right_pattern_1 == Stone::Black && left_pattern_5 == Ternary(Stone::Black, Stone::Black, Stone::Black, Stone::Black, Stone::None)) return kScoreInf;
				if (right_pattern_2 == Ternary(Stone::Black, Stone::None) && left_pattern_4 == Ternary(Stone::Black, Stone::Black, Stone::Black, Stone::None)) return kScoreInf;
				if (right_pattern_3 == Ternary(Stone::Black, Stone::Black, Stone::None) && left_pattern_3 == Ternary(Stone::Black, Stone::Black, Stone::None)) return kScoreInf;
				if (left_pattern_2 == Ternary(Stone::Black, Stone::None) && right_pattern_4 == Ternary(Stone::Black, Stone::Black, Stone::Black, Stone::None)) return kScoreInf;
				if (left_pattern_1 == Stone::Black && right_pattern_5 == Ternary(Stone::Black, Stone::Black, Stone::Black, Stone::Black, Stone::None)) return kScoreInf;

				// Too long multiplex(Cho-ren)
				// Cho-ren of black is Prohibited move(Kinsyu).
				if (left_pattern_5 == Ternary(Stone::Black, Stone::Black, Stone::Black, Stone::Black, Stone::Black)) return kScoreProhibit;
				if (right_pattern_1 == Stone::Black && left_pattern_4 == Ternary(Stone::Black, Stone::Black, Stone::Black, Stone::Black)) return kScoreProhibit;
				if (right_pattern_2 == Ternary(Stone::Black, Stone::Black) && left_pattern_3 == Ternary(Stone::Black, Stone::Black, Stone::Black)) return kScoreProhibit;
				if (right_pattern_5 == Ternary(Stone::Black, Stone::Black, Stone::Black, Stone::Black, Stone::Black)) return kScoreProhibit;
				if (left_pattern_1 == Stone::Black && right_pattern_4 == Ternary(Stone::Black, Stone::Black, Stone::Black, Stone::Black)) return kScoreProhibit;
				if (left_pattern_2 == Ternary(Stone::Black, Stone::Black) && right_pattern_3 == Ternary(Stone::Black, Stone::Black, Stone::Black)) return kScoreProhibit;

				/* If you make some Quadruplex of black, it's Prohibited move(Kinsyu) "Shi-Shi".
				* 1. BO|BBB|OB   "Ryoutou-no-ShiShi"
				* 2. BBO|BB|OBB  "Chouda-no-ShiShi"
				* 3. BBBO|B|OBBB "Souryu-no-ShiShi"
				* 一直線上に出来る四々で、両頭の四々・長蛇の四々・双龍の四々
				*/
				//BO|BBB|OB
				if (right_pattern_5 == Ternary(Stone::Black, Stone::Black, Stone::None, Stone::Black, Stone::None) && left_pattern_3 == Ternary(Stone::None, Stone::Black, Stone::None)) return kScoreProhibit;
				if (right_pattern_4 == Ternary(Stone::Black, Stone::None, Stone::Black, Stone::None) && left_pattern_4 == Ternary(Stone::Black, Stone::None, Stone::Black, Stone::None)) return kScoreProhibit;
				if (right_pattern_3 == Ternary(Stone::None, Stone::Black, Stone::None) && left_pattern_5 == Ternary(Stone::Black, Stone::Black, Stone::None, Stone::Black, Stone::None)) return kScoreProhibit;
				//BBO|BB|OBB
				if (right_pattern_5 == Ternary(Stone::Black, Stone::None, Stone::Black, Stone::Black, Stone::None) && left_pattern_4 == Ternary(Stone::None, Stone::Black, Stone::Black, Stone::None)) return kScoreProhibit;
				if (right_pattern_4 == Ternary(Stone::None, Stone::Black, Stone::Black, Stone::None) && left_pattern_5 == Ternary(Stone::Black, Stone::None, Stone::Black, Stone::Black, Stone::None)) return kScoreProhibit;
				//BBBO|B|OBBB
				if (right_pattern_5 == Ternary(Stone::None, Stone::Black, Stone::Black, Stone::Black, Stone::None) && left_pattern_5 == Ternary(Stone::None, Stone::Black, Stone::Black, Stone::Black, Stone::None)) return kScoreProhibit;

				// Strong Quadruplex(Shi-ren)
				if (right_pattern_2 == Ternary(Stone::None, Stone::None) && left_pattern_5 == Ternary(Stone::Black, Stone::Black, Stone::Black, Stone::None, Stone::None)) { ++sum_4_strong; continue; }
				if (right_pattern_3 == Ternary(Stone::Black, Stone::None, Stone::None) && left_pattern_4 == Ternary(Stone::Black, Stone::Black, Stone::None, Stone::None)) { ++sum_4_strong; continue; }
				if (right_pattern_4 == Ternary(Stone::Black, Stone::Black, Stone::None, Stone::None) && left_pattern_3 == Ternary(Stone::Black, Stone::None, Stone::None)) { ++sum_4_strong; continue; }
				if (right_pattern_5 == Ternary(Stone::Black, Stone::Black, Stone::Black, Stone::None, Stone::None) && left_pattern_2 == Ternary(Stone::None, Stone::None)) { ++sum_4_strong; continue; }

				// Normal Quadruplex(Katsu-shi)
				//11110
				if (right_pattern_2 == Ternary(Stone::None, Stone::None) && left_pattern_4 == Ternary(Stone::Black, Stone::Black, Stone::Black, Stone::None)) { ++sum_4_normal; continue; }
				if (right_pattern_3 == Ternary(Stone::Black, Stone::None, Stone::None) && left_pattern_3 == Ternary(Stone::Black, Stone::Black, Stone::None)) { ++sum_4_normal; continue; }
				if (right_pattern_4 == Ternary(Stone::Black, Stone::Black, Stone::None, Stone::None) && left_pattern_2 == Ternary(Stone::Black, Stone::None)) { ++sum_4_normal; continue; }
				if (right_pattern_5 == Ternary(Stone::Black, Stone::Black, Stone::Black, Stone::None, Stone::None) && left_pattern_1 == Ternary(Stone::None)) { ++sum_4_normal; continue; }
				//11101
				if (right_pattern_1 == Ternary(Stone::None) && left_pattern_5 == Ternary(Stone::None, Stone::Black, Stone::Black, Stone::Black, Stone::None)) { ++sum_4_normal; continue; }
				if (right_pattern_3 == Ternary(Stone::None, Stone::Black, Stone::None) && left_pattern_3 == Ternary(Stone::Black, Stone::Black, Stone::None)) { ++sum_4_normal; continue; }
				if (right_pattern_4 == Ternary(Stone::Black, Stone::None, Stone::Black, Stone::None) && left_pattern_2 == Ternary(Stone::Black, Stone::None)) { ++sum_4_normal; continue; }
				if (right_pattern_5 == Ternary(Stone::Black, Stone::Black, Stone::None, Stone::Black, Stone::None) && left_pattern_1 == Ternary(Stone::None)) { ++sum_4_normal; continue; }
				//11011
				if (right_pattern_1 == Ternary(Stone::None) && left_pattern_5 == Ternary(Stone::Black, Stone::None, Stone::Black, Stone::Black, Stone::None)) { ++sum_4_normal; continue; }
				if (right_pattern_2 == Ternary(Stone::Black, Stone::None) && left_pattern_4 == Ternary(Stone::None, Stone::Black, Stone::Black, Stone::None)) { ++sum_4_normal; continue; }
				if (left_pattern_1 == Ternary(Stone::None) && right_pattern_5 == Ternary(Stone::Black, Stone::None, Stone::Black, Stone::Black, Stone::None)) { ++sum_4_normal; continue; }
				if (left_pattern_2 == Ternary(Stone::Black, Stone::None) && right_pattern_4 == Ternary(Stone::None, Stone::Black, Stone::Black, Stone::None)) { ++sum_4_normal; continue; }
				//10111
				if (left_pattern_1 == Ternary(Stone::None) && right_pattern_5 == Ternary(Stone::None, Stone::Black, Stone::Black, Stone::Black, Stone::None)) { ++sum_4_normal; continue; }
				if (left_pattern_3 == Ternary(Stone::None, Stone::Black, Stone::None) && right_pattern_3 == Ternary(Stone::Black, Stone::Black, Stone::None)) { ++sum_4_normal; continue; }
				if (left_pattern_4 == Ternary(Stone::Black, Stone::None, Stone::Black, Stone::None) && right_pattern_2 == Ternary(Stone::Black, Stone::None)) { ++sum_4_normal; continue; }
				if (left_pattern_5 == Ternary(Stone::Black, Stone::Black, Stone::None, Stone::Black, Stone::None) && right_pattern_1 == Ternary(Stone::None)) { ++sum_4_normal; continue; }
				//01111
				if (left_pattern_2 == Ternary(Stone::None, Stone::None) && right_pattern_4 == Ternary(Stone::Black, Stone::Black, Stone::Black, Stone::None)) { ++sum_4_normal; continue; }
				if (left_pattern_3 == Ternary(Stone::Black, Stone::None, Stone::None) && right_pattern_3 == Ternary(Stone::Black, Stone::Black, Stone::None)) { ++sum_4_normal; continue; }
				if (left_pattern_4 == Ternary(Stone::Black, Stone::Black, Stone::None, Stone::None) && right_pattern_2 == Ternary(Stone::Black, Stone::None)) { ++sum_4_normal; continue; }
				if (left_pattern_5 == Ternary(Stone::Black, Stone::Black, Stone::Black, Stone::None, Stone::None) && right_pattern_1 == Ternary(Stone::None)) { ++sum_4_normal; continue; }

				// Strong Triplex(Katsu-san)
				int offset1 = 0, offset2 = 0;
				while (true) {
					//011100
					if (right_pattern_3 == Ternary(Stone::None, Stone::None, Stone::None) && left_pattern_4 == Ternary(Stone::Black, Stone::Black, Stone::None, Stone::None)) { offset1 = 1; offset2 = -3; break; }
					if (right_pattern_4 == Ternary(Stone::Black, Stone::None, Stone::None, Stone::None) && left_pattern_3 == Ternary(Stone::Black, Stone::None, Stone::None)) { offset1 = 2; offset2 = -2; break; }
					if (right_pattern_5 == Ternary(Stone::Black, Stone::Black, Stone::None, Stone::None, Stone::None) && left_pattern_2 == Ternary(Stone::None, Stone::None)) { offset1 = 3; offset2 = -1; break; }
					//011010
					if (right_pattern_2 == Ternary(Stone::None, Stone::None) && left_pattern_5 == Ternary(Stone::None, Stone::Black, Stone::Black, Stone::None, Stone::None)) { offset1 = -1; break; }
					if (right_pattern_4 == Ternary(Stone::None, Stone::Black, Stone::None, Stone::None) && left_pattern_3 == Ternary(Stone::Black, Stone::None, Stone::None)) { offset1 = 1; break; }
					if (right_pattern_5 == Ternary(Stone::Black, Stone::None, Stone::Black, Stone::None, Stone::None) && left_pattern_2 == Ternary(Stone::None, Stone::None)) { offset1 = 2; break; }
					//010110
					if (left_pattern_2 == Ternary(Stone::None, Stone::None) && right_pattern_5 == Ternary(Stone::None, Stone::Black, Stone::Black, Stone::None, Stone::None)) { offset1 = 1; break; }
					if (left_pattern_4 == Ternary(Stone::None, Stone::Black, Stone::None, Stone::None) && right_pattern_3 == Ternary(Stone::Black, Stone::None, Stone::None)) { offset1 = -1; break; }
					if (left_pattern_5 == Ternary(Stone::Black, Stone::None, Stone::Black, Stone::None, Stone::None) && right_pattern_2 == Ternary(Stone::None, Stone::None)) { offset1 = -2; break; }
					//001110
					if (left_pattern_3 == Ternary(Stone::None, Stone::None, Stone::None) && right_pattern_4 == Ternary(Stone::Black, Stone::Black, Stone::None, Stone::None)) { offset1 = -1; offset2 = 3; break; }
					if (left_pattern_4 == Ternary(Stone::Black, Stone::None, Stone::None, Stone::None) && right_pattern_3 == Ternary(Stone::Black, Stone::None, Stone::None)) { offset1 = -2; offset2 = 2; break; }
					if (left_pattern_5 == Ternary(Stone::Black, Stone::Black, Stone::None, Stone::None, Stone::None) && right_pattern_2 == Ternary(Stone::None, Stone::None)) { offset1 = -3; offset2 = 1; break; }
					break;
				}
				if (offset2 != 0) {
					auto pos_move2 = pos_move + it_offset * offset2;
					board_[position] = turn;
					auto score = CalcScore(pos_move2.GetPos(), turn, true);
					board_[position] = Stone::None;
					if (score != kScoreProhibit) {
						++sum_3;
						continue;
					}
				}
				if (offset1 != 0) {
					auto pos_move2 = pos_move + it_offset * offset1;
					board_[position] = turn;
					auto score = CalcScore(pos_move2.GetPos(), turn, true);
					board_[position] = Stone::None;
					if (score != kScoreProhibit) {
						++sum_3;
						continue;
					}
				}
			}
			else {
				size_t right_pattern_1 = right_pattern[0];
				size_t right_pattern_2 = Ternary(right_pattern[0], right_pattern[1]);
				size_t right_pattern_3 = Ternary(right_pattern[0], right_pattern[1], right_pattern[2]);
				size_t right_pattern_4 = Ternary(right_pattern[0], right_pattern[1], right_pattern[2], right_pattern[3]);
				size_t right_pattern_5 = Ternary(right_pattern[0], right_pattern[1], right_pattern[2], right_pattern[3], right_pattern[4]);
				size_t left_pattern_1 = left_pattern[0];
				size_t left_pattern_2 = Ternary(left_pattern[0], left_pattern[1]);
				size_t left_pattern_3 = Ternary(left_pattern[0], left_pattern[1], left_pattern[2]);
				size_t left_pattern_4 = Ternary(left_pattern[0], left_pattern[1], left_pattern[2], left_pattern[3]);
				size_t left_pattern_5 = Ternary(left_pattern[0], left_pattern[1], left_pattern[2], left_pattern[3], left_pattern[4]);
				// Quintuplex(Go-ren)
				if (left_pattern_4 == Ternary(Stone::White, Stone::White, Stone::White, Stone::White)) return kScoreInf;
				if (right_pattern_1 == Ternary(Stone::White) && left_pattern_3 == Ternary(Stone::White, Stone::White, Stone::White)) return kScoreInf;
				if (right_pattern_2 == Ternary(Stone::White, Stone::White) && left_pattern_2 == Ternary(Stone::White, Stone::White)) return kScoreInf;
				if (right_pattern_3 == Ternary(Stone::White, Stone::White, Stone::White) && left_pattern_1 == Ternary(Stone::White)) return kScoreInf;
				if (right_pattern_4 == Ternary(Stone::White, Stone::White, Stone::White, Stone::White)) return kScoreInf;

				/* If you make some Quadruplex of black, it's Prohibited move(Kinsyu) "Shi-Shi".
				* 1. BO|BBB|OB   "Ryoutou-no-ShiShi"
				* 2. BBO|BB|OBB  "Chouda-no-ShiShi"
				* 3. BBBO|B|OBBB "Souryu-no-ShiShi"
				* 一直線上に出来る四々で、両頭の四々・長蛇の四々・双龍の四々
				*/
				//
				if (right_pattern_4 == Ternary(Stone::White, Stone::White, Stone::None, Stone::White) && left_pattern_2 == Ternary(Stone::None, Stone::White)) { sum_4_normal += 2; continue; }
				if (right_pattern_3 == Ternary(Stone::White, Stone::None, Stone::White) && left_pattern_3 == Ternary(Stone::White, Stone::None, Stone::White)) { sum_4_normal += 2; continue; }
				if (right_pattern_2 == Ternary(Stone::None, Stone::White) && left_pattern_4 == Ternary(Stone::White, Stone::White, Stone::None, Stone::White)) { sum_4_normal += 2; continue; }
				//
				if (right_pattern_4 == Ternary(Stone::White, Stone::None, Stone::White, Stone::White) && left_pattern_3 == Ternary(Stone::None, Stone::White, Stone::White)) { sum_4_normal += 2; continue; }
				if (right_pattern_3 == Ternary(Stone::None, Stone::White, Stone::White) && left_pattern_4 == Ternary(Stone::White, Stone::None, Stone::White, Stone::White)) { sum_4_normal += 2; continue; }
				//
				if (right_pattern_4 == Ternary(Stone::None, Stone::White, Stone::White, Stone::White) && left_pattern_4 == Ternary(Stone::None, Stone::White, Stone::White, Stone::White)) { sum_4_normal += 2; continue; }

				// Strong Quadruplex(Shi-ren)
				if (right_pattern_1 == Ternary(Stone::None) && left_pattern_4 == Ternary(Stone::White, Stone::White, Stone::White, Stone::None)) { ++sum_4_strong; continue; }
				if (right_pattern_2 == Ternary(Stone::White, Stone::None) && left_pattern_3 == Ternary(Stone::White, Stone::White, Stone::None)) { ++sum_4_strong; continue; }
				if (right_pattern_3 == Ternary(Stone::White, Stone::White, Stone::None) && left_pattern_2 == Ternary(Stone::White, Stone::None)) { ++sum_4_strong; continue; }
				if (right_pattern_4 == Ternary(Stone::White, Stone::White, Stone::White, Stone::None) && left_pattern_1 == Ternary(Stone::None)) { ++sum_4_strong; continue; }

				// Normal Quadruplex(Katsu-shi)
				//11110
				if (right_pattern_1 == Ternary(Stone::None) && left_pattern_3 == Ternary(Stone::White, Stone::White, Stone::White)) { ++sum_4_normal; continue; }
				if (right_pattern_2 == Ternary(Stone::White, Stone::None) && left_pattern_2 == Ternary(Stone::White, Stone::White)) { ++sum_4_normal; continue; }
				if (right_pattern_3 == Ternary(Stone::White, Stone::White, Stone::None) && left_pattern_1 == Ternary(Stone::White)) { ++sum_4_normal; continue; }
				if (right_pattern_4 == Ternary(Stone::White, Stone::White, Stone::White, Stone::None)) { ++sum_4_normal; continue; }
				//11101
				if (left_pattern_4 == Ternary(Stone::None, Stone::White, Stone::White, Stone::White)) { ++sum_4_normal; continue; }
				if (right_pattern_2 == Ternary(Stone::None, Stone::White) && left_pattern_2 == Ternary(Stone::White, Stone::White)) { ++sum_4_normal; continue; }
				if (right_pattern_3 == Ternary(Stone::White, Stone::None, Stone::White) && left_pattern_1 == Ternary(Stone::White)) { ++sum_4_normal; continue; }
				if (right_pattern_4 == Ternary(Stone::White, Stone::White, Stone::None, Stone::White)) { ++sum_4_normal; continue; }
				//11011
				if (left_pattern_4 == Ternary(Stone::White, Stone::None, Stone::White, Stone::White)) { ++sum_4_normal; continue; }
				if (right_pattern_1 == Ternary(Stone::White) && left_pattern_3 == Ternary(Stone::None, Stone::White, Stone::White)) { ++sum_4_normal; continue; }
				if (right_pattern_4 == Ternary(Stone::White, Stone::None, Stone::White, Stone::White)) { ++sum_4_normal; continue; }
				if (left_pattern_1 == Ternary(Stone::White) && right_pattern_3 == Ternary(Stone::None, Stone::White, Stone::White)) { ++sum_4_normal; continue; }
				//10111
				if (right_pattern_4 == Ternary(Stone::None, Stone::White, Stone::White, Stone::White)) { ++sum_4_normal; continue; }
				if (left_pattern_2 == Ternary(Stone::None, Stone::White) && right_pattern_2 == Ternary(Stone::White, Stone::White)) { ++sum_4_normal; continue; }
				if (left_pattern_3 == Ternary(Stone::White, Stone::None, Stone::White) && right_pattern_1 == Ternary(Stone::White)) { ++sum_4_normal; continue; }
				if (left_pattern_4 == Ternary(Stone::White, Stone::White, Stone::None, Stone::White)) { ++sum_4_normal; continue; }
				//01111
				if (left_pattern_1 == Ternary(Stone::None) && right_pattern_3 == Ternary(Stone::White, Stone::White, Stone::White)) { ++sum_4_normal; continue; }
				if (left_pattern_2 == Ternary(Stone::White, Stone::None) && right_pattern_2 == Ternary(Stone::White, Stone::White)) { ++sum_4_normal; continue; }
				if (left_pattern_3 == Ternary(Stone::White, Stone::White, Stone::None) && right_pattern_1 == Ternary(Stone::White)) { ++sum_4_normal; continue; }
				if (left_pattern_4 == Ternary(Stone::White, Stone::White, Stone::White, Stone::None)) { ++sum_4_normal; continue; }

				// Strong Triplex(Katsu-san)
				//011100
				if (right_pattern_2 == Ternary(Stone::None, Stone::None) && left_pattern_3 == Ternary(Stone::White, Stone::White, Stone::None)) { ++sum_3; continue; }
				if (right_pattern_3 == Ternary(Stone::White, Stone::None, Stone::None) && left_pattern_2 == Ternary(Stone::White, Stone::None)) { ++sum_3; continue; }
				if (right_pattern_4 == Ternary(Stone::White, Stone::White, Stone::None, Stone::None) && left_pattern_1 == Ternary(Stone::None)) { ++sum_3; continue; }
				//011010
				if (right_pattern_1 == Ternary(Stone::None) && left_pattern_4 == Ternary(Stone::None, Stone::White, Stone::White, Stone::None)) { ++sum_3; continue; }
				if (right_pattern_3 == Ternary(Stone::None, Stone::White, Stone::None) && left_pattern_2 == Ternary(Stone::White, Stone::None)) { ++sum_3; continue; }
				if (right_pattern_4 == Ternary(Stone::White, Stone::None, Stone::White, Stone::None) && left_pattern_1 == Ternary(Stone::None)) { ++sum_3; continue; }
				//010110
				if (left_pattern_1 == Ternary(Stone::None) && right_pattern_4 == Ternary(Stone::None, Stone::White, Stone::White, Stone::None)) { ++sum_3; continue; }
				if (left_pattern_3 == Ternary(Stone::None, Stone::White, Stone::None) && right_pattern_2 == Ternary(Stone::White, Stone::None)) { ++sum_3; continue; }
				if (left_pattern_4 == Ternary(Stone::White, Stone::None, Stone::White, Stone::None) && right_pattern_1 == Ternary(Stone::None)) { ++sum_3; continue; }
				//001110
				if (left_pattern_2 == Ternary(Stone::None, Stone::None) && right_pattern_3 == Ternary(Stone::White, Stone::White, Stone::None)) { ++sum_3; continue; }
				if (left_pattern_3 == Ternary(Stone::White, Stone::None, Stone::None) && right_pattern_2 == Ternary(Stone::White, Stone::None)) { ++sum_3; continue; }
				if (left_pattern_4 == Ternary(Stone::White, Stone::White, Stone::None, Stone::None) && right_pattern_1 == Ternary(Stone::None)) { ++sum_3; continue; }
			}
		}
		if (sum_4_strong + sum_4_normal >= 2) return (turn == Stone::Black ? kScoreProhibit : 80);
		if (sum_3 >= 2) return (turn == Stone::Black ? kScoreProhibit : 60);
		if (sum_4_strong + sum_4_normal == 1 && sum_3 == 1) return 70;
		if (sum_4_strong == 1) return 90;
		if (sum_4_normal == 1) return 50;
		if (sum_3 == 1) return 10;
		return pos_move.TenGenDist();
	}
	int NegaMax(const Stone turn, const size_t depth, int alpha, int beta) noexcept{
		// If this board is game end, return status
		if (IsDraw()) return 0;
		auto game_set_check = IsGameSet(turn);
		if (game_set_check != 0) return game_set_check;
		game_set_check = IsGameSet(ReverseTurn(turn));
		if (game_set_check != 0) return -game_set_check;
		// Calc NextMove's score
		if (depth == 0) {
			int max_score = alpha;
			auto range = GetRange();
			for (size_t y = range[1]; y <= range[3]; ++y) {
				for (size_t x = range[0]; x <= range[2]; ++x) {
					size_t p = x + y * kBoardSize;
					// Validation of this move
					if (board_[p] != Stone::None) continue;
					int score = CalcScore(p, turn);
					if (score == kScoreProhibit) continue;
					if (score == kScoreInf) return kScoreInf;
					// Refresh best move
					if (max_score < score) {
						max_score = score;
						if (max_score >= beta) return max_score;
					}
				}
			}
			return max_score;
		}
		else {
			vector<Score> pre_score;
			auto range = GetRange();
				for (size_t y = range[1]; y <= range[3]; ++y) {
					for (size_t x = range[0]; x <= range[2]; ++x) {
						size_t p = x + y * kBoardSize;
					// Validation of this move
					if (board_[p] != Stone::None) continue;
					int score = CalcScore(p, turn);
					if (score == kScoreProhibit) continue;
					if (score == kScoreInf) return kScoreInf;
					pre_score.push_back(Score(p, score));
				}
			}
			std::sort(pre_score.begin(), pre_score.end(), [](const Score &a, const Score &b) {return std::get<1>(a) > std::get<1>(b); });
			for (auto &it : pre_score) {
				board_[std::get<0>(it)] = turn;
				std::get<1>(it) = -NegaMax(ReverseTurn(turn), depth - 1, -beta, -alpha);
				board_[std::get<0>(it)] = Stone::None;
				if (std::get<1>(it) == kScoreInf) return kScoreInf;
				alpha = std::max(alpha, std::get<1>(it));
				if (alpha >= beta) return alpha;
			}
			int max_score = alpha;
			for (auto &it : pre_score) {
				// Refresh best move
				if (max_score < std::get<1>(it)) {
					max_score = std::get<1>(it);
					if (max_score >= beta) return max_score;
				}
			}
			return max_score;
		}
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
			<< "stone or blank : black(*), white(O), blank(-)" << endl;
	}
	try {
		Board board(argv[1], argv[2]);
		int depth = std::stoi(argv[3]);
		if (depth < 0) depth = 0;
//		cout << PositionToString(board.NextMove(depth)) << endl;
		cout << board.NextMove(depth) << endl;
	}
	catch (const std::exception& er) {
		std::cerr << er.what() << endl;
		cout << -1 << endl;
		return 1;
	}
	return 0;
}
