/**
* @file Board.hpp
* ~japanese	@brief ゲームで使用する盤面を定義する
* ~english	@brief Definition of Board class for Renju game
* @author YSR
* @date 2016/04/26
*/
#pragma once
#include<cstring>
#include<iostream>
#include"constant.hpp"
#include"misc_functions.hpp"
#include"types.hpp"
#include"BitBoard.hpp"

using std::cout;
using std::endl;

/**
* @class Board
* @brief Board class for Renju game
*/
class Board {
	BitBoard black_board_, white_board_;
	Stone turn_;
	/**
	* @fn ToStone
	* @brief Convert char to Stone
	* @param str /[BWN]/
	* @return 0-2[Stone::Black, Stone::White, Stone::None]
	* ~japanese	@detail strがkStoneStringのどれにも当てはまらない場合は例外を投げる
	* ~english	@detail If you can't find str in kStoneString, this throw exception.
	*/
	static Stone ToStone(const char str) {
		constexpr static char kStoneString[] = "BWN";
		auto find_ptr = strchr(kStoneString, str);
		if (find_ptr != NULL) {
			return static_cast<Stone>(find_ptr - kStoneString);
		}
		else {
			throw std::invalid_argument("Can't read board/turn data.");
		}
	}
	/**
	* @fn SetStone
	* @brief Set stone in board
	*/
	void SetStone(const size_t position, const Stone stone) noexcept{
		if (stone == Stone::Black) black_board_.SetBit(position);
		if (stone == Stone::White) white_board_.SetBit(position);
	}
	/**
	* @fn GetStone
	* @brief Get stone in board
	*/
	Stone GetStone(const size_t position) const noexcept {
		if (black_board_.GetBit(position)) return Stone::Black;
		if (white_board_.GetBit(position)) return Stone::White;
		return Stone::None;
	}
public:
	/**
	* @fn Board
	* @brief Constructor
	* @param board_text /^[BWN]{225}$/
	* @param turn_text /^[BWN]$/
	* @detail Reset data of board for Renju game
	*/
	Board(const char board_text[], const char turn_text[]) {
		BitBoard::Initialize();
		// Read board_text
		if (strlen(board_text) < kAllBoardSize) {
			throw std::invalid_argument("Too short board-text size!");
		}
		for (size_t position = 0; position < kAllBoardSize; ++position) {
			auto stone = ToStone(board_text[position]);
			SetStone(position, stone);
		}
	}
	/**
	* @fn PutBoard
	* @brief Put text of board for debug
	*/
	void PutBoard() const noexcept{
		for (size_t y = 0; y < kBoardSize; ++y) {
			for (size_t x = 0; x < kBoardSize; ++x) {
				auto position = ToPosition(x, y);
				auto stone = GetStone(position);
				switch (stone) {
				case Stone::Black:
					cout << "●";
					break;
				case Stone::White:
					cout << "○";
					break;
				case Stone::None:
					cout << "┼";
					break;
				}
			}
			cout << endl;
		}
		return;
	}
	void NextMove() {
		cout << "-1" << endl;	//! dummy
	}
};
