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
#include"BookDB.hpp"
#include"ShiftPattern.hpp"

using std::array;
using std::cout;
using std::endl;
using std::vector;

//! Definition of random_device
std::random_device rd;
//! Definition of pseudo-random generator
std::mt19937 mt(rd());

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
	ShiftPattern GetShiftPattern() const{
		ShiftPattern shift_Pattern;
		REP(dir_, Direction::Directions) {
			auto dir = static_cast<Direction>(dir_);
			auto pattern_black_right = black_board_ << dir;
			auto pattern_white_right = white_board_ << dir;
			auto pattern_black_left = black_board_ >> dir;
			auto pattern_white_left = white_board_ >> dir;
			REP(shift, kMaxShifts - 1) {
				shift_Pattern.SetData(dir, Stone::Black, Side::Right, shift, pattern_black_right);
				shift_Pattern.SetData(dir, Stone::White, Side::Right, shift, pattern_white_right);
				shift_Pattern.SetData(dir, Stone::None, Side::Right, shift, !(pattern_black_right | pattern_white_right));
				shift_Pattern.SetData(dir, Stone::NonBlack, Side::Right, shift, !pattern_black_right);
				shift_Pattern.SetData(dir, Stone::Black, Side::Left, shift, pattern_black_left);
				shift_Pattern.SetData(dir, Stone::White, Side::Left, shift, pattern_white_left);
				shift_Pattern.SetData(dir, Stone::None, Side::Left, shift, !(pattern_black_left | pattern_white_left));
				shift_Pattern.SetData(dir, Stone::NonBlack, Side::Left, shift, !pattern_black_left);
				pattern_black_right <<= dir;
				pattern_white_right <<= dir;
				pattern_black_left >>= dir;
				pattern_white_left >>= dir;
			}
			shift_Pattern.SetData(dir, Stone::Black, Side::Right, kMaxShifts - 1, pattern_black_right);
			shift_Pattern.SetData(dir, Stone::White, Side::Right, kMaxShifts - 1, pattern_white_right);
			shift_Pattern.SetData(dir, Stone::None, Side::Right, kMaxShifts - 1, !(pattern_black_right | pattern_white_right));
			shift_Pattern.SetData(dir, Stone::NonBlack, Side::Right, kMaxShifts - 1, !pattern_black_right);
			shift_Pattern.SetData(dir, Stone::Black, Side::Left, kMaxShifts - 1, pattern_black_left);
			shift_Pattern.SetData(dir, Stone::White, Side::Left, kMaxShifts - 1, pattern_white_left);
			shift_Pattern.SetData(dir, Stone::None, Side::Left, kMaxShifts - 1, !(pattern_black_left | pattern_white_left));
			shift_Pattern.SetData(dir, Stone::NonBlack, Side::Left, kMaxShifts - 1, !pattern_black_left);
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
		BitBoard choren_mask = shift_pattern.CalcChorenMaskB();
		unorder_mask |= choren_mask;
		//! Shi-Shi on 1 line check
		BitBoard shishi1_mask = shift_pattern.CalcLineShiShiMaskB();
		unorder_mask |= shishi1_mask;
		//! Shi-ren check
		auto shiren_mask = shift_pattern.CalcShirenMaskB();
		SetPatternMask(shiren_mask, unorder_mask);
		unorder_mask |= (shiren_mask[0] | shiren_mask[1] | shiren_mask[2] | shiren_mask[3]);
		//! Katsu-Shi check
		auto katsushi_mask = shift_pattern.CalcKatsuShiMaskB();
		SetPatternMask(katsushi_mask, unorder_mask);
		unorder_mask |= (katsushi_mask[0] | katsushi_mask[1] | katsushi_mask[2] | katsushi_mask[3]);
		//! Calc Shi-Shi point
		BitBoard s1 = shiren_mask[0] | katsushi_mask[0], s2 = shiren_mask[1] | katsushi_mask[1],
			s3 = shiren_mask[2] | katsushi_mask[2], s4 = shiren_mask[3] | katsushi_mask[3];
		BitBoard shishi_mask = (s1 & s2) | (s1 & s3) | (s1 & s4) | (s2 & s3) | (s2 & s4) | (s3 & s4);
		//! Katsu-San check
		auto katsusan_mask = shift_pattern.CalcKatsuSanMaskB(unorder_mask);
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
		if (list.empty()) return optional<size_t>();
		return optional<size_t>(list[RandInt(list.size())]);
	}
	bool IsGameEnd(const BitBoard &board) {
		//! Row
		uint16_t mask = 0b11111;
		REP(j, kBoardSize - 5) {
			REP(i, kBoardSize) {
				if ((board.line_[i] & mask) == mask) return true;
			}
			mask <<= 1;
		}
		//! Column
		auto filter_column_base =
			kPositionArray[0] | kPositionArray[kBoardSize] | kPositionArray[kBoardSize * 2]
			| kPositionArray[kBoardSize * 3] | kPositionArray[kBoardSize * 4];
		REP(j, kBoardSize - 5 + 1) {
			auto filter_column = filter_column_base;
			REP(i, kBoardSize) {
				if ((board & filter_column) == filter_column) {
					return true;
				}
				filter_column <<= Direction::Row;
			}
			filter_column_base <<= Direction::Column;
		}
		//! DiagR
		auto filter_diagr_base =
			kPositionArray[4] | kPositionArray[kBoardSize + 3] | kPositionArray[kBoardSize * 2 + 2] | kPositionArray[kBoardSize * 3 + 1] | kPositionArray[kBoardSize * 4];
		REP(j, kBoardSize - 5 + 1) {
			auto filter_diagr = filter_diagr_base;
			REP(i, kBoardSize - 5 + 1) {
				if ((board & filter_diagr) == filter_diagr) {
					return true;
				}
				filter_diagr <<= Direction::Row;
			}
			filter_diagr_base <<= Direction::Column;
		}
		//! DiagL
		auto filter_diagl_base =
			kPositionArray[0] | kPositionArray[kBoardSize + 1] | kPositionArray[kBoardSize * 2 + 2] | kPositionArray[kBoardSize * 3 + 3] | kPositionArray[kBoardSize * 4 + 4];
		REP(j, kBoardSize - 5 + 1) {
			auto filter_diagl = filter_diagl_base;
			REP(i, kBoardSize - 5 + 1) {
				if ((board & filter_diagl) == filter_diagl) {
					return true;
				}
				filter_diagl <<= Direction::Row;
			}
			filter_diagl_base <<= Direction::Column;
		}
		return false;
	}
	bool IsGameEnd() {
		if (IsGameEnd(black_board_)) return true;
		if (IsGameEnd(white_board_)) return true;
		return false;
	}
	/**
	* @return Next Move(Goren)
	*/
	optional<size_t> FindGorenMove(const Stone turn) const noexcept{
		auto shift_pattern = GetShiftPattern();
		auto goren_mask = (turn == Stone::Black
			? CalcGorenMaskB(shift_pattern) & (!white_board_)
			: CalcGorenMaskW(shift_pattern) & (!black_board_));
		if (IsZero(goren_mask)) return optional<size_t>();
		REP(position, kAllBoardSize) {
			auto temp = kPositionArray[position] & goren_mask;
			if (!IsZero(kPositionArray[position] & goren_mask)) {
				return optional<size_t>(position);
			}
		}
		return optional<size_t>();
	}
	/**
	* @return Next Move(Stop Goren)
	*/
	optional<size_t> FindStopGorenMove(const Stone turn) noexcept {
		auto shift_pattern = GetShiftPattern();
		auto goren_mask = (turn == Stone::Black
			? CalcGorenMaskW(shift_pattern) & (!black_board_)
			: CalcGorenMaskB(shift_pattern) & (!white_board_));
		if (IsZero(goren_mask)) return optional<size_t>();
		if (turn == Stone::Black) {
			auto invalid_mask = CalcInValidMask();
			goren_mask &= !invalid_mask;
			if(IsZero(goren_mask)) return optional<size_t>(-1);
		}
		REP(position, kAllBoardSize) {
			auto temp = kPositionArray[position] & goren_mask;
			if (!IsZero(kPositionArray[position] & goren_mask)) {
				return optional<size_t>(position);
			}
		}
		return optional<size_t>();
	}
	/**
	* ~japanese	@brief 五連を起こす石の位置を算出する(黒石用)
	* ~english	@brief Make position mask of Cho-ren for Stone::Black
	*/
	inline BitBoard CalcGorenMaskB(const ShiftPattern &pattern) const noexcept {
		BitBoard goren_mask;
		REP(dir_, Direction::Directions) {
			auto dir = static_cast<Direction>(dir_);
			using namespace omission;
			auto &BL0 = pattern.GetData(dir, B, L, 0), &NL0 = pattern.GetData(dir, N, L, 0), &bL0 = pattern.GetData(dir, b, L, 0);
			auto &BL1 = pattern.GetData(dir, B, L, 1), &NL1 = pattern.GetData(dir, N, L, 1), &bL1 = pattern.GetData(dir, b, L, 1);
			auto &BL2 = pattern.GetData(dir, B, L, 2), &NL2 = pattern.GetData(dir, N, L, 2), &bL2 = pattern.GetData(dir, b, L, 2);
			auto &BL3 = pattern.GetData(dir, B, L, 3), &NL3 = pattern.GetData(dir, N, L, 3), &bL3 = pattern.GetData(dir, b, L, 3);
			auto &BL4 = pattern.GetData(dir, B, L, 4), &NL4 = pattern.GetData(dir, N, L, 4), &bL4 = pattern.GetData(dir, b, L, 4);
			auto &BR0 = pattern.GetData(dir, B, R, 0), &NR0 = pattern.GetData(dir, N, R, 0), &bR0 = pattern.GetData(dir, b, R, 0);
			auto &BR1 = pattern.GetData(dir, B, R, 1), &NR1 = pattern.GetData(dir, N, R, 1), &bR1 = pattern.GetData(dir, b, R, 1);
			auto &BR2 = pattern.GetData(dir, B, R, 2), &NR2 = pattern.GetData(dir, N, R, 2), &bR2 = pattern.GetData(dir, b, R, 2);
			auto &BR3 = pattern.GetData(dir, B, R, 3), &NR3 = pattern.GetData(dir, N, R, 3), &bR3 = pattern.GetData(dir, b, R, 3);
			auto &BR4 = pattern.GetData(dir, B, R, 4), &NR4 = pattern.GetData(dir, N, R, 4), &bR4 = pattern.GetData(dir, b, R, 4);
			auto &ML1 = kBitMaskArray[dir][L][0], &ML2 = kBitMaskArray[dir][L][1];
			auto &ML3 = kBitMaskArray[dir][L][2], &ML4 = kBitMaskArray[dir][L][3];
			auto &MR1 = kBitMaskArray[dir][R][0], &MR2 = kBitMaskArray[dir][R][1];
			auto &MR3 = kBitMaskArray[dir][R][2], &MR4 = kBitMaskArray[dir][R][3];

			//! [XBBBBBX]
			goren_mask |= bL0 /**/& BR0 & BR1 & BR2 & BR3 & bR4 & MR4;
			goren_mask |= BL0 & bL1 & ML1 /**/& BR0 & BR1 & BR2 & bR3 & MR3;
			goren_mask |= BL0 & BL1 & bL2 & ML2 /**/& BR0 & BR1 & bR2 & MR2;
			goren_mask |= BL0 & BL1 & BL2 & bL3 & ML3 /**/& BR0 & bR1 & MR1;
			goren_mask |= BL0 & BL1 & BL2 & BL3 & bL4 & ML4 /**/& bR0;
		}
		return goren_mask;
	}
	inline BitBoard CalcGorenMaskW(const ShiftPattern &pattern) const noexcept {
		BitBoard goren_mask;
		REP(dir_, Direction::Directions) {
			auto dir = static_cast<Direction>(dir_);
			using namespace omission;
			auto &WL0 = pattern.GetData(dir, W, L, 0), &NL0 = pattern.GetData(dir, N, L, 0);
			auto &WL1 = pattern.GetData(dir, W, L, 1), &NL1 = pattern.GetData(dir, N, L, 1);
			auto &WL2 = pattern.GetData(dir, W, L, 2), &NL2 = pattern.GetData(dir, N, L, 2);
			auto &WL3 = pattern.GetData(dir, W, L, 3), &NL3 = pattern.GetData(dir, N, L, 3);
			auto &WR0 = pattern.GetData(dir, W, R, 0), &NR0 = pattern.GetData(dir, N, R, 0);
			auto &WR1 = pattern.GetData(dir, W, R, 1), &NR1 = pattern.GetData(dir, N, R, 1);
			auto &WR2 = pattern.GetData(dir, W, R, 2), &NR2 = pattern.GetData(dir, N, R, 2);
			auto &WR3 = pattern.GetData(dir, W, R, 3), &NR3 = pattern.GetData(dir, N, R, 3);
			auto &ML1 = kBitMaskArray[dir][L][0], &ML2 = kBitMaskArray[dir][L][1];
			auto &ML3 = kBitMaskArray[dir][L][2], &ML4 = kBitMaskArray[dir][L][3];
			auto &MR1 = kBitMaskArray[dir][R][0], &MR2 = kBitMaskArray[dir][R][1];
			auto &MR3 = kBitMaskArray[dir][R][2], &MR4 = kBitMaskArray[dir][R][3];

			//! [WWWWW]
			goren_mask |= WR0 & WR1 & WR2 & WR3 & MR3;
			goren_mask |= WL0 /**/& WR0 & WR1 & WR2 & MR2;
			goren_mask |= WL0 & WL1 & ML1 /**/& WR0 & WR1 & MR1;
			goren_mask |= WL0 & WL1 & WL2 & ML2 /**/& WR0;
			goren_mask |= WL0 & WL1 & WL2 & WL3 & ML3;
		}
		return goren_mask;
	}

public:
	/**
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
	int NextMove(bool debug_flg = false) {
		//! If the game is end, you don't move.
		if (IsGameEnd()) return -1;
		//! Opening move
		if (debug_flg) std::cerr << "Opening" << endl;
		if (IsZero(black_board_) && IsZero(white_board_)) return ToPosition(7, 7);
		if (black_board_ == kPositionArray[112] && IsZero(white_board_)) return ToPosition(7 + RandInt(2), 6);
		//! Book move
//		if (debug_flg) std::cerr << "Book" << endl;
//		auto book = BookDB("book.csv");
//		auto book_data = book.GetBookData(black_board_, white_board_);
//		if (!book_data.empty()) return book_data[RandInt(book_data.size())];
		//! If you can make Go-ren, you must do it.
		if (debug_flg) std::cerr << "Goren" << endl;
		auto result = FindGorenMove(turn_);
		if (result) return *result;
		//! If enemy can make Go-ren, you should block it.
		if (debug_flg) std::cerr << "Stop Goren" << endl;
		result = FindStopGorenMove(turn_);
		if (result) return *result;
		//! Random move(test)
		if (debug_flg) std::cerr << "Random" << endl;
		result = FindRandomMove();
		if (result) return *result;
		return -1;
	}
	void Test() {
		const size_t count = 10000;
		auto start = std::chrono::system_clock::now();
		REP(i, count) {
			REP(j, count) {
				IsGameEnd();
			}
		}
		auto end = std::chrono::system_clock::now();
		auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		cout << msec << "[ms]" << endl;
	}
};
