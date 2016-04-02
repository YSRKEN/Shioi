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

enum Stone : uint8_t {
	Blank,
	Black,
	White
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
		for (size_t p = 0; p < kBoardSize * kBoardSize; ++p) {
			if (!IsValidMove(p, turn_)) continue;
			next_move.push_back(p);
		}
		return (next_move.size() == 0 ? -1 : next_move[std::uniform_int_distribution<size_t>{0, next_move.size() - 1}(mt)]);
	}
	bool IsValidMove(const size_t position, const Stone turn) const noexcept {
		if (board_[position] != Stone::Blank) return false;
		return true;
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
