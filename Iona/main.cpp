#include<iostream>
#include"Board.hpp"

int main(int argc, char *argv[]) {
	using std::cout;
	using std::endl;
	// Read arguments
	if (argc < 4) {
		std::cout << "Usage: Iona [board_text] [turn_text] [depth] (--debug)" << endl
			<< endl
			<< "board_text     : board data of stone or blank" << endl
			<< "turn_text      : turn data of stone or blank" << endl
			<< "depth          : depth of thinking" << endl
			<< R"(--debug(opt)   : put debug info to std::err if this string is "--debug")" << endl
			<< endl
			<< "stone or blank : black(B), white(W), blank(N)" << endl
			<< "return value   : " << endl
			<< R"(    When you specify "--debug", return >=0[move] | -1[can't move] | -2[give up].)" << endl
			<< "    On the other case, return 0[no error] or -1[error]" << endl;
		return -1;
	}
}
