/**
* @file ShiftPattern.hpp
* ~japanese	@brief 盤面全体をシフトしたデータを記憶する
* ~english	@brief Definition of shift-board for Renju game
* @author YSR
* @date 2016/05/03
*/
#pragma once
#include<array>
#include"constant.hpp"
#include"BitBoard.hpp"
#include"types.hpp"

using std::array;
//! Definition of Renju pattern
using RenjuPattern = array<BitBoard, Direction::Directions>;

class ShiftPattern {
	array<array<array<array<BitBoard, kMaxShifts>, Side::Sides>, Stone::Stones>, Direction::Directions> data_;
public:
	void SetData(const Direction dir, const Stone stone, const Side side, const size_t shift, const BitBoard &board) noexcept{
		data_[dir][stone][side][shift] = board;
	}
	BitBoard GetData(const Direction dir, const Stone stone, const Side side, const size_t shift) const noexcept {
		return data_[dir][stone][side][shift];
	}
	/**
	* ~japanese	@brief 長連を起こす石の位置を算出する(黒石用)
	* ~english	@brief Make position mask of Cho-ren for Stone::Black
	*/
	BitBoard CalcChorenMaskB() const noexcept {
		BitBoard choren_mask;
		REP(dir_, Direction::Directions) {
			auto dir = static_cast<Direction>(dir_);
			using namespace omission;
			auto &BL0 = GetData(dir, B, L, 0), &NL0 = GetData(dir, N, L, 0);
			auto &BL1 = GetData(dir, B, L, 1), &NL1 = GetData(dir, N, L, 1);
			auto &BL2 = GetData(dir, B, L, 2), &NL2 = GetData(dir, N, L, 2);
			auto &BL3 = GetData(dir, B, L, 3), &NL3 = GetData(dir, N, L, 3);
			auto &BL4 = GetData(dir, B, L, 4), &NL4 = GetData(dir, N, L, 4);
			auto &BR0 = GetData(dir, B, R, 0), &NR0 = GetData(dir, N, R, 0);
			auto &BR1 = GetData(dir, B, R, 1), &NR1 = GetData(dir, N, R, 1);
			auto &BR2 = GetData(dir, B, R, 2), &NR2 = GetData(dir, N, R, 2);
			auto &BR3 = GetData(dir, B, R, 3), &NR3 = GetData(dir, N, R, 3);
			auto &BR4 = GetData(dir, B, R, 4), &NR4 = GetData(dir, N, R, 4);

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
	BitBoard CalcLineShiShiMaskB() const noexcept {
		BitBoard shishi1_mask;
		REP(dir_, Direction::Directions) {
			auto dir = static_cast<Direction>(dir_);
			using namespace omission;
			auto &BL0 = GetData(dir, B, L, 0), &NL0 = GetData(dir, N, L, 0), &bL0 = GetData(dir, b, L, 0);
			auto &BL1 = GetData(dir, B, L, 1), &NL1 = GetData(dir, N, L, 1), &bL1 = GetData(dir, b, L, 1);
			auto &BL2 = GetData(dir, B, L, 2), &NL2 = GetData(dir, N, L, 2), &bL2 = GetData(dir, b, L, 2);
			auto &BL3 = GetData(dir, B, L, 3), &NL3 = GetData(dir, N, L, 3), &bL3 = GetData(dir, b, L, 3);
			auto &BL4 = GetData(dir, B, L, 4), &NL4 = GetData(dir, N, L, 4), &bL4 = GetData(dir, b, L, 4);
			auto &BR0 = GetData(dir, B, R, 0), &NR0 = GetData(dir, N, R, 0), &bR0 = GetData(dir, b, R, 0);
			auto &BR1 = GetData(dir, B, R, 1), &NR1 = GetData(dir, N, R, 1), &bR1 = GetData(dir, b, R, 1);
			auto &BR2 = GetData(dir, B, R, 2), &NR2 = GetData(dir, N, R, 2), &bR2 = GetData(dir, b, R, 2);
			auto &BR3 = GetData(dir, B, R, 3), &NR3 = GetData(dir, N, R, 3), &bR3 = GetData(dir, b, R, 3);
			auto &BR4 = GetData(dir, B, R, 4), &NR4 = GetData(dir, N, R, 4), &bR4 = GetData(dir, b, R, 4);

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
	RenjuPattern CalcShirenMaskB() const noexcept {
		RenjuPattern shiren_mask;
		REP(dir_, Direction::Directions) {
			auto dir = static_cast<Direction>(dir_);
			using namespace omission;
			auto &BL0 = GetData(dir, B, L, 0), &NL0 = GetData(dir, N, L, 0), &bL0 = GetData(dir, b, L, 0);
			auto &BL1 = GetData(dir, B, L, 1), &NL1 = GetData(dir, N, L, 1), &bL1 = GetData(dir, b, L, 1);
			auto &BL2 = GetData(dir, B, L, 2), &NL2 = GetData(dir, N, L, 2), &bL2 = GetData(dir, b, L, 2);
			auto &BL3 = GetData(dir, B, L, 3), &NL3 = GetData(dir, N, L, 3), &bL3 = GetData(dir, b, L, 3);
			auto &BL4 = GetData(dir, B, L, 4), &NL4 = GetData(dir, N, L, 4), &bL4 = GetData(dir, b, L, 4);
			auto &BR0 = GetData(dir, B, R, 0), &NR0 = GetData(dir, N, R, 0), &bR0 = GetData(dir, b, R, 0);
			auto &BR1 = GetData(dir, B, R, 1), &NR1 = GetData(dir, N, R, 1), &bR1 = GetData(dir, b, R, 1);
			auto &BR2 = GetData(dir, B, R, 2), &NR2 = GetData(dir, N, R, 2), &bR2 = GetData(dir, b, R, 2);
			auto &BR3 = GetData(dir, B, R, 3), &NR3 = GetData(dir, N, R, 3), &bR3 = GetData(dir, b, R, 3);
			auto &BR4 = GetData(dir, B, R, 4), &NR4 = GetData(dir, N, R, 4), &bR4 = GetData(dir, b, R, 4);
			auto &ML1 = kBitMaskArray[dir][L][0], &ML2 = kBitMaskArray[dir][L][1];
			auto &ML3 = kBitMaskArray[dir][L][2], &ML4 = kBitMaskArray[dir][L][3];
			auto &MR1 = kBitMaskArray[dir][R][0], &MR2 = kBitMaskArray[dir][R][1];
			auto &MR3 = kBitMaskArray[dir][R][2], &MR4 = kBitMaskArray[dir][R][3];

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
	RenjuPattern CalcKatsuShiMaskB() const noexcept {
		RenjuPattern katsushi_mask;
		REP(dir_, Direction::Directions) {
			auto dir = static_cast<Direction>(dir_);
			using namespace omission;
			auto &BL0 = GetData(dir, B, L, 0), &NL0 = GetData(dir, N, L, 0), &bL0 = GetData(dir, b, L, 0);
			auto &BL1 = GetData(dir, B, L, 1), &NL1 = GetData(dir, N, L, 1), &bL1 = GetData(dir, b, L, 1);
			auto &BL2 = GetData(dir, B, L, 2), &NL2 = GetData(dir, N, L, 2), &bL2 = GetData(dir, b, L, 2);
			auto &BL3 = GetData(dir, B, L, 3), &NL3 = GetData(dir, N, L, 3), &bL3 = GetData(dir, b, L, 3);
			auto &BL4 = GetData(dir, B, L, 4), &NL4 = GetData(dir, N, L, 4), &bL4 = GetData(dir, b, L, 4);
			auto &BR0 = GetData(dir, B, R, 0), &NR0 = GetData(dir, N, R, 0), &bR0 = GetData(dir, b, R, 0);
			auto &BR1 = GetData(dir, B, R, 1), &NR1 = GetData(dir, N, R, 1), &bR1 = GetData(dir, b, R, 1);
			auto &BR2 = GetData(dir, B, R, 2), &NR2 = GetData(dir, N, R, 2), &bR2 = GetData(dir, b, R, 2);
			auto &BR3 = GetData(dir, B, R, 3), &NR3 = GetData(dir, N, R, 3), &bR3 = GetData(dir, b, R, 3);
			auto &BR4 = GetData(dir, B, R, 4), &NR4 = GetData(dir, N, R, 4), &bR4 = GetData(dir, b, R, 4);
			auto &ML1 = kBitMaskArray[dir][L][0], &ML2 = kBitMaskArray[dir][L][1];
			auto &ML3 = kBitMaskArray[dir][L][2], &ML4 = kBitMaskArray[dir][L][3];
			auto &MR1 = kBitMaskArray[dir][R][0], &MR2 = kBitMaskArray[dir][R][1];
			auto &MR3 = kBitMaskArray[dir][R][2], &MR4 = kBitMaskArray[dir][R][3];

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
	RenjuPattern CalcKatsuSanMaskB(const BitBoard &unorder_mask) const noexcept {
		RenjuPattern katsusan_mask;
		REP(dir_, Direction::Directions) {
			auto dir = static_cast<Direction>(dir_);
			using namespace omission;
			auto &BL0 = GetData(dir, B, L, 0), &NL0 = GetData(dir, N, L, 0), &bL0 = GetData(dir, b, L, 0);
			auto &BL1 = GetData(dir, B, L, 1), &NL1 = GetData(dir, N, L, 1), &bL1 = GetData(dir, b, L, 1);
			auto &BL2 = GetData(dir, B, L, 2), &NL2 = GetData(dir, N, L, 2), &bL2 = GetData(dir, b, L, 2);
			auto &BL3 = GetData(dir, B, L, 3), &NL3 = GetData(dir, N, L, 3), &bL3 = GetData(dir, b, L, 3);
			auto &BL4 = GetData(dir, B, L, 4), &NL4 = GetData(dir, N, L, 4), &bL4 = GetData(dir, b, L, 4);
			auto &BR0 = GetData(dir, B, R, 0), &NR0 = GetData(dir, N, R, 0), &bR0 = GetData(dir, b, R, 0);
			auto &BR1 = GetData(dir, B, R, 1), &NR1 = GetData(dir, N, R, 1), &bR1 = GetData(dir, b, R, 1);
			auto &BR2 = GetData(dir, B, R, 2), &NR2 = GetData(dir, N, R, 2), &bR2 = GetData(dir, b, R, 2);
			auto &BR3 = GetData(dir, B, R, 3), &NR3 = GetData(dir, N, R, 3), &bR3 = GetData(dir, b, R, 3);
			auto &BR4 = GetData(dir, B, R, 4), &NR4 = GetData(dir, N, R, 4), &bR4 = GetData(dir, b, R, 4);
			auto &ML1 = kBitMaskArray[dir][L][0], &ML2 = kBitMaskArray[dir][L][1];
			auto &ML3 = kBitMaskArray[dir][L][2], &ML4 = kBitMaskArray[dir][L][3];
			auto &MR1 = kBitMaskArray[dir][R][0], &MR2 = kBitMaskArray[dir][R][1];
			auto &MR3 = kBitMaskArray[dir][R][2], &MR4 = kBitMaskArray[dir][R][3];

			//! [XO{B3O1}OX]
			const size_t kKatsuSanPositions = 12;
			array<BitBoard, kKatsuSanPositions> katsusan_position;
			static size_t offset_r[] = { 3,2,1,2,1,0,1,0,0,0,0,0 };
			static size_t offset_l[] = { 0,0,0,0,0,1,0,1,2,1,2,3 };
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
};
