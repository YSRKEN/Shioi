/**
* @file Board.hpp
* ~japanese	@brief ゲームで使用する盤面を定義する
* ~english	@brief Definition of Board class for Renju game
* @author YSR
* @date 2016/04/26
*/
#pragma once
#include<cstring>
#include<array>
#include<iostream>
#include<random>
#include<vector>
#include"constant.hpp"
#include"misc_functions.hpp"
#include"types.hpp"
#include"Optional.hpp"
#include"BitBoard.hpp"

using std::array;
using std::cout;
using std::endl;
using std::vector;

//! Definition of random_device
std::random_device rd;
//! Definition of pseudo-random generator
std::mt19937 mt(rd());

//! Definition of shift pattern
using ShiftPattern = array<array<array<array<BitBoard, kMaxShifts>, Side::Sides>, Stone::Stones>, Direction::Directions>;

/**
* @fn RandInt
* @return Integer random number in [0, n)
*/
inline size_t RandInt(const size_t n) {
	return std::uniform_int_distribution<size_t>{0, n - 1}(mt);
}

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
	/**
	* ~japanese	@brief 方向・先後・左右・シフト数に応じたシフトパターンを作成する
	* ~english	@brief Make shift pattern
	*/
	ShiftPattern GetShiftPattern() {
		ShiftPattern shift_Pattern;
		REP(dir_, Direction::Directions) {
			const auto dir = static_cast<Direction>(dir_);
			auto pattern = black_board_;
			for (auto& shift : shift_Pattern[dir][Stone::Black][Side::Left])  { pattern <<= dir; shift = pattern; }
			pattern = black_board_;
			for (auto& shift : shift_Pattern[dir][Stone::Black][Side::Right]) { pattern >>= dir; shift = pattern; }
			pattern = white_board_;
			for (auto& shift : shift_Pattern[dir][Stone::White][Side::Left])  { pattern <<= dir; shift = pattern; }
			pattern = white_board_;
			for (auto& shift : shift_Pattern[dir][Stone::White][Side::Right]) { pattern >>= dir; shift = pattern; }
		}
		return shift_Pattern;
	}
	/**
	* ~japanese	@brief 長連を起こす石の位置を算出する(黒石用)
	* ~english	@brief Make position mask of Cho-ren for Stone::Black
	*/
	BitBoard CalcChorenMaskB(const ShiftPattern &pattern) {
		BitBoard choren_mask;
		REP(dir, Direction::Directions) {
			//BitBoard temp;
			//! BBBBBX
			choren_mask = choren_mask | (pattern[dir][Stone::Black][Side::Left][0] & pattern[dir][Stone::Black][Side::Left][1]
				& pattern[dir][Stone::Black][Side::Left][2] & pattern[dir][Stone::Black][Side::Left][3]
				& pattern[dir][Stone::Black][Side::Left][4]);
			//! BBBBXB
			choren_mask = choren_mask | (pattern[dir][Stone::Black][Side::Left][0] & pattern[dir][Stone::Black][Side::Left][1]
				& pattern[dir][Stone::Black][Side::Left][2] & pattern[dir][Stone::Black][Side::Left][3]
				& pattern[dir][Stone::Black][Side::Right][0]);
			//! BBBXBB
			choren_mask = choren_mask | (pattern[dir][Stone::Black][Side::Left][0] & pattern[dir][Stone::Black][Side::Left][1]
				& pattern[dir][Stone::Black][Side::Left][2] & pattern[dir][Stone::Black][Side::Right][0]
				& pattern[dir][Stone::Black][Side::Right][1]);
			//! BBXBBB
			choren_mask = choren_mask | (pattern[dir][Stone::Black][Side::Left][0] & pattern[dir][Stone::Black][Side::Left][1]
				& pattern[dir][Stone::Black][Side::Right][0] & pattern[dir][Stone::Black][Side::Right][1]
				& pattern[dir][Stone::Black][Side::Right][2]);
			//! BXBBBB
			choren_mask = choren_mask | (pattern[dir][Stone::Black][Side::Left][0] & pattern[dir][Stone::Black][Side::Right][0]
				& pattern[dir][Stone::Black][Side::Right][1] & pattern[dir][Stone::Black][Side::Right][2]
				& pattern[dir][Stone::Black][Side::Right][3]);
			//! XBBBBB
			choren_mask = choren_mask | (pattern[dir][Stone::Black][Side::Right][0] & pattern[dir][Stone::Black][Side::Right][1]
				& pattern[dir][Stone::Black][Side::Right][2] & pattern[dir][Stone::Black][Side::Right][3]
				& pattern[dir][Stone::Black][Side::Right][4]);
		}
		return choren_mask;
	}
	/**
	* ~japanese	@brief 禁手で打てない位置のビットを立てたマスクを作成する
	* ~english	@brief Calc invaild position's mask
	*/
	BitBoard CalcInValidMask() {
		//! shift_pattern[Black, White][Row, Column, DiagR, DiagL][Left, Right][Shifts]
		auto shift_pattern = GetShiftPattern();
		/*REP(dir, Direction::Directions) {
			REP(stone, Stone::Stones) {
				REP(side, Side::Sides) {
					REP(shift, kMaxShifts) {
						cout << "Dir:" << dir << " Stone:" << stone << " Side:" << side << " Shift:" << (shift + 1) << endl;
						shift_pattern[dir][stone][side][shift].PutBoard();
						int i = 1;
					}
				}
			}
		}*/
		BitBoard invalid_mask;
		//! Cho-ren check
		auto choren_mask = CalcChorenMaskB(shift_pattern);
		invalid_mask |= choren_mask;
		//! Shi-Shi on 1 line check

		choren_mask.PutBoard();
		
		return invalid_mask;	//! dummy
	}
	/**
	* @fn FindRandomMove
	* @return Next Move(Random)
	*/
	optional<size_t> FindRandomMove() {
		vector<size_t> list;
		auto invalid_mask = black_board_ | white_board_;
		if (turn_ == Stone::Black) {
			invalid_mask = invalid_mask | CalcInValidMask();
		}
		REP(position, kAllBoardSize) {
			//! You can only move at Stone::None in Board
			if (0 != (BitBoard::kPositionArray[position] & invalid_mask)) continue;
			list.push_back(position);
		}
		if(list.size() > 0) return optional<size_t>(list[RandInt(list.size())]);
		return optional<size_t>(-1);
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
		// Read board_text
		if (strlen(board_text) < kAllBoardSize) {
			throw std::invalid_argument("Too short board-text size!");
		}
		REP(position, kAllBoardSize) {
			auto stone = ToStone(board_text[position]);
			SetStone(position, stone);
		}
		turn_ = ToStone(turn_text[0]);
	}
	/**
	* @fn PutBoard
	* @brief Put text of board for debug
	*/
	void PutBoard() const noexcept{
		REP(y, kBoardSize) {
			REP(x, kBoardSize) {
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
					if (position == ToPosition(0, 0)) {
						cout << "┌";
					}else if(position == ToPosition(0, kBoardSize - 1)){
						cout << "└";
					}else if(position == ToPosition(kBoardSize - 1, 0)){
						cout << "┐";
					}else if(position == ToPosition(kBoardSize - 1, kBoardSize - 1)){
						cout << "┘";
					}else if (position == ToPosition(3, 3)
					       || position == ToPosition(3, 11)
					       || position == ToPosition(11, 3)
					       || position == ToPosition(11, 11)
					       || position == ToPosition(7, 7)) {
						cout << "╋";
					}else if(x == 0){
						cout << "├";
					}else if(x == kBoardSize - 1){
						cout << "┤";
					}else if(y == 0){
						cout << "┬";
					}else if(y == kBoardSize - 1){
						cout << "┴";
					}else{
						cout << "┼";
					}
					break;
				}
			}
			cout << endl;
		}
		return;
	}
	/**
	* @fn NextMove
	* @brief thinking next move
	*/
	int NextMove() {
		auto result = FindRandomMove();
		if (result) return *result;
		return -1;
	}
};
