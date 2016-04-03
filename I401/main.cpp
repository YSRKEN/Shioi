#include<array>
#include<cstdint>
#include<iostream>
#include<random>
#include<string>
#include<vector>

using std::array;
using std::cout;
using std::endl;
using std::string;
using std::vector;

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

string PositionToString(const size_t p) {
	const static string kPositionString = "abcdefghijklmno";
	return kPositionString.substr(p % kBoardSize, 1) + std::to_string(p / kBoardSize + 1);
}

class Point {
	int x_, y_;
public:
	Point(){}
	Point(const int x, const int y) {
		x_ = x;
		y_ = y;
	}
	Point(const size_t p){
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
	Point operator +(const Point &offset) const{
		return Point(x_ + offset.x_, y_ + offset.y_);
	}
	Point operator -(const Point &offset) const {
		return Point(x_ - offset.x_, y_ - offset.y_);
	}
	Point operator *(const int n) const {
		return Point(x_ * n, y_ * n);
	}
	Point& operator +=(const Point &offset) noexcept{
		x_ += offset.x_;
		y_ += offset.y_;
		return *this;
	}
	Point& operator -=(const Point &offset) noexcept{
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
	int NextMove() {
		vector<size_t> next_move;
		int max_score = -kScoreInf - 1;
		for (size_t p = 0; p < kBoardSize * kBoardSize; ++p) {
			if (!IsValidMove(p, turn_)) continue;
			int score = CalcScore(p, turn_);
			/*if (score != 0) {
				cout << PositionToString(p) << " " << score << endl;
			}*/
			if (max_score < score) {
				max_score = score;
				next_move.clear();
			}
			if (max_score == score) {
				next_move.push_back(p);
			}
		}
		return (next_move.size() == 0 ? -1 : next_move[std::uniform_int_distribution<size_t>{0, next_move.size() - 1}(mt)]);
	}
	bool IsValidMove(const size_t position, const Stone turn) const noexcept {
		if (board_[position] != Stone::None) return false;
		return true;
	}
	int CalcScore(const size_t position, const Stone turn) noexcept {
		// If this board is game end, return status
		
		// Calc Score
		Point pos_move(position);
		size_t sum_4_strong = 0, sum_4_normal = 0, sum_3 = 0;
		array<Point, 4> pos_offsets{ Point{1, 0}, Point{1, 1}, Point{0, 1}, Point{-1, 1} };
		for (auto &it_offset : pos_offsets) {
			// Get right and left pattern
			array<Stone, kBoardSize> right_pattern, left_pattern;
			std::fill(right_pattern.begin(), right_pattern.end(), Stone::None);
			std::fill(left_pattern.begin(), left_pattern.end(), Stone::None);
			Point pos;
			size_t i;
			for (i = 0, pos = pos_move + it_offset; pos.IsInBoard(); ++i, pos += it_offset) {
				right_pattern[i] = board_[pos.GetPos()];
			}
			for (i = 0, pos = pos_move - it_offset; pos.IsInBoard(); ++i, pos -= it_offset) {
				left_pattern[i] = board_[pos.GetPos()];
			}
			int a = 0;
			if (turn == Stone::Black) {
				// Quintuplex(Go-ren)
				if (MatchArray(right_pattern, { Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::Black, Stone::Black, Stone::UnBlack })) return kScoreInf;
				if (MatchArray(right_pattern, { Stone::Black, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::Black, Stone::UnBlack })) return kScoreInf;
				if (MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::UnBlack })) return kScoreInf;
				if (MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::Black, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::UnBlack })) return kScoreInf;
				if (MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::Black, Stone::Black, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::UnBlack })) return kScoreInf;

				// Too long multiplex(Cho-ren)
				// Cho-ren of black is Prohibited move(Kinsyu).
				if (MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::Black, Stone::Black , Stone::Black })) return kScoreProhibit;
				if (MatchArray(right_pattern, { Stone::Black }) && MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::Black, Stone::Black })) return kScoreProhibit;
				if (MatchArray(right_pattern, { Stone::Black, Stone::Black }) && MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::Black })) return kScoreProhibit;
				if (MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::Black }) && MatchArray(left_pattern, { Stone::Black, Stone::Black })) return kScoreProhibit;
				if (MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::Black, Stone::Black }) && MatchArray(left_pattern, { Stone::Black })) return kScoreProhibit;
				if (MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::Black, Stone::Black , Stone::Black })) return kScoreProhibit;

				/* If you make some Quadruplex of black, it's Prohibited move(Kinsyu) "Shi-Shi".
				* 1. BO|BBB|OB   "Ryoutou-no-ShiShi"
				* 2. BBO|BB|OBB  "Chouda-no-ShiShi"
				* 3. BBBO|B|OBBB "Souryu-no-ShiShi"
				* 一直線上に出来る四々で、両頭の四々・長蛇の四々・双龍の四々
				*/
				//BO|BBB|OB
				if (MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::None, Stone::Black, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::None, Stone::Black, Stone::UnBlack })) return kScoreProhibit;
				if (MatchArray(right_pattern, { Stone::Black, Stone::None, Stone::Black, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::None, Stone::Black, Stone::UnBlack })) return kScoreProhibit;
				if (MatchArray(right_pattern, { Stone::None, Stone::Black, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::None, Stone::Black, Stone::UnBlack })) return kScoreProhibit;
				//BBO|BB|OBB
				if (MatchArray(right_pattern, { Stone::Black, Stone::None, Stone::Black, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::None, Stone::Black, Stone::Black, Stone::UnBlack })) return kScoreProhibit;
				if (MatchArray(right_pattern, { Stone::None, Stone::Black, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::None, Stone::Black, Stone::Black, Stone::UnBlack })) return kScoreProhibit;
				//BBBO|B|OBBB
				if (MatchArray(right_pattern, { Stone::None, Stone::Black, Stone::Black, Stone::Black, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::None, Stone::Black, Stone::Black, Stone::Black, Stone::UnBlack })) return kScoreProhibit;

				// Strong Quadruplex(Shi-ren)
				if (MatchArray(right_pattern, { Stone::None, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::Black, Stone::None, Stone::UnBlack })) {++sum_4_strong; continue;}
				if (MatchArray(right_pattern, { Stone::Black, Stone::None, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::None, Stone::UnBlack })) {++sum_4_strong; continue;}
				if (MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::None, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::None, Stone::UnBlack })) {++sum_4_strong; continue;}
				if (MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::Black, Stone::None, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::None, Stone::UnBlack })) {++sum_4_strong; continue;}

				// Normal Quadruplex(Katsu-shi)
				//11110
				if (MatchArray(right_pattern, { Stone::None, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::Black, Stone::UnBlack })) { ++sum_4_normal; continue; }
				if (MatchArray(right_pattern, { Stone::Black, Stone::None, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::UnBlack })) { ++sum_4_normal; continue; }
				if (MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::None, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::UnBlack })) { ++sum_4_normal; continue; }
				if (MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::Black, Stone::None, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::UnBlack })) { ++sum_4_normal; continue; }
				//11101
				if (MatchArray(right_pattern, { Stone::UnBlack }) && MatchArray(left_pattern, { Stone::None, Stone::Black, Stone::Black, Stone::Black, Stone::UnBlack })) { ++sum_4_normal; continue; }
				if (MatchArray(right_pattern, { Stone::None, Stone::Black, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::UnBlack })) { ++sum_4_normal; continue; }
				if (MatchArray(right_pattern, { Stone::Black, Stone::None, Stone::Black, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::UnBlack })) { ++sum_4_normal; continue; }
				if (MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::None, Stone::Black, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::UnBlack })) { ++sum_4_normal; continue; }
				//11011
				if (MatchArray(right_pattern, { Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::None, Stone::Black, Stone::Black, Stone::UnBlack })) { ++sum_4_normal; continue; }
				if (MatchArray(right_pattern, { Stone::Black, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::None, Stone::Black, Stone::Black, Stone::UnBlack })) { ++sum_4_normal; continue; }
				if (MatchArray(left_pattern, { Stone::UnBlack }) && MatchArray(right_pattern, { Stone::Black, Stone::None, Stone::Black, Stone::Black, Stone::UnBlack })) { ++sum_4_normal; continue; }
				if (MatchArray(left_pattern, { Stone::Black, Stone::UnBlack }) && MatchArray(right_pattern, { Stone::None, Stone::Black, Stone::Black, Stone::UnBlack })) { ++sum_4_normal; continue; }
				//10111
				if (MatchArray(left_pattern, { Stone::UnBlack }) && MatchArray(right_pattern, { Stone::None, Stone::Black, Stone::Black, Stone::Black, Stone::UnBlack })) { ++sum_4_normal; continue; }
				if (MatchArray(left_pattern, { Stone::None, Stone::Black, Stone::UnBlack }) && MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::UnBlack })) { ++sum_4_normal; continue; }
				if (MatchArray(left_pattern, { Stone::Black, Stone::None, Stone::Black, Stone::UnBlack }) && MatchArray(right_pattern, { Stone::Black, Stone::UnBlack })) { ++sum_4_normal; continue; }
				if (MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::None, Stone::Black, Stone::UnBlack }) && MatchArray(right_pattern, { Stone::UnBlack })) { ++sum_4_normal; continue; }
				//01111
				if (MatchArray(left_pattern, { Stone::None, Stone::UnBlack }) && MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::Black, Stone::UnBlack })) { ++sum_4_normal; continue; }
				if (MatchArray(left_pattern, { Stone::Black, Stone::None, Stone::UnBlack }) && MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::UnBlack })) { ++sum_4_normal; continue; }
				if (MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::None, Stone::UnBlack }) && MatchArray(right_pattern, { Stone::Black, Stone::UnBlack })) { ++sum_4_normal; continue; }
				if (MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::Black, Stone::None, Stone::UnBlack }) && MatchArray(right_pattern, { Stone::UnBlack })) { ++sum_4_normal; continue; }

				// Strong Triplex(Katsu-san)
				int offset1 = 0, offset2 = 0;
				while (true) {
					//011100
					if (MatchArray(right_pattern, { Stone::None, Stone::None, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::None, Stone::UnBlack })) { offset1 = 1; offset2 = -3; break; }
					if (MatchArray(right_pattern, { Stone::Black, Stone::None, Stone::None, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::None, Stone::UnBlack })) { offset1 = 2; offset2 = -2; break; }
					if (MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::None, Stone::None, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::None, Stone::UnBlack })) { offset1 = 3; offset2 = -1; break; }
					//011010
					if (MatchArray(right_pattern, { Stone::None, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::None, Stone::Black, Stone::Black, Stone::None, Stone::UnBlack })) { offset1 = -1; break; }
					if (MatchArray(right_pattern, { Stone::None, Stone::Black, Stone::None, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::Black, Stone::None, Stone::UnBlack })) { offset1 = 1; break; }
					if (MatchArray(right_pattern, { Stone::Black, Stone::None, Stone::Black, Stone::None, Stone::UnBlack }) && MatchArray(left_pattern, { Stone::None, Stone::UnBlack })) { offset1 = 2; break; }
					//010110
					if (MatchArray(left_pattern, { Stone::None, Stone::UnBlack }) && MatchArray(right_pattern, { Stone::None, Stone::Black, Stone::Black, Stone::None, Stone::UnBlack })) { offset1 = 1; break; }
					if (MatchArray(left_pattern, { Stone::None, Stone::Black, Stone::None, Stone::UnBlack }) && MatchArray(right_pattern, { Stone::Black, Stone::None, Stone::UnBlack })) { offset1 = -1; break; }
					if (MatchArray(left_pattern, { Stone::Black, Stone::None, Stone::Black, Stone::None, Stone::UnBlack }) && MatchArray(right_pattern, { Stone::None, Stone::UnBlack })) { offset1 = -2; break; }
					//001110
					if (MatchArray(left_pattern, { Stone::None, Stone::None, Stone::UnBlack }) && MatchArray(right_pattern, { Stone::Black, Stone::Black, Stone::None, Stone::UnBlack })) { offset1 = -1; offset2 = 3; break; }
					if (MatchArray(left_pattern, { Stone::Black, Stone::None, Stone::None, Stone::UnBlack }) && MatchArray(right_pattern, { Stone::Black, Stone::None, Stone::UnBlack })) { offset1 = -2; offset2 = 2; break; }
					if (MatchArray(left_pattern, { Stone::Black, Stone::Black, Stone::None, Stone::None, Stone::UnBlack }) && MatchArray(right_pattern, { Stone::None, Stone::UnBlack })) { offset1 = -3; offset2 = 1; break; }
					break;
				}
				if (offset2 != 0) {
					auto pos_move2 = pos_move + it_offset * offset2;
					board_[position] = turn;
					auto score = CalcScore(pos_move2.GetPos(), turn);
					board_[position] = Stone::None;
					if (score != kScoreProhibit) {
						++sum_3;
					}
				}
				if (offset1 != 0) {
					auto pos_move2 = pos_move + it_offset * offset1;
					board_[position] = turn;
					auto score = CalcScore(pos_move2.GetPos(), turn);
					board_[position] = Stone::None;
					if (score != kScoreProhibit) {
						++sum_3;
					}
					continue;
				}
			}else {
				// Quintuplex(Go-ren)
				if (MatchArray(left_pattern, { Stone::White, Stone::White, Stone::White, Stone::White })) return kScoreInf;
				if (MatchArray(right_pattern, { Stone::White }) && MatchArray(left_pattern, { Stone::White, Stone::White, Stone::White})) return kScoreInf;
				if (MatchArray(right_pattern, { Stone::White, Stone::White }) && MatchArray(left_pattern, { Stone::White, Stone::White })) return kScoreInf;
				if (MatchArray(right_pattern, { Stone::White, Stone::White, Stone::White }) && MatchArray(left_pattern, { Stone::White })) return kScoreInf;
				if (MatchArray(right_pattern, { Stone::White, Stone::White, Stone::White, Stone::White })) return kScoreInf;

				/* If you make some Quadruplex of black, it's Prohibited move(Kinsyu) "Shi-Shi".
				* 1. BO|BBB|OB   "Ryoutou-no-ShiShi"
				* 2. BBO|BB|OBB  "Chouda-no-ShiShi"
				* 3. BBBO|B|OBBB "Souryu-no-ShiShi"
				* 一直線上に出来る四々で、両頭の四々・長蛇の四々・双龍の四々
				*/
				//
				if (MatchArray(right_pattern, { Stone::White, Stone::White, Stone::None, Stone::White }) && MatchArray(left_pattern, { Stone::None, Stone::White })) {sum_4_normal += 2; continue;}
				if (MatchArray(right_pattern, { Stone::White, Stone::None, Stone::White }) && MatchArray(left_pattern, { Stone::White, Stone::None, Stone::White })) {sum_4_normal += 2; continue;}
				if (MatchArray(right_pattern, { Stone::None, Stone::White }) && MatchArray(left_pattern, { Stone::White, Stone::White, Stone::None, Stone::White })) {sum_4_normal += 2; continue;}
				//
				if (MatchArray(right_pattern, { Stone::White, Stone::None, Stone::White }) && MatchArray(left_pattern, { Stone::None, Stone::White, Stone::White })) {sum_4_normal += 2; continue;}
				if (MatchArray(right_pattern, { Stone::None, Stone::White }) && MatchArray(left_pattern, { Stone::White, Stone::None, Stone::White, Stone::White })) {sum_4_normal += 2; continue;}
				//
				if (MatchArray(right_pattern, { Stone::None, Stone::White, Stone::White, Stone::White }) && MatchArray(left_pattern, { Stone::None, Stone::White, Stone::White, Stone::White })) {sum_4_normal += 2; continue;}

				// Strong Quadruplex(Shi-ren)
				if (MatchArray(right_pattern, { Stone::None }) && MatchArray(left_pattern, { Stone::White, Stone::White, Stone::White, Stone::None })) {++sum_4_strong; continue;}
				if (MatchArray(right_pattern, { Stone::White, Stone::None }) && MatchArray(left_pattern, { Stone::White, Stone::White, Stone::None })) {++sum_4_strong; continue;}
				if (MatchArray(right_pattern, { Stone::White, Stone::White, Stone::None }) && MatchArray(left_pattern, { Stone::White, Stone::None })) {++sum_4_strong; continue;}
				if (MatchArray(right_pattern, { Stone::White, Stone::White, Stone::White, Stone::None }) && MatchArray(left_pattern, { Stone::None })) {++sum_4_strong; continue;}

				// Normal Quadruplex(Katsu-shi)
				//11110
				if (MatchArray(right_pattern, { Stone::None }) && MatchArray(left_pattern, { Stone::White, Stone::White, Stone::White })) { ++sum_4_normal; continue; }
				if (MatchArray(right_pattern, { Stone::White, Stone::None }) && MatchArray(left_pattern, { Stone::White, Stone::White })) { ++sum_4_normal; continue; }
				if (MatchArray(right_pattern, { Stone::White, Stone::White, Stone::None }) && MatchArray(left_pattern, { Stone::White })) { ++sum_4_normal; continue; }
				if (MatchArray(right_pattern, { Stone::White, Stone::White, Stone::White, Stone::None })) { ++sum_4_normal; continue; }
				//11101
				if (MatchArray(left_pattern, { Stone::None, Stone::White, Stone::White, Stone::White })) { ++sum_4_normal; continue; }
				if (MatchArray(right_pattern, { Stone::None, Stone::White }) && MatchArray(left_pattern, { Stone::White, Stone::White })) { ++sum_4_normal; continue; }
				if (MatchArray(right_pattern, { Stone::White, Stone::None, Stone::White }) && MatchArray(left_pattern, { Stone::White })) { ++sum_4_normal; continue; }
				if (MatchArray(right_pattern, { Stone::White, Stone::White, Stone::None, Stone::White })) { ++sum_4_normal; continue; }
				//11011
				if (MatchArray(left_pattern, { Stone::White, Stone::None, Stone::White, Stone::White })) { ++sum_4_normal; continue; }
				if (MatchArray(right_pattern, { Stone::White }) && MatchArray(left_pattern, { Stone::None, Stone::White, Stone::White })) { ++sum_4_normal; continue; }
				if (MatchArray(right_pattern, { Stone::White, Stone::None, Stone::White, Stone::White })) { ++sum_4_normal; continue; }
				if (MatchArray(left_pattern, { Stone::White }) && MatchArray(right_pattern, { Stone::None, Stone::White, Stone::White })) { ++sum_4_normal; continue; }
				//10111
				if (MatchArray(right_pattern, { Stone::None, Stone::White, Stone::White, Stone::White })) { ++sum_4_normal; continue; }
				if (MatchArray(left_pattern, { Stone::None, Stone::White }) && MatchArray(right_pattern, { Stone::White, Stone::White })) { ++sum_4_normal; continue; }
				if (MatchArray(left_pattern, { Stone::White, Stone::None, Stone::White }) && MatchArray(right_pattern, { Stone::White })) { ++sum_4_normal; continue; }
				if (MatchArray(left_pattern, { Stone::White, Stone::White, Stone::None, Stone::White })) { ++sum_4_normal; continue; }
				//01111
				if (MatchArray(left_pattern, { Stone::None }) && MatchArray(right_pattern, { Stone::White, Stone::White, Stone::White })) { ++sum_4_normal; continue; }
				if (MatchArray(left_pattern, { Stone::White, Stone::None }) && MatchArray(right_pattern, { Stone::White, Stone::White })) { ++sum_4_normal; continue; }
				if (MatchArray(left_pattern, { Stone::White, Stone::White, Stone::None }) && MatchArray(right_pattern, { Stone::White })) { ++sum_4_normal; continue; }
				if (MatchArray(left_pattern, { Stone::White, Stone::White, Stone::White, Stone::None })) { ++sum_4_normal; continue; }

				// Strong Triplex(Katsu-san)
				//011100
				if (MatchArray(right_pattern, { Stone::None, Stone::None }) && MatchArray(left_pattern, { Stone::White, Stone::White, Stone::None })) { ++sum_3; continue; }
				if (MatchArray(right_pattern, { Stone::White, Stone::None, Stone::None }) && MatchArray(left_pattern, { Stone::White, Stone::None })) { ++sum_3; continue; }
				if (MatchArray(right_pattern, { Stone::White, Stone::White, Stone::None, Stone::None }) && MatchArray(left_pattern, { Stone::None })) { ++sum_3; continue; }
				//011010
				if (MatchArray(right_pattern, { Stone::None }) && MatchArray(left_pattern, { Stone::None, Stone::White, Stone::White, Stone::None })) { ++sum_3; continue; }
				if (MatchArray(right_pattern, { Stone::None, Stone::White, Stone::None }) && MatchArray(left_pattern, { Stone::White, Stone::None })) { ++sum_3; continue; }
				if (MatchArray(right_pattern, { Stone::White, Stone::None, Stone::White, Stone::None }) && MatchArray(left_pattern, { Stone::None })) { ++sum_3; continue; }
				//010110
				if (MatchArray(left_pattern, { Stone::None }) && MatchArray(right_pattern, { Stone::None, Stone::White, Stone::White, Stone::None })) { ++sum_3; continue; }
				if (MatchArray(left_pattern, { Stone::None, Stone::White, Stone::None }) && MatchArray(right_pattern, { Stone::White, Stone::None })) { ++sum_3; continue; }
				if (MatchArray(left_pattern, { Stone::White, Stone::None, Stone::White, Stone::None }) && MatchArray(right_pattern, { Stone::None })) { ++sum_3; continue; }
				//001110
				if (MatchArray(left_pattern, { Stone::None, Stone::None }) && MatchArray(right_pattern, { Stone::White, Stone::White, Stone::None })) { ++sum_3; continue; }
				if (MatchArray(left_pattern, { Stone::White, Stone::None, Stone::None }) && MatchArray(right_pattern, { Stone::White, Stone::None })) { ++sum_3; continue; }
				if (MatchArray(left_pattern, { Stone::White, Stone::White, Stone::None, Stone::None }) && MatchArray(right_pattern, { Stone::None })) { ++sum_3; continue; }
			}
		}
		if (sum_4_strong + sum_4_normal >= 2) return (turn == Stone::Black ? kScoreProhibit : 80);
		if (sum_3 >= 2) return (turn == Stone::Black ? kScoreProhibit : 60);
		if (sum_4_strong + sum_4_normal == 1 && sum_3 == 1) return 70;
		if (sum_4_strong == 1) return 90;
		if (sum_4_normal == 1) return 50;
		if (sum_3 == 1) return 10;
		return 0;
	}
};

int main(int argc, char *argv[]) {
	// Read arguments
	if (argc < 3) {
		cout << "Usage: I401 board_text turn_text" << endl
			 << endl
			 << "board_text     : board data of stone or blank" << endl
			 << "turn_text      : turn data of stone or blank" << endl
			 << "stone or blank : black(*), white(O), blank(-)" << endl;
	}
	try {
		Board board(argv[1], argv[2]);
		cout << board.NextMove() << endl;
	}
	catch (const std::exception& er) {
		std::cerr << er.what() << endl;
		cout << -1 << endl;
		return 1;
	}
	return 0;
}
