#include<iostream>
#include<string>
using std::cout;
using std::endl;
using std::string;

int main(int argc, char *argv[]) {
	if (argc < 3) {
		cout << "Usage: I401 noard_text turn_text" << endl
			 << endl
			 << "board_text     : board data of stone or blank" << endl
			 << "turn_text      : turn data of stone or blank" << endl
			 << "stone or blank : black(*), white(O), blank(-)" << endl;
	}
	cout << "test" << endl;
}
