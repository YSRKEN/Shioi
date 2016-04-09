#include<array>
#include<cstdint>
#include<iostream>
#include<random>
#include<string>
#include<vector>

// using declaration
using std::array;
using std::cout;
using std::endl;
using std::string;
using std::vector;

// const value declaration
const size_t kBoardSize = 15;
const int kScoreInf = 1000;
const string kPositionStringX = "abcdefghijklmno";
const string kPositionStringY = "123456789ABCDEF";
enum Stone : uint8_t {
	None,
	Black,
	White
};

// typedef declaration
typedef std::pair<size_t, int> Score;
typedef std::pair<size_t, bool> Result;

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

// Board class
class Board {
	array<Stone, kBoardSize * kBoardSize> board_;
	Stone turn_;
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
	}
	// Put board
	void PutBoard() {
		const static string kStoneString2[] = { "Ñ©", "Åú", "Åõ" };
		for (size_t y = 0; y < kBoardSize; ++y) {
			for (size_t x = 0; x < kBoardSize; ++x) {
				size_t p = x + y * kBoardSize;
				cout << kStoneString2[board_[p]];
			}
			cout << endl;
		}
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
		// Row(Ñü)
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
				}else{
					if (rlengh + llengh >= 4) return Result(p, true);
				}
			}
		}
		// Column(Ñ†)
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
				}else {
					if (rlengh + llengh >= 4) return Result(p, true);
				}
			}
		}
		// Diagonally right(Å^)
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
		// Diagonally left(Å_)
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
	// Thinking next move
	int NextMove(const size_t depth, bool debug_flg = false) {
		// Opening move
		auto result = GetOpeningMove(turn_);
		if (result.second) return result.first;
		// If you can make Go-ren, you must do.
		result = FindGorenMove(turn_);
		if (result.second) return result.first;
		// If enemy can make Go-ren, you must block it.
		result = FindGorenMove(EnemyTurn(turn_));
		if (result.second) return result.first;
		// Search of quadruplex's problem(Shi-Oi Tsume)

		//PutBoard();
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
