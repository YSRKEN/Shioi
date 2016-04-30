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
#include"../I401/constant_range_loop.hpp"

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
		if (black_board_.GetBit(position)) return Stone::Black;
		if (white_board_.GetBit(position)) return Stone::White;
		return Stone::None;
	}
	template<Stone color, enable_if_t<color == Stone::Black> = nullptr> BitBoard GetBitBoardByStoneColor() const noexcept { return black_board_; }
	template<Stone color, enable_if_t<color == Stone::White> = nullptr> BitBoard GetBitBoardByStoneColor() const noexcept { return white_board_; }
	template<Stone color, Side side> void GetShiftPattern_impl(array<array<array<BitBoard, kMaxShifts>, Side::Sides>, Stone::Stones>& s_p, Direction dir){
		auto pattern = GetBitBoardByStoneColor<color>();
		for (auto& shift : s_p[color][side])  { sift_assign<side>(pattern, dir); shift = pattern; }
	}
	template<Side side> void GetShiftPattern_impl(array<array<array<BitBoard, kMaxShifts>, Side::Sides>, Stone::Stones>& s_p, Stone color, Direction dir){
		auto pattern_black = black_board_; auto pattern_white = white_board_;
		for (auto& shift : s_p[color][side]) {
			pattern_black >>= dir; pattern_white >>= dir;
			shift = !(pattern_black | pattern_white);
		}
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
	* ~japanese	@brief 方向・先後・左右・シフト数に応じたシフトパターンを作成する
	* ~english	@brief Make shift pattern
	*/
	ShiftPattern GetShiftPattern() {
		ShiftPattern shift_Pattern;
		for(const auto dir : rep(Direction::Directions)) {
			auto& s_p = shift_Pattern[dir];
			//modify shift_Pattern
			GetShiftPattern_impl<Stone::Black, Side::Right>(s_p, dir);
			GetShiftPattern_impl<Stone::White, Side::Right>(s_p, dir);
			GetShiftPattern_impl<Side::Right>(s_p, Stone::None, dir);
			GetShiftPattern_impl<Stone::Black, Side::Left >(s_p, dir);
			GetShiftPattern_impl<Stone::White, Side::Left >(s_p, dir);
			GetShiftPattern_impl<Side::Left >(s_p, Stone::None, dir);
			
			s_p[Stone::NonBlack] = s_p[Stone::Black];//copy
			for(auto s : s_p[Stone::NonBlack]) for(auto& spss : s) spss = !spss;//否定
		}
		return shift_Pattern;
	}
	/**
	* ~japanese	@p[b][r][i]ef RenjuPatternに対し除外マスクを適用する
	* ~english	@brief apply exclude-filter to RenjuPattern
	*/
	void SetPatternMask(RenjuPattern &pattern, const BitBoard &unorder_mask) noexcept {
		for (auto &it : pattern) {
			it = it & (!unorder_mask);
		}
	}

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning( push )
//VSはどうもrepをコンテナとして使うRange-based forが実行されない可能性が無いことを見抜けないようで、警告を出す
#pragma warning( disable : 4701 )//初期化されていない可能性のあるローカル変数 xxxx が使用されます
#endif
	/**
	* ~japanese	@brief 長連を起こす石の位置を算出する(黒石用)
	* ~english	@brief Make position mask of Cho-ren for Stone::Black
	*/
	BitBoard CalcChorenMaskB(const ShiftPattern &pattern) const noexcept {
		BitBoard choren_mask;
		for(const auto dir : rep(Direction::Directions)) {
			auto& p = pattern[dir];
			using namespace short_constant;

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
	BitBoard CalcLineShiShiMaskB(const ShiftPattern &pattern) const noexcept {
		BitBoard shishi1_mask;
		for(const auto dir : rep(Direction::Directions)) {
			auto& p = pattern[dir];
			using namespace short_constant;

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
	RenjuPattern CalcShirenMaskB(const ShiftPattern &pattern) const noexcept {
		RenjuPattern shiren_mask;
		for (const auto dir : rep(Direction::Directions)) {
			auto& p = pattern[dir];
			auto& m = BitBoard::kBitMaskArray[dir];
			using namespace short_constant;

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
	RenjuPattern CalcKatsuShiMaskB(const ShiftPattern &pattern) const noexcept {
		RenjuPattern katsushi_mask;
		for (const auto dir : rep(Direction::Directions)) {
			auto& p = pattern[dir];
			auto& m = BitBoard::kBitMaskArray[dir];
			using namespace short_constant;

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
	RenjuPattern CalcKatsuSanMaskB(const ShiftPattern &pattern, const BitBoard &unorder_mask) noexcept {
		RenjuPattern katsusan_mask;
		for (const auto dir : rep(Direction::Directions)) {
			auto& p = pattern[dir];
			auto& m = BitBoard::kBitMaskArray[dir];
			using namespace short_constant;

			//! [XO{B3O1}OX]
			//! XOBBBOOX
			katsusan_mask[dir] |= p[N][L][0] & p[b][L][1] & m[L][1] /**/& p[B][R][0] & p[B][R][1] & p[N][R][2] & p[N][R][3] & p[b][R][4] & m[R][4];
			katsusan_mask[dir] |= p[B][L][0] & p[N][L][1] & p[b][L][2] & m[L][2] /**/& p[B][R][0] & p[N][R][1] & p[N][R][2] & p[b][R][3] & m[R][3];
			katsusan_mask[dir] |= p[B][L][0] & p[B][L][1] & p[N][L][2] & p[b][L][3] & m[L][3] /**/& p[N][R][0] & p[N][R][1] & p[b][R][2] & m[R][2];
			//! XOBBOBOX
			katsusan_mask[dir] |= p[N][L][0] & p[b][L][1] & m[L][1] /**/& p[B][R][0] & p[N][R][1] & p[B][R][2] & p[N][R][3] & p[b][R][4] & m[R][4];
			katsusan_mask[dir] |= p[B][L][0] & p[N][L][1] & p[b][L][2] & m[L][2] /**/& p[N][R][0] & p[B][R][1] & p[N][R][2] & p[b][R][3] & m[R][3];
			katsusan_mask[dir] |= p[N][L][0] & p[B][L][1] & p[B][L][2] & p[N][L][3] & p[b][L][4] & m[L][4] /**/& p[N][R][0] & p[b][R][1] & m[R][1];
			//! XOBOBBOX
			katsusan_mask[dir] |= p[N][L][0] & p[b][L][1] & m[L][1] /**/& p[N][R][0] & p[B][R][1] & p[B][R][2] & p[N][R][3] & p[b][R][4] & m[R][4];
			katsusan_mask[dir] |= p[N][L][0] & p[B][L][1] & p[N][L][2] & p[b][L][3] & m[L][3] /**/& p[B][R][0] & p[N][R][1] & p[b][R][2] & m[R][2];
			katsusan_mask[dir] |= p[B][L][0] & p[N][L][1] & p[B][L][2] & p[N][L][3] & p[b][L][4] & m[L][4] /**/& p[N][R][0] & p[b][R][1] & m[R][1];
			//! XOOBBBOX
			katsusan_mask[dir] |= p[N][L][0] & p[N][L][1] & p[b][L][2] & m[L][2] /**/& p[B][R][0] & p[B][R][1] & p[N][R][2] & p[b][R][3] & m[R][3];
			katsusan_mask[dir] |= p[B][L][0] & p[N][L][1] & p[N][L][2] & p[b][L][3] & m[L][3] /**/& p[B][R][0] & p[N][R][1] & p[b][R][2] & m[R][2];
			katsusan_mask[dir] |= p[B][L][0] & p[B][L][1] & p[N][L][2] & p[N][L][3] & p[b][L][4] & m[L][4] /**/& p[N][R][0] & p[b][R][1] & m[R][1];

			//! Ina-San-San Filter
			katsusan_mask[dir] = katsusan_mask[dir] & (!unorder_mask);
			if (0 != katsusan_mask[dir]) {

			}
		}
		return katsusan_mask;
	}
#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning( pop )//#pragma warning( disable : 4701 )//初期化されていない可能性のあるローカル変数 xxxx が使用されます
#endif
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
		SetPatternMask(katsusan_mask, unorder_mask);
		//! Calc San-San point
		BitBoard sansan_mask = (katsusan_mask[0] & katsusan_mask[1]) | (katsusan_mask[0] & katsusan_mask[2]) | (katsusan_mask[0] & katsusan_mask[3])
			| (katsusan_mask[1] & katsusan_mask[2]) | (katsusan_mask[1] & katsusan_mask[3]) | (katsusan_mask[2] & katsusan_mask[3]);

		BitBoard(shiren_mask[0] | shiren_mask[1] | shiren_mask[2] | shiren_mask[3]).PutBoard();
		BitBoard(katsushi_mask[0] | katsushi_mask[1] | katsushi_mask[2] | katsushi_mask[3]).PutBoard();
		BitBoard(katsusan_mask[0] | katsusan_mask[1] | katsusan_mask[2] | katsusan_mask[3]).PutBoard();

		invalid_mask |= choren_mask;
		invalid_mask |= shishi1_mask;
		invalid_mask |= shishi_mask;
		invalid_mask |= sansan_mask;
		invalid_mask.PutBoard();
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
		for(const auto position : rep(kAllBoardSize)) {
			//! You can only move at Stone::None in Board
			if (0 != (BitBoard::kPositionArray[position] & invalid_mask)) continue;
			list.push_back(position);
		}
		if(list.size() > 0) return optional<size_t>(list[RandInt(list.size())]);
		return{};
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
		for (const auto position : rep(kAllBoardSize)) {
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
		for(const auto y : rep(kBoardSize)) {
			for (const auto x : rep(kBoardSize)) {
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
				default: break;
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
