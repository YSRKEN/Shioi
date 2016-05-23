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
			using namespace omission;
			auto& p = this->data_[dir_];

			//! [BBBBBB]
			//! BBBBBY
			choren_mask |= (p[B][L][0] & p[B][L][1] & p[B][L][2] & p[B][L][3] & p[B][L][4]);
			//! BBBBYB
			choren_mask |= (p[B][L][0] & p[B][L][1] & p[B][L][2] & p[B][L][3] /**/& p[B][R][0]);
			//! BBBYBB
			choren_mask |= (p[B][L][0] & p[B][L][1] & p[B][L][2] /**/& p[B][R][0] & p[B][R][1]);
			//! BBYBBB
			choren_mask |= (p[B][L][0] & p[B][L][1] /**/& p[B][R][0] & p[B][R][1] & p[B][R][2]);
			//! BYBBBB
			choren_mask |= (p[B][L][0] /**/& p[B][R][0] & p[B][R][1] & p[B][R][2] & p[B][R][3]);
			//! YBBBBB
			choren_mask |= (p[B][R][0] & p[B][R][1] & p[B][R][2] & p[B][R][3] & p[B][R][4]);
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
			auto& p = this->data_[dir_];
			using namespace omission;

			//! [XBOYYYOBX]
			//! XBOBBYOBX
			shishi1_mask |= (p[B][L][0] & p[B][L][1] & p[N][L][2] & p[B][L][3] & p[b][L][4] /**/& p[N][R][0] & p[B][R][1] & p[b][R][2]);
			//! XBOBYBOBX
			shishi1_mask |= (p[B][L][0] & p[N][L][1] & p[B][L][2] & p[b][L][3] /**/& p[B][R][0] & p[N][R][1] & p[B][R][2] & p[b][R][3]);
			//! XBOYBBOBX
			shishi1_mask |= (p[N][L][0] & p[B][L][1] & p[b][L][2] /**/& p[B][R][0] & p[B][R][1] & p[N][R][2] & p[B][R][3] & p[b][R][4]);
			//! [XBBOYYOBBX]
			//! XBBOBYOBBX
			shishi1_mask |= (p[B][L][0] & p[N][L][1] & p[B][L][2] & p[B][L][3] & p[b][L][4] /**/& p[N][R][0] & p[B][R][1] & p[B][R][2] & p[b][R][3]);
			//! XBBOYBOBBX
			shishi1_mask |= (p[N][L][0] & p[B][L][1] & p[B][L][2] & p[b][L][3] /**/& p[B][R][0] & p[N][R][1] & p[B][R][2] & p[B][R][3] & p[b][R][4]);
			//! [XBBBOYOBBBX]
			shishi1_mask |= (p[N][R][0] & p[B][R][1] & p[B][R][2] & p[B][R][3] & p[b][R][4] /**/& p[N][L][0] & p[B][L][1] & p[B][L][2] & p[B][L][3] & p[b][L][4]);
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
			auto& p = this->data_[dir_];
			using namespace omission;
			auto& m = kBitMaskArray[dir];

			//! [XOBBBBOX]
			//! XOYBBBOX
			shiren_mask[dir] |= (p[N][L][0] & p[b][L][1] & m[L][1]/**/& p[B][R][0] & p[B][R][1] & p[B][R][2] & p[N][R][3] & p[b][R][4] & m[R][4]);
			//! XOBYBBOX
			shiren_mask[dir] |= (p[B][L][0] & p[N][L][1] & p[b][L][2] & m[L][2]/**/& p[B][R][0] & p[B][R][1] & p[N][R][2] & p[b][R][3] & m[R][3]);
			//! XOBBYBOX
			shiren_mask[dir] |= (p[B][R][0] & p[N][R][1] & p[b][R][2] & m[R][2]/**/& p[B][L][0] & p[B][L][1] & p[N][L][2] & p[b][L][3] & m[L][3]);
			//! XOBBBYOX
			shiren_mask[dir] |= (p[N][R][0] & p[b][R][1] & m[R][1]/**/& p[B][L][0] & p[B][L][1] & p[B][L][2] & p[N][L][3] & p[b][L][4] & m[L][4]);
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
			auto& p = this->data_[dir_];
			using namespace omission;
			auto& m = kBitMaskArray[dir];

			//! [[X{B4O1}X]]
			//! [XBBBBOX]
			katsushi_mask[dir] |= p[b][L][0] /**/& p[B][R][0] & p[B][R][1] & p[B][R][2] & p[N][R][3] & p[b][R][4] & m[R][4];
			katsushi_mask[dir] |= p[B][L][0] & p[b][L][1] & m[L][1] /**/& p[B][R][0] & p[B][R][1] & p[N][R][2] & p[b][R][3] & m[R][3];
			katsushi_mask[dir] |= p[B][L][0] & p[B][L][1] & p[b][L][2] & m[L][2] /**/& p[B][R][0] & p[N][R][1] & p[b][R][2] & m[R][2];
			katsushi_mask[dir] |= p[B][L][0] & p[B][L][1] & p[B][L][2] & p[b][L][3] & m[L][3] /**/& p[N][R][0] & p[b][R][1] & m[R][1];

			//! [XBBBOBX]
			katsushi_mask[dir] |= p[b][L][0] /**/& p[B][R][0] & p[B][R][1] & p[N][R][2] & p[B][R][3] & p[b][R][4] & m[R][4];
			katsushi_mask[dir] |= p[B][L][0] & p[b][L][1] & m[L][1] /**/& p[B][R][0] & p[N][R][1] & p[B][R][2] & p[b][R][3] & m[R][3];
			katsushi_mask[dir] |= p[B][L][0] & p[B][L][1] & p[b][L][2] & m[L][2] /**/& p[N][R][0] & p[B][R][1] & p[b][R][2] & m[R][2];
			katsushi_mask[dir] |= p[N][L][0] & p[B][L][1] & p[B][L][2] & p[B][L][3] & p[b][L][4] & m[L][4] /**/& p[b][R][0];

			//! [XBBOBBX]
			katsushi_mask[dir] |= p[b][L][0] /**/& p[B][R][0] & p[N][R][1] & p[B][R][2] & p[B][R][3] & p[b][R][4] & m[R][4];
			katsushi_mask[dir] |= p[B][L][0] & p[b][L][1] & m[L][1] /**/& p[N][R][0] & p[B][R][1] & p[B][R][2] & p[b][R][3] & m[R][3];
			katsushi_mask[dir] |= p[N][L][0] & p[B][L][1] & p[B][L][2] & p[b][L][3] & m[L][3] /**/& p[B][R][0] & p[b][R][1] & m[R][1];
			katsushi_mask[dir] |= p[B][L][0] & p[N][L][1] & p[B][L][2] & p[B][L][3] & p[b][L][4] & m[L][4] /**/& p[b][R][0];

			//! [XBOBBBX]
			katsushi_mask[dir] |= p[b][L][0] /**/& p[N][R][0] & p[B][R][1] & p[B][R][2] & p[B][R][3] & p[b][R][4] & m[R][4];
			katsushi_mask[dir] |= p[N][L][0] & p[B][L][1] & p[b][L][2] & m[L][2] /**/& p[B][R][0] & p[B][R][1] & p[b][R][2] & m[R][2];
			katsushi_mask[dir] |= p[B][L][0] & p[N][L][1] & p[B][L][2] & p[b][L][3] & m[L][3] /**/& p[B][R][0] & p[b][R][1] & m[R][1];
			katsushi_mask[dir] |= p[B][L][0] & p[B][L][1] & p[N][L][2] & p[B][L][3] & p[b][L][4] & m[L][4] /**/& p[b][R][0];

			//! [XOBBBBX]
			katsushi_mask[dir] |= p[N][L][0] & p[b][L][1] & m[L][1] /**/& p[B][R][0] & p[B][R][1] & p[B][R][2] & p[b][R][3] & m[R][3];
			katsushi_mask[dir] |= p[B][L][0] & p[N][L][1] & p[b][L][2] & m[L][2] /**/& p[B][R][0] & p[B][R][1] & p[b][R][2] & m[R][2];
			katsushi_mask[dir] |= p[B][L][0] & p[B][L][1] & p[N][L][2] & p[b][L][3] & m[L][3] /**/& p[B][R][0] & p[b][R][1] & m[R][1];
			katsushi_mask[dir] |= p[B][L][0] & p[B][L][1] & p[B][L][2] & p[N][L][3] & p[b][L][4] & m[L][4] /**/& p[b][R][0];
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
			auto& p = this->data_[dir_];
			using namespace omission;
			auto& m = kBitMaskArray[dir];

			//! [XO{B3O1}OX]
			const size_t kKatsuSanPositions = 12;
			array<BitBoard, kKatsuSanPositions> katsusan_position;
			//static size_t offset_r[] = { 3,2,1,2,1,0,1,0,0,0,0,0 };
			//static size_t offset_l[] = { 0,0,0,0,0,1,0,1,2,1,2,3 };
			auto filter = !unorder_mask;
			//! XOBBBOOX
			katsusan_position[0] = p[N][L][0] & p[b][L][1] & m[L][1] /**/& p[B][R][0] & p[B][R][1] & p[N][R][2] & p[N][R][3] & p[b][R][4] & m[R][4] & filter;
			katsusan_position[1] = p[B][L][0] & p[N][L][1] & p[b][L][2] & m[L][2] /**/& p[B][R][0] & p[N][R][1] & p[N][R][2] & p[b][R][3] & m[R][3] & filter;
			katsusan_position[2] = p[B][L][0] & p[B][L][1] & p[N][L][2] & p[b][L][3] & m[L][3] /**/& p[N][R][0] & p[N][R][1] & p[b][R][2] & m[R][2] & filter;
			//! XOBBOBOX
			katsusan_position[3] = p[N][L][0] & p[b][L][1] & m[L][1] /**/& p[B][R][0] & p[N][R][1] & p[B][R][2] & p[N][R][3] & p[b][R][4] & m[R][4] & filter;
			katsusan_position[4] = p[B][L][0] & p[N][L][1] & p[b][L][2] & m[L][2] /**/& p[N][R][0] & p[B][R][1] & p[N][R][2] & p[b][R][3] & m[R][3] & filter;
			katsusan_position[5] = p[N][L][0] & p[B][L][1] & p[B][L][2] & p[N][L][3] & p[b][L][4] & m[L][4] /**/& p[N][R][0] & p[b][R][1] & m[R][1] & filter;
			//! XOBOBBOX
			katsusan_position[6] = p[N][L][0] & p[b][L][1] & m[L][1] /**/& p[N][R][0] & p[B][R][1] & p[B][R][2] & p[N][R][3] & p[b][R][4] & m[R][4] & filter;
			katsusan_position[7] = p[N][L][0] & p[B][L][1] & p[N][L][2] & p[b][L][3] & m[L][3] /**/& p[B][R][0] & p[N][R][1] & p[b][R][2] & m[R][2] & filter;
			katsusan_position[8] = p[B][L][0] & p[N][L][1] & p[B][L][2] & p[N][L][3] & p[b][L][4] & m[L][4] /**/& p[N][R][0] & p[b][R][1] & m[R][1] & filter;
			//! XOOBBBOX
			katsusan_position[9] = p[N][L][0] & p[N][L][1] & p[b][L][2] & m[L][2] /**/& p[B][R][0] & p[B][R][1] & p[N][R][2] & p[b][R][3] & m[R][3] & filter;
			katsusan_position[10] = p[B][L][0] & p[N][L][1] & p[N][L][2] & p[b][L][3] & m[L][3] /**/& p[B][R][0] & p[N][R][1] & p[b][R][2] & m[R][2] & filter;
			katsusan_position[11] = p[B][L][0] & p[B][L][1] & p[N][L][2] & p[N][L][3] & p[b][L][4] & m[L][4] /**/& p[N][R][0] & p[b][R][1] & m[R][1] & filter;

			REP(index, kKatsuSanPositions) {
				katsusan_mask[dir] |= katsusan_position[index];
			}
		}
		return katsusan_mask;
	}
	/**
	* ~japanese	@brief 五連を起こす石の位置を算出する(黒石用)
	* ~english	@brief Make position mask of Cho-ren for Stone::Black
	*/
	BitBoard CalcGorenMaskB() const noexcept {
		BitBoard goren_mask;
		REP(dir_, Direction::Directions) {
			auto dir = static_cast<Direction>(dir_);
			auto& p = this->data_[dir_];
			using namespace omission;
			auto& m = kBitMaskArray[dir];

			//! [XBBBBBX]
			goren_mask |= p[b][L][0] /**/& p[B][R][0] & p[B][R][1] & p[B][R][2] & p[B][R][3] & p[b][R][4] & m[R][4];
			goren_mask |= p[B][L][0] & p[b][L][1] & m[L][1] /**/& p[B][R][0] & p[B][R][1] & p[B][R][2] & p[b][R][3] & m[R][3];
			goren_mask |= p[B][L][0] & p[B][L][1] & p[b][L][2] & m[L][2] /**/& p[B][R][0] & p[B][R][1] & p[b][R][2] & m[R][2];
			goren_mask |= p[B][L][0] & p[B][L][1] & p[B][L][2] & p[b][L][3] & m[L][3] /**/& p[B][R][0] & p[b][R][1] & m[R][1];
			goren_mask |= p[B][L][0] & p[B][L][1] & p[B][L][2] & p[B][L][3] & p[b][L][4] & m[L][4] /**/& p[b][R][0];
		}
		return goren_mask;
	}
	/**
	* ~japanese	@brief 五連を起こす石の位置を算出する(白石用)
	* ~english	@brief Make position mask of Cho-ren for Stone::White
	*/
	BitBoard CalcGorenMaskW() const noexcept {
		BitBoard goren_mask;
		REP(dir_, Direction::Directions) {
			auto dir = static_cast<Direction>(dir_);
			auto& p = this->data_[dir_];
			using namespace omission;
			auto& m = kBitMaskArray[dir];

			//! [WWWWW]
			goren_mask |= p[W][R][0] & p[W][R][1] & p[W][R][2] & p[W][R][3] & m[R][3];
			goren_mask |= p[W][L][0] /**/& p[W][R][0] & p[W][R][1] & p[W][R][2] & m[R][2];
			goren_mask |= p[W][L][0] & p[W][L][1] & m[L][1] /**/& p[W][R][0] & p[W][R][1] & m[R][1];
			goren_mask |= p[W][L][0] & p[W][L][1] & p[W][L][2] & m[L][2] /**/& p[W][R][0];
			goren_mask |= p[W][L][0] & p[W][L][1] & p[W][L][2] & p[W][L][3] & m[L][3];
		}
		return goren_mask;
	}
};

/**
* ~japanese	@brief RenjuPatternに対し除外マスクを適用する
* ~english	@brief apply exclude-filter to RenjuPattern
*/
inline void SetPatternMask(RenjuPattern &pattern, const BitBoard &unorder_mask) {
	for (auto &it : pattern) {
		it = it & (!unorder_mask);
	}
}
