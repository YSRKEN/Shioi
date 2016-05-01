/**
* @file Board.hpp
* ~japanese	@brief ゲームで使用する盤面を定義する
* ~english	@brief Definition of Board class for Renju game
* @author YSR
* @date 2016/04/26
*/
#pragma once
#include<chrono>
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
//! Definition of Renju pattern
using RenjuPattern = array<BitBoard, Direction::Directions>;

/**
* @class Board
* @brief Board class for Renju game
*/
class Board {
	BitBoard black_board_, white_board_;
	Stone turn_;
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
		if (black_board_[position]) return Stone::Black;
		if (white_board_[position]) return Stone::White;
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
			auto pattern_black_right = black_board_ << dir;
			auto pattern_black_left = black_board_ >> dir;
			auto pattern_white_right = white_board_ << dir;
			auto pattern_white_left = white_board_ >> dir;
			REP(shift, kMaxShifts - 1) {
				shift_Pattern[dir][Stone::Black][Side::Right][shift] = pattern_black_right;
				shift_Pattern[dir][Stone::White][Side::Right][shift] = pattern_white_right;
				shift_Pattern[dir][Stone::None][Side::Right][shift] = !(pattern_black_right | pattern_white_right);
				shift_Pattern[dir][Stone::NonBlack][Side::Right][shift] = !pattern_black_right;
				shift_Pattern[dir][Stone::Black][Side::Left][shift] = pattern_black_left;
				shift_Pattern[dir][Stone::White][Side::Left][shift] = pattern_white_left;
				shift_Pattern[dir][Stone::None][Side::Left][shift] = !(pattern_black_left | pattern_white_left);
				shift_Pattern[dir][Stone::NonBlack][Side::Left][shift] = !pattern_black_left;
				pattern_black_right <<= dir;
				pattern_white_right >>= dir;
				pattern_black_left <<= dir;
				pattern_white_left >>= dir;
			}
			shift_Pattern[dir][Stone::Black][Side::Right][kMaxShifts - 1] = pattern_black_right;
			shift_Pattern[dir][Stone::White][Side::Right][kMaxShifts - 1] = pattern_white_right;
			shift_Pattern[dir][Stone::None][Side::Right][kMaxShifts - 1] = !(pattern_black_right | pattern_white_right);
			shift_Pattern[dir][Stone::NonBlack][Side::Right][kMaxShifts - 1] = !pattern_black_right;
			shift_Pattern[dir][Stone::Black][Side::Left][kMaxShifts - 1] = pattern_black_left;
			shift_Pattern[dir][Stone::White][Side::Left][kMaxShifts - 1] = pattern_white_left;
			shift_Pattern[dir][Stone::None][Side::Left][kMaxShifts - 1] = !(pattern_black_left | pattern_white_left);
			shift_Pattern[dir][Stone::NonBlack][Side::Left][kMaxShifts - 1] = !pattern_black_left;
		}
		return shift_Pattern;
	}
	/**
	* @fn RandInt
	* @return Integer random number in [0, n)
	*/
	inline size_t RandInt(const size_t n) {
		return std::uniform_int_distribution<size_t>{0, n - 1}(mt);
	}
	/**
	* @fn ToStone
	* @brief Convert char to Stone
	* @param str /[BWN]/
	* @return 0-2[Stone::Black, Stone::White, Stone::None]
	* ~japanese	@detail strがkStoneStringのどれにも当てはまらない場合は例外を投げる
	* ~english	@detail If you can't find str in kStoneString, this throw exception.
	*/
	inline Stone ToStone(const char str) {
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
	* ~japanese	@brief 長連を起こす石の位置を算出する(黒石用)
	* ~english	@brief Make position mask of Cho-ren for Stone::Black
	*/
	inline BitBoard CalcChorenMaskB(const ShiftPattern &pattern) noexcept {
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
			//! BBBBBY
			choren_mask |= (BL0 & BL1 & BL2 & BL3 & BL4);
			//! BBBBYB
			choren_mask |= (BL0 & BL1 & BL2 & BL3 /**/& BR0);
			//! BBBYBB
			choren_mask |= (BL0 & BL1 & BL2 /**/& BR0 & BR1);
			//! BBYBBB
			choren_mask |= (BL0 & BL1 /**/& BR0 & BR1 & BR2);
			//! BYBBBB
			choren_mask |= (BL0 /**/& BR0 & BR1 & BR2 & BR3);
			//! YBBBBB
			choren_mask |= (BR0 & BR1 & BR2 & BR3 & BR4);
		}
		return choren_mask;
	}
	/**
	* ~japanese	@brief 一直線上の四々を起こす石の位置を算出する(黒石用)
	* ~english	@brief Make position mask of Shi-Shi on 1Line for Stone::Black
	*/
	inline BitBoard CalcLineShiShiMaskB(const ShiftPattern &pattern)  noexcept {
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
			//! XBOBBYOBX
			shishi1_mask |= (BL0 & BL1 & NL2 & BL3 & bL4 /**/& NR0 & BR1 & bR2);
			//! XBOBYBOBX
			shishi1_mask |= (BL0 & NL1 & BL2 & bL3 /**/& BR0 & NR1 & BR2 & bR3);
			//! XBOYBBOBX
			shishi1_mask |= (NL0 & BL1 & bL2 /**/& BR0 & BR1 & NR2 & BR3 & bR4);
			//! [XBBOYYOBBX]
			//! XBBOBYOBBX
			shishi1_mask |= (BL0 & NL1 & BL2 & BL3 & bL4 /**/& NR0 & BR1 & BR2 & bR3);
			//! XBBOYBOBBX
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
	inline RenjuPattern CalcShirenMaskB(const ShiftPattern &pattern)  noexcept {
		RenjuPattern shiren_mask;
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
			auto &ML1 = kBitMaskArray[dir][Side::Left][0], &ML2 = kBitMaskArray[dir][Side::Left][1];
			auto &ML3 = kBitMaskArray[dir][Side::Left][2], &ML4 = kBitMaskArray[dir][Side::Left][3];
			auto &MR1 = kBitMaskArray[dir][Side::Right][0], &MR2 = kBitMaskArray[dir][Side::Right][1];
			auto &MR3 = kBitMaskArray[dir][Side::Right][2], &MR4 = kBitMaskArray[dir][Side::Right][3];

			//! [XOBBBBOX]
			//! XOYBBBOX
			shiren_mask[dir] |= (NL0 & bL1 & ML1/**/& BR0 & BR1 & BR2 & NR3 & bR4 & MR4);
			//! XOBYBBOX
			shiren_mask[dir] |= (BL0 & NL1 & bL2 & ML2/**/& BR0 & BR1 & NR2 & bR3 & MR3);
			//! XOBBYBOX
			shiren_mask[dir] |= (BR0 & NR1 & bR2 & MR2/**/& BL0 & BL1 & NL2 & bL3 & ML3);
			//! XOBBBYOX
			shiren_mask[dir] |= (NR0 & bR1 & MR1/**/& BL0 & BL1 & BL2 & NL3 & bL4 & ML4);
		}
		return shiren_mask;
	}
	/**
	* ~japanese	@brief 活四を作る石の位置を算出する(黒石用)
	* ~english	@brief Make position mask of Katsu-Shi for Stone::Black
	*/
	inline RenjuPattern CalcKatsuShiMaskB(const ShiftPattern &pattern)  noexcept {
		RenjuPattern katsushi_mask;
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
			auto &ML1 = kBitMaskArray[dir][Side::Left][0], &ML2 = kBitMaskArray[dir][Side::Left][1];
			auto &ML3 = kBitMaskArray[dir][Side::Left][2], &ML4 = kBitMaskArray[dir][Side::Left][3];
			auto &MR1 = kBitMaskArray[dir][Side::Right][0], &MR2 = kBitMaskArray[dir][Side::Right][1];
			auto &MR3 = kBitMaskArray[dir][Side::Right][2], &MR4 = kBitMaskArray[dir][Side::Right][3];

			//! [[X{B4O1}X]]
			//! [XBBBBOX]
			katsushi_mask[dir] |= bL0 /**/& BR0 & BR1 & BR2 & NR3 & bR4 & MR4;
			katsushi_mask[dir] |= BL0 & bL1 & ML1 /**/& BR0 & BR1 & NR2 & bR3 & MR3;
			katsushi_mask[dir] |= BL0 & BL1 & bL2 & ML2 /**/& BR0 & NR1 & bR2 & MR2;
			katsushi_mask[dir] |= BL0 & BL1 & BL2 & bL3 & ML3 /**/& NR0 & bR1 & MR1;

			//! [XBBBOBX]
			katsushi_mask[dir] |= bL0 /**/& BR0 & BR1 & NR2 & BR3 & bR4 & MR4;
			katsushi_mask[dir] |= BL0 & bL1 & ML1 /**/& BR0 & NR1 & BR2 & bR3 & MR3;
			katsushi_mask[dir] |= BL0 & BL1 & bL2 & ML2 /**/& NR0 & BR1 & bR2 & MR2;
			katsushi_mask[dir] |= NL0 & BL1 & BL2 & BL3 & bL4 & ML4 /**/& bR0;

			//! [XBBOBBX]
			katsushi_mask[dir] |= bL0 /**/& BR0 & NR1 & BR2 & BR3 & bR4 & MR4;
			katsushi_mask[dir] |= BL0 & bL1 & ML1 /**/& NR0 & BR1 & BR2 & bR3 & MR3;
			katsushi_mask[dir] |= NL0 & BL1 & BL2 & bL3 & ML3 /**/& BR0 & bR1 & MR1;
			katsushi_mask[dir] |= BL0 & NL1 & BL2 & BL3 & bL4 & ML4 /**/& bR0;

			//! [XBOBBBX]
			katsushi_mask[dir] |= bL0 /**/& NR0 & BR1 & BR2 & BR3 & bR4 & MR4;
			katsushi_mask[dir] |= NL0 & BL1 & bL2 & ML2 /**/& BR0 & BR1 & bR2 & MR2;
			katsushi_mask[dir] |= BL0 & NL1 & BL2 & bL3 & ML3 /**/& BR0 & bR1 & MR1;
			katsushi_mask[dir] |= BL0 & BL1 & NL2 & BL3 & bL4 & ML4 /**/& bR0;

			//! [XOBBBBX]
			katsushi_mask[dir] |= NL0 & bL1 & ML1 /**/& BR0 & BR1 & BR2 & bR3 & MR3;
			katsushi_mask[dir] |= BL0 & NL1 & bL2 & ML2 /**/& BR0 & BR1 & bR2 & MR2;
			katsushi_mask[dir] |= BL0 & BL1 & NL2 & bL3 & ML3 /**/& BR0 & bR1 & MR1;
			katsushi_mask[dir] |= BL0 & BL1 & BL2 & NL3 & bL4 & ML4 /**/& bR0;
		}
		return katsushi_mask;
	}
	/**
	* ~japanese	@brief 活三を作る石の位置を算出する(黒石用)
	* ~english	@brief Make position mask of Katsu-San for Stone::Black
	*/
	RenjuPattern CalcKatsuSanMaskB(const ShiftPattern &pattern, const BitBoard &unorder_mask) noexcept {
		RenjuPattern katsusan_mask;
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
			auto &ML1 = kBitMaskArray[dir][Side::Left][0], &ML2 = kBitMaskArray[dir][Side::Left][1];
			auto &ML3 = kBitMaskArray[dir][Side::Left][2], &ML4 = kBitMaskArray[dir][Side::Left][3];
			auto &MR1 = kBitMaskArray[dir][Side::Right][0], &MR2 = kBitMaskArray[dir][Side::Right][1];
			auto &MR3 = kBitMaskArray[dir][Side::Right][2], &MR4 = kBitMaskArray[dir][Side::Right][3];

			//! [XO{B3O1}OX]
			const size_t kKatsuSanPositions = 12;
			array<BitBoard, kKatsuSanPositions> katsusan_position;
			static size_t offset_r[] = {3,2,1,2,1,0,1,0,0,0,0,0};
			static size_t offset_l[] = {0,0,0,0,0,1,0,1,2,1,2,3};
			auto filter = !unorder_mask;
			//! XOBBBOOX
			katsusan_position[0] = NL0 & bL1 & ML1 /**/& BR0 & BR1 & NR2 & NR3 & bR4 & MR4 & filter;
			katsusan_position[1] = BL0 & NL1 & bL2 & ML2 /**/& BR0 & NR1 & NR2 & bR3 & MR3 & filter;
			katsusan_position[2] = BL0 & BL1 & NL2 & bL3 & ML3 /**/& NR0 & NR1 & bR2 & MR2 & filter;
			//! XOBBOBOX
			katsusan_position[3] = NL0 & bL1 & ML1 /**/& BR0 & NR1 & BR2 & NR3 & bR4 & MR4 & filter;
			katsusan_position[4] = BL0 & NL1 & bL2 & ML2 /**/& NR0 & BR1 & NR2 & bR3 & MR3 & filter;
			katsusan_position[5] = NL0 & BL1 & BL2 & NL3 & bL4 & ML4 /**/& NR0 & bR1 & MR1 & filter;
			//! XOBOBBOX
			katsusan_position[6] = NL0 & bL1 & ML1 /**/& NR0 & BR1 & BR2 & NR3 & bR4 & MR4 & filter;
			katsusan_position[7] = NL0 & BL1 & NL2 & bL3 & ML3 /**/& BR0 & NR1 & bR2 & MR2 & filter;
			katsusan_position[8] = BL0 & NL1 & BL2 & NL3 & bL4 & ML4 /**/& NR0 & bR1 & MR1 & filter;
			//! XOOBBBOX
			katsusan_position[9] = NL0 & NL1 & bL2 & ML2 /**/& BR0 & BR1 & NR2 & bR3 & MR3 & filter;
			katsusan_position[10] = BL0 & NL1 & NL2 & bL3 & ML3 /**/& BR0 & NR1 & bR2 & MR2 & filter;
			katsusan_position[11] = BL0 & BL1 & NL2 & NL3 & bL4 & ML4 /**/& NR0 & bR1 & MR1 & filter;

			REP(index, kKatsuSanPositions) {
				katsusan_mask[dir] |= katsusan_position[index];
			}
		}
		return katsusan_mask;
	}
	/**
	* ~japanese	@brief RenjuPatternに対し除外マスクを適用する
	* ~english	@brief apply exclude-filter to RenjuPattern
	*/
	void SetPatternMask(RenjuPattern &pattern, const BitBoard &unorder_mask) noexcept {
		for (auto &it : pattern) {
			it = it & (!unorder_mask);
		}
	}
	/**
	* ~japanese	@brief 禁手で打てない位置のビットを立てたマスクを作成する
	* ~english	@brief Calc invaild position's mask
	*/
	BitBoard CalcInValidMask() noexcept{
		//! shift_pattern[Black, White][Row, Column, DiagR, DiagL][Left, Right][Shifts]
		auto shift_pattern = GetShiftPattern();
		return CalcInValidMask(shift_pattern);
	}
	BitBoard CalcInValidMask(const ShiftPattern &shift_pattern) noexcept{
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
		BitBoard invalid_mask = (black_board_ | white_board_), unorder_mask = (black_board_ | white_board_);
		//! Cho-ren check
		BitBoard choren_mask = CalcChorenMaskB(shift_pattern);
		unorder_mask |= choren_mask;
		//! Shi-Shi on 1 line check
		BitBoard shishi1_mask = CalcLineShiShiMaskB(shift_pattern);
		unorder_mask |= shishi1_mask;
		//! Shi-ren check
		auto shiren_mask = CalcShirenMaskB(shift_pattern);
		SetPatternMask(shiren_mask, unorder_mask);
		unorder_mask |= (shiren_mask[0] | shiren_mask[1] | shiren_mask[2] | shiren_mask[3]);
		//! Katsu-Shi check
		auto katsushi_mask = CalcKatsuShiMaskB(shift_pattern);
		SetPatternMask(katsushi_mask, unorder_mask);
		unorder_mask |= (katsushi_mask[0] | katsushi_mask[1] | katsushi_mask[2] | katsushi_mask[3]);
		//! Calc Shi-Shi point
		BitBoard s1 = shiren_mask[0] | katsushi_mask[0], s2 = shiren_mask[1] | katsushi_mask[1],
			s3 = shiren_mask[2] | katsushi_mask[2], s4 = shiren_mask[3] | katsushi_mask[3];
		BitBoard shishi_mask = (s1 & s2) | (s1 & s3) | (s1 & s4) | (s2 & s3) | (s2 & s4) | (s3 & s4);
		//! Katsu-San check
		auto katsusan_mask = CalcKatsuSanMaskB(shift_pattern, unorder_mask);
		//! Calc San-San point
		BitBoard sansan_mask = (katsusan_mask[0] & katsusan_mask[1]) | (katsusan_mask[0] & katsusan_mask[2]) | (katsusan_mask[0] & katsusan_mask[3])
			| (katsusan_mask[1] & katsusan_mask[2]) | (katsusan_mask[1] & katsusan_mask[3]) | (katsusan_mask[2] & katsusan_mask[3]);

		//BitBoard(shiren_mask[0] | shiren_mask[1] | shiren_mask[2] | shiren_mask[3]).PutBoard();
		//BitBoard(katsushi_mask[0] | katsushi_mask[1] | katsushi_mask[2] | katsushi_mask[3]).PutBoard();
		//BitBoard(katsusan_mask[0] | katsusan_mask[1] | katsusan_mask[2] | katsusan_mask[3]).PutBoard();

		invalid_mask |= choren_mask;
		invalid_mask |= shishi1_mask;
		invalid_mask |= shishi_mask;
		invalid_mask |= sansan_mask;
		//invalid_mask.PutBoard();
		return invalid_mask;	//! dummy
	}
	/**
	* @fn FindRandomMove
	* @return Next Move(Random)
	*/
	optional<size_t> FindRandomMove() {
		vector<size_t> list;
		auto invalid_mask = (turn_ == Stone::Black ? CalcInValidMask() : black_board_ | white_board_);
		//BitBoard(invalid_mask).PutBoard();
		REP(position, kAllBoardSize) {
			//! You can only move at Stone::None in Board
			if (!IsZero(kPositionArray[position] & invalid_mask)) continue;
			list.push_back(position);
		}
		if(list.empty()) return optional<size_t>(-1); 
		return optional<size_t>(list[RandInt(list.size())]);
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
	void Test() {
		const size_t count = 1000000;
		auto start = std::chrono::system_clock::now();
		REP(i, count) {
			FindRandomMove();
		}
		auto end = std::chrono::system_clock::now();
		auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		cout << msec << "[ms]" << endl;
	}
};
