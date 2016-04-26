/**
* @file main.cpp
* @brief main関数を記述する
* @author YSR
* @date 2016/04/26
*/

#include<iostream>
#include"Board.hpp"

using std::cout;
using std::endl;

/**
* @brief main function
* @param argc argument's count
* @param argv argument's value
* @return 0[no error] or -1[error]
* @detail Usage: Iona [board_text] [turn_text] [depth] (--debug)
*/
int main(int argc, char *argv[]) {
	/**
	* Read arguments
	*/
	if (argc < 4) {
		std::cout << "Usage: Iona [board_text] [turn_text] [depth] (--debug)" << endl
			<< endl
			<< "board_text     : board data of stone or blank" << endl
			<< "turn_text      : turn data of stone or blank" << endl
			<< "depth          : depth of thinking" << endl
			<< R"(--debug(opt)   : put debug info to std::err if this string is "--debug")" << endl
			<< endl
			<< "stone or blank : black(B), white(W), blank(N)" << endl
			<< "return value   : return 0[no error] or -1[error]" << endl;
		return -1;
	}
	try {
		/**
		* Calc next move
		*/
		Board board(argv[1], argv[2]);
		board.PutBoard();
	}
	catch (const std::exception& er) {
		std::cerr << er.what() << endl;
		cout << -1 << endl;
		return -1;
	}
}
