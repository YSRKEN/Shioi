#include<array>
#include<cstdint>
#include<iostream>
#include<string>

using std::array;
using std::cout;
using std::endl;
using std::string;

const size_t kBoardSize = 15;

enum Stone : uint8_t {
	Blank,
	Black,
	White
};

class Board {
	array<array<Stone, kBoardSize>, kBoardSize> board_;
	Stone turn_;
public:
	Board(const char board_text[], const char turn_text[]) {
		const string board_text_str(board_text);
		if (board_text_str.size() < kBoardSize * kBoardSize) {
			throw std::exception("Too short board-text size!");
		}
		for (size_t y = 0; y < kBoardSize; ++y) {
			cout << board_text_str.substr(y * kBoardSize, kBoardSize) << endl;
		}
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
		Board board_text(argv[1], argv[2]);
	}
	catch (const std::exception& er) {
//		std::cerr << er.what() << endl;
		cout << "**" << endl;
		return 1;
	}
}
