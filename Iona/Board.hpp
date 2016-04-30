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
	* ~japanese	@brief 方向・先後・左右・シフト数に応じたシフトパターンを作成する
	* ~english	@brief Make shift pattern
	*/
	ShiftPattern GetShiftPattern() {
		ShiftPattern shift_Pattern;
		REP(dir_, Direction::Directions) {
			const auto dir = static_cast<Direction>(dir_);
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
	* ~japanese	@brief 長連を起こす石の位置を算出する(黒石用)
	* ~english	@brief Make position mask of Cho-ren for Stone::Black
	*/
	BitBoard CalcChorenMaskB(const ShiftPattern &pattern) {
		BitBoard choren_mask;
		REP(dir, Direction::Directions) {
			auto& p = pattern[dir];
			constexpr size_t B = Stone::Black;
			//constexpr size_t N = Stone::None;
			constexpr size_t L = Side::Left;
			constexpr size_t R = Side::Right;

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
	BitBoard CalcLineShiShiMaskB(const ShiftPattern &pattern) {
		BitBoard shishi1_mask;
		REP(dir, Direction::Directions) {
			auto& p = pattern[dir];
			constexpr size_t B = Stone::Black;
			constexpr size_t N = Stone::None;
			constexpr size_t b = Stone::NonBlack;
			constexpr size_t L = Side::Left;
			constexpr size_t R = Side::Right;

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
	array<BitBoard, Direction::Directions> CalcShirenMaskB(const ShiftPattern &pattern) {
		array<BitBoard, Direction::Directions> shiren_mask;
		REP(dir, Direction::Directions) {
			auto& p = pattern[dir];
			auto& m = BitBoard::kBitMaskArray[dir];
			constexpr size_t B = Stone::Black;
			constexpr size_t N = Stone::None;
			constexpr size_t b = Stone::NonBlack;
			constexpr size_t L = Side::Left;
			constexpr size_t R = Side::Right;

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
