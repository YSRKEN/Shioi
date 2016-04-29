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
			auto dir = static_cast<Direction>(dir_);
			auto pattern_black_left = black_board_.ShiftLeft(dir);
			auto pattern_black_right = black_board_.ShiftRight(dir);
			auto pattern_white_left = white_board_.ShiftLeft(dir);
			auto pattern_white_right = white_board_.ShiftRight(dir);
			REP(shift, kMaxShifts - 1) {
				shift_Pattern[dir][Stone::Black][Side::Left][shift] = pattern_black_left;
				shift_Pattern[dir][Stone::White][Side::Left][shift] = pattern_white_left;
				shift_Pattern[dir][Stone::None][Side::Left][shift] = !(pattern_black_left | pattern_white_left);
				shift_Pattern[dir][Stone::NonBlack][Side::Left][shift] = !pattern_black_left;
				shift_Pattern[dir][Stone::Black][Side::Right][shift] = pattern_black_right;
				shift_Pattern[dir][Stone::White][Side::Right][shift] = pattern_white_right;
				shift_Pattern[dir][Stone::None][Side::Right][shift] = !(pattern_black_right | pattern_white_right);
				shift_Pattern[dir][Stone::NonBlack][Side::Right][shift] = !pattern_black_right;
				pattern_black_left.ShiftLeftD(dir);
				pattern_white_left.ShiftLeftD(dir);
				pattern_black_right.ShiftRightD(dir);
				pattern_white_right.ShiftRightD(dir);
			}
			shift_Pattern[dir][Stone::Black][Side::Left][kMaxShifts - 1] = pattern_black_left;
			shift_Pattern[dir][Stone::White][Side::Left][kMaxShifts - 1] = pattern_white_left;
			shift_Pattern[dir][Stone::None][Side::Left][kMaxShifts - 1] = !(pattern_black_left | pattern_white_left);
			shift_Pattern[dir][Stone::NonBlack][Side::Left][kMaxShifts - 1] = !pattern_black_left;
			shift_Pattern[dir][Stone::Black][Side::Right][kMaxShifts - 1] = pattern_black_right;
			shift_Pattern[dir][Stone::White][Side::Right][kMaxShifts - 1] = pattern_white_right;
			shift_Pattern[dir][Stone::None][Side::Right][kMaxShifts - 1] = !(pattern_black_right | pattern_white_right);
			shift_Pattern[dir][Stone::NonBlack][Side::Right][kMaxShifts - 1] = !pattern_black_right;
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
			auto &BL0 = pattern[dir][Stone::Black][Side::Left][0], &NL0 = pattern[dir][Stone::None][Side::Left][0];
			auto &BL1 = pattern[dir][Stone::Black][Side::Left][1], &NL1 = pattern[dir][Stone::None][Side::Left][1];
			auto &BL2 = pattern[dir][Stone::Black][Side::Left][2], &NL2 = pattern[dir][Stone::None][Side::Left][2];
			auto &BL3 = pattern[dir][Stone::Black][Side::Left][3], &NL3 = pattern[dir][Stone::None][Side::Left][3];
			auto &BL4 = pattern[dir][Stone::Black][Side::Left][4], &NL4 = pattern[dir][Stone::None][Side::Left][4];
			auto &BR0 = pattern[dir][Stone::Black][Side::Right][0], &NR0 = pattern[dir][Stone::None][Side::Right][0];
			auto &BR1 = pattern[dir][Stone::Black][Side::Right][1], &NR1 = pattern[dir][Stone::None][Side::Right][1];
			auto &BR2 = pattern[dir][Stone::Black][Side::Right][2], &NR2 = pattern[dir][Stone::None][Side::Right][2];
			auto &BR3 = pattern[dir][Stone::Black][Side::Right][3], &NR3 = pattern[dir][Stone::None][Side::Right][3];
			auto &BR4 = pattern[dir][Stone::Black][Side::Right][4], &NR4 = pattern[dir][Stone::None][Side::Right][4];

			//! [BBBBBB]
			//! YBBBBB
			choren_mask |= (BL0 & BL1 & BL2 & BL3 & BL4);
			//! BYBBBB
			choren_mask |= (BL0 & BL1 & BL2 & BL3 /**/& BR0);
			//! BBYBBB
			choren_mask |= (BL0 & BL1 & BL2 /**/& BR0 & BR1);
			//! BBBYBB
			choren_mask |= (BL0 & BL1 /**/& BR0 & BR1 & BR2);
			//! BBBBYB
			choren_mask |= (BL0 /**/& BR0 & BR1 & BR2 & BR3);
			//! BBBBBY
			choren_mask |= (BR0 & BR1 & BR2 & BR3 & BR4);
		}
		return choren_mask;
	}
	/**
	* ~japanese	@brief 一直線上の四々を起こす石の位置を算出する(黒石用)
	* ~english	@brief Make position mask of Shi-Shi on 1Line for Stone::Black
	*/
	BitBoard CalcLineShiShiMaskB(const ShiftPattern &pattern) {
		BitBoard shishi1_mask;
		REP(dir, Direction::Directions) {
			auto &BL0 = pattern[dir][Stone::Black][Side::Left][0], &NL0 = pattern[dir][Stone::None][Side::Left][0], &bL0 = pattern[dir][Stone::NonBlack][Side::Left][0];
			auto &BL1 = pattern[dir][Stone::Black][Side::Left][1], &NL1 = pattern[dir][Stone::None][Side::Left][1], &bL1 = pattern[dir][Stone::NonBlack][Side::Left][1];
			auto &BL2 = pattern[dir][Stone::Black][Side::Left][2], &NL2 = pattern[dir][Stone::None][Side::Left][2], &bL2 = pattern[dir][Stone::NonBlack][Side::Left][2];
			auto &BL3 = pattern[dir][Stone::Black][Side::Left][3], &NL3 = pattern[dir][Stone::None][Side::Left][3], &bL3 = pattern[dir][Stone::NonBlack][Side::Left][3];
			auto &BL4 = pattern[dir][Stone::Black][Side::Left][4], &NL4 = pattern[dir][Stone::None][Side::Left][4], &bL4 = pattern[dir][Stone::NonBlack][Side::Left][4];
			auto &BR0 = pattern[dir][Stone::Black][Side::Right][0], &NR0 = pattern[dir][Stone::None][Side::Right][0], &bR0 = pattern[dir][Stone::NonBlack][Side::Right][0];
			auto &BR1 = pattern[dir][Stone::Black][Side::Right][1], &NR1 = pattern[dir][Stone::None][Side::Right][1], &bR1 = pattern[dir][Stone::NonBlack][Side::Right][1];
			auto &BR2 = pattern[dir][Stone::Black][Side::Right][2], &NR2 = pattern[dir][Stone::None][Side::Right][2], &bR2 = pattern[dir][Stone::NonBlack][Side::Right][2];
			auto &BR3 = pattern[dir][Stone::Black][Side::Right][3], &NR3 = pattern[dir][Stone::None][Side::Right][3], &bR3 = pattern[dir][Stone::NonBlack][Side::Right][3];
			auto &BR4 = pattern[dir][Stone::Black][Side::Right][4], &NR4 = pattern[dir][Stone::None][Side::Right][4], &bR4 = pattern[dir][Stone::NonBlack][Side::Right][4];

			//! [XBOYYYOBX]
			//! XBOYBBOBX
			shishi1_mask |= (BL0 & BL1 & NL2 & BL3 & bL4 /**/& NR0 & BR1 & bR2);
			//! XBOBYBOBX
			shishi1_mask |= (BL0 & NL1 & BL2 & bL3 /**/& BR0 & NR1 & BR2 & bR3);
			//! XBOBBYOBX
			shishi1_mask |= (NL0 & BL1 & bL2 /**/& BR0 & BR1 & NR2 & BR3 & bR4);
			//! [XBBOYYOBBX]
			//! XBBOYBOBBX
			shishi1_mask |= (BL0 & NL1 & BL2 & BL3 & bL4 /**/& NR0 & BR1 & BR2 & bR3);
			//! XBBOBYOBBX
			shishi1_mask |= (NL0 & BL1 & BL2 & bL3 /**/& BR0 & NR1 & BR2 & BR3 & bR4);
			//! [XBBBOYOBBBX]
			shishi1_mask |= (NR0 & BR1 & BR2 & BR3 & bR4 /**/& NL0 & BL1 & BL2 & BL3 & bL4);
		}
		return shishi1_mask;
	}
	/**
	* ~japanese	@brief 四連を作る石の位置を算出する(黒石用)
	* ~english	@brief Make position mask of Shi-ren for Stone::Black
	*/
	array<BitBoard, Direction::Directions> CalcShirenMaskB(const ShiftPattern &pattern) {
		array<BitBoard, Direction::Directions> shiren_mask;
		REP(dir, Direction::Directions) {
			auto &BL0 = pattern[dir][Stone::Black][Side::Left][0], &NL0 = pattern[dir][Stone::None][Side::Left][0], &bL0 = pattern[dir][Stone::NonBlack][Side::Left][0];
			auto &BL1 = pattern[dir][Stone::Black][Side::Left][1], &NL1 = pattern[dir][Stone::None][Side::Left][1], &bL1 = pattern[dir][Stone::NonBlack][Side::Left][1];
			auto &BL2 = pattern[dir][Stone::Black][Side::Left][2], &NL2 = pattern[dir][Stone::None][Side::Left][2], &bL2 = pattern[dir][Stone::NonBlack][Side::Left][2];
			auto &BL3 = pattern[dir][Stone::Black][Side::Left][3], &NL3 = pattern[dir][Stone::None][Side::Left][3], &bL3 = pattern[dir][Stone::NonBlack][Side::Left][3];
			auto &BL4 = pattern[dir][Stone::Black][Side::Left][4], &NL4 = pattern[dir][Stone::None][Side::Left][4], &bL4 = pattern[dir][Stone::NonBlack][Side::Left][4];
			auto &BR0 = pattern[dir][Stone::Black][Side::Right][0], &NR0 = pattern[dir][Stone::None][Side::Right][0], &bR0 = pattern[dir][Stone::NonBlack][Side::Right][0];
			auto &BR1 = pattern[dir][Stone::Black][Side::Right][1], &NR1 = pattern[dir][Stone::None][Side::Right][1], &bR1 = pattern[dir][Stone::NonBlack][Side::Right][1];
			auto &BR2 = pattern[dir][Stone::Black][Side::Right][2], &NR2 = pattern[dir][Stone::None][Side::Right][2], &bR2 = pattern[dir][Stone::NonBlack][Side::Right][2];
			auto &BR3 = pattern[dir][Stone::Black][Side::Right][3], &NR3 = pattern[dir][Stone::None][Side::Right][3], &bR3 = pattern[dir][Stone::NonBlack][Side::Right][3];
			auto &BR4 = pattern[dir][Stone::Black][Side::Right][4], &NR4 = pattern[dir][Stone::None][Side::Right][4], &bR4 = pattern[dir][Stone::NonBlack][Side::Right][4];
			auto &ML1 = kBitMaskArray[dir][Side::Right][0], &ML2 = kBitMaskArray[dir][Side::Right][1];
			auto &ML3 = kBitMaskArray[dir][Side::Right][2] , &ML4 = kBitMaskArray[dir][Side::Right][3];
			auto &MR1 = kBitMaskArray[dir][Side::Left][0], &MR2 = kBitMaskArray[dir][Side::Left][1];
			auto &MR3 = kBitMaskArray[dir][Side::Left][2], &MR4 = kBitMaskArray[dir][Side::Left][3];

			//! [XOBBBBOX]
			//! XOBBBYOX
			shiren_mask[dir] |= (NL0 & bL1 & ML1/**/& BR0 & BR1 & BR2 & NR3 & bR4 & MR4);
			//! XOBBYBOX
			shiren_mask[dir] |= (BL0 & NL1 & bL2 & ML2/**/& BR0 & BR1 & NR2 & bR3 & MR3);
			//! XOBYBBOX
			shiren_mask[dir] |= (BR0 & NR1 & bR2 & MR2/**/& BL0 & BL1 & NL2 & bL3 & ML3);
			//! XOYBBBOX
			shiren_mask[dir] |= (NR0 & bR1 & MR1/**/& BL0 & BL1 & BL2 & NL3 & bL4 & ML4);
		}
		return shiren_mask;
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
		auto shishi1_mask = CalcLineShiShiMaskB(shift_pattern);
		invalid_mask |= shishi1_mask;
		//! Shi-ren check
		auto shiren_mask = CalcShirenMaskB(shift_pattern);

		choren_mask.PutBoard();
		shishi1_mask.PutBoard();
		BitBoard(shiren_mask[0] | shiren_mask[1] | shiren_mask[2] | shiren_mask[3]).PutBoard();
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
		BitBoard(invalid_mask).PutBoard();
		REP(position, kAllBoardSize) {
			//! You can only move at Stone::None in Board
			if (!IsZero(kPositionArray[position] & invalid_mask)) continue;
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
