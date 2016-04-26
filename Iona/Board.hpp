/**
* @file Board.hpp
* @brief ゲームで使用する盤面を定義する
* @author YSR
* @date 2016/04/26
*/

#include<iostream>
#include"BitBoard.hpp"

using std::cout;
using std::endl;

/**
* @class
* @brief Board class for Renju game
*/
class Board {
	BitBoard black_board_, white_board_;
public:
	/**
	* @fn
	* @brief Constructor
	* @param board_text /^[BWN]{225}$/
	* @param turn_text /^[BWN]$/
	* @detail Reset data of board for Renju game
	*/
	Board(const char board_text[], const char turn_text[]) {

	}
	/**
	* @fn
	* @brief Put text of board for debug
	*/
	void PutBoard() {

	}
};
