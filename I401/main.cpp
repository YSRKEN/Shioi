#include<array>
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
const int kScoreInf = 1000;

enum Stone : uint8_t {
	None,
	Black,
	White
};

typedef array<Stone, kBoardSize * kBoardSize> StoneBoard;

string PositionToString(const size_t p) {
	const static string kPositionStringX = "abcdefghijklmno";
	const static string kPositionStringY = "123456789ABCDEF";
	return kPositionStringX.substr(p % kBoardSize, 1) + kPositionStringY.substr(p / kBoardSize, 1);
}

class Board {
	StoneBoard board_;
	Stone turn_;
public:
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
	int NextMove(const size_t depth, bool debug_flg = false) {
		for (size_t y = 0; y < kBoardSize; ++y) {
			for (size_t x = 0; x < kBoardSize; ++x) {
				size_t p = x + y * kBoardSize;
				cout << board_[p] << " ";
			}
			cout << endl;
		}
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
