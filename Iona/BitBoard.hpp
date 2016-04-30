/**
* @file BitBoard.hpp
* ~japanese	@brief ビットボードをAVX2で定義する
* ~english	@brief Definition of BitBoard used by AVX2
* @author YSR
* @date 2016/04/26
*/
#pragma once
#include<cstdint>
#include<iostream>
#include<immintrin.h>
#include<array>
#include"types.hpp"
#include"constant.hpp"
#include"misc_functions.hpp"
using std::array;
/**
* @class BitBoard
* @brief BitBoard class for game board
*/
struct BitBoard {
	//! constant table
	static const std::array<__m256i, kAllBoardSize> kPositionArray;
	static const array<array<array<BitBoard, kMaxShifts>, Side::Sides>, Direction::Directions> kBitMaskArray;
	//! mask constant
	static const __m256i kBitMaskR;
	static const __m256i kBitMaskL;
	static const __m256i kBitMaskU;
	static const __m256i kBitMaskD;
	static const __m256i kBitMaskRU;
	static const __m256i kBitMaskRD;
	static const __m256i kBitMaskLD;
	static const __m256i kBitMaskLU;
	static const __m256i AllBit1;
	union
	{
		__m256i board_;
		uint16_t line_[16];
	};
	BitBoard() = default;
	BitBoard(const __m256i a) noexcept;
	BitBoard(const BitBoard&) = default;
	BitBoard(BitBoard&&) = default;
	BitBoard& operator=(const BitBoard&) = default;
	BitBoard& operator=(BitBoard&&) = default;
	/**
	* @brief Cast operator(to __m256i)
	*/
	explicit operator __m256i() const { return board_; }
	/**
	* ~japanese	@brief BitBoardにおけるpositionの位置のビットを調べる
	* ~english	@brief Get bit of position in BitBoard
	*/
	bool GetBit(const size_t position) const noexcept {
		return (*this & kPositionArray[position]) != 0;
	}
	/**
	* ~japanese	@brief BitBoardにおけるpositionの位置のビットを立てる
	* ~english	@brief Set "1" to bit of position in BitBoard
	*/
	void SetBit(const size_t position) noexcept {
		*this |= kPositionArray[position];
	}
	/**
	* @fn PutBoard
	* @brief Put text of board for debug
	*/
	void PutBoard() const noexcept {
		using std::cout;
		using std::endl;
		REP(y, kBoardSize) {
			REP(x, kBoardSize) {
				auto position = ToPosition(x, y);
				if (GetBit(position)) {
					cout << "□";
				}else{
					if (position == ToPosition(0, 0)) {
						cout << "┌";
					}
					else if (position == ToPosition(0, kBoardSize - 1)) {
						cout << "└";
					}
					else if (position == ToPosition(kBoardSize - 1, 0)) {
						cout << "┐";
					}
					else if (position == ToPosition(kBoardSize - 1, kBoardSize - 1)) {
						cout << "┘";
					}
					else if (position == ToPosition(3, 3)
						|| position == ToPosition(3, 11)
						|| position == ToPosition(11, 3)
						|| position == ToPosition(11, 11)
						|| position == ToPosition(7, 7)) {
						cout << "╋";
					}
					else if (x == 0) {
						cout << "├";
					}
					else if (x == kBoardSize - 1) {
						cout << "┤";
					}
					else if (y == 0) {
						cout << "┬";
					}
					else if (y == kBoardSize - 1) {
						cout << "┴";
					}
					else {
						cout << "┼";
					}
				}
			}
			cout << endl;
		}
		return;
	}
	/**
	* ~japanese	@brief BitBoardを、dir方向における「左」にシフトする
	* ~english	@brief shift BitBoard to "Left" direction
	*/
	BitBoard operator<<(const Direction dir) const noexcept;
	/**
	* ~japanese	@brief BitBoardを、dir方向における「右」にシフトする
	* ~english	@brief shift BitBoard to "Right" direction
	*/
	BitBoard operator>>(const Direction dir) const noexcept;
	/**
	* ~japanese	@brief BitBoardを、dir方向における「左」にシフトする(破壊的変更)
	* ~english	@brief shift BitBoard to "Left" direction(Destructive change)
	*/
	BitBoard& operator<<=(const Direction dir) noexcept {
		*this = *this << dir;
		return *this;
	}
	/**
	* ~japanese	@brief BitBoardを、dir方向における「右」にシフトする(破壊的変更)
	* ~english	@brief shift BitBoard to "Right" direction(Destructive change)
	*/
	BitBoard& operator>>=(const Direction dir) noexcept {
		*this = *this >> dir;
		return *this;
	}
	BitBoard operator|(const BitBoard& r) const noexcept { return _mm256_or_si256(this->board_, r.board_); }
	BitBoard& operator|=(const BitBoard& r) noexcept { *this = *this | r; return *this; }
	BitBoard operator&(const BitBoard& r) const noexcept { return _mm256_and_si256(this->board_, r.board_); }
	inline bool operator==(std::nullptr_t) const noexcept {
		/**
		* _mm256_testz_si256は、2つの引数のANDを取り、全てのビットが0ならZFフラグを設定し、
		* そうでなければZFフラグをクリアする。ZFフラグが設定されるとこの関数の返り値が非0になり、
		* そうでなければ返り値が0になる。つまり、引数に両方aを噛ますと、aがオール0ならば
		* 返り値が非0、そうでなければ返り値が0になる。
		*/
		return (_mm256_testz_si256(this->board_, this->board_) != 0);
	}
	BitBoard operator^(const BitBoard& r) const noexcept { return _mm256_xor_si256(this->board_, r.board_); }
	bool operator == (const BitBoard& r) const noexcept {
		/**
		* 【通常のコード】
		* _mm256_cmpeq_epi64は、__m256iを64bit毎に分割した要素毎に比較を行い、
		* 等しければ要素の全ビットに1を、等しくなければ0を立てる。
		* _mm256_movemask_epi8は__m256iを8bit毎に分割した要素の最上位ビットをかき集める。
		* 256/8=32なので、int型の変数に集約されることになるが、先ほどより0か1かなので、
		* 結局完全一致してたら0xFFFFFFFF、そうでなければどこかが異なることになる。
		*/
		/*auto temp = a & b;
		const __m256i kPackedZero{};
		auto temp2 = _mm256_cmpeq_epi64(temp, kPackedZero);
		auto temp3 = _mm256_movemask_epi8(temp2);
		return (temp3 != 0xFFFFFFFF);*/
		/**
		* 【高度なコード】
		* 二引数のXORを取れば、等しいビットは全て0、等しくないビットは全て1になる。
		* つまり、全ビットが0ならば等しく、そうでなければ等しくないと言える。
		*/
		return (*this ^ r) == 0;
	}
	bool operator!=(const BitBoard& r) const noexcept { return !(*this == r); }
	bool operator!=(std::nullptr_t) const noexcept { return !(*this == 0); }
	BitBoard operator! () const noexcept { return _mm256_xor_si256(this->board_, AllBit1); }
};
inline bool operator==(std::nullptr_t, const BitBoard& r) noexcept { return r == 0; }
inline bool operator!=(std::nullptr_t l, const BitBoard& r) { return !(l == r); }
inline BitBoard operator & (const __m256i a, const BitBoard& b) noexcept { return _mm256_and_si256(a, b.board_); }
template<Side side> inline BitBoard sift(const BitBoard& l , Direction r) noexcept;
template<> inline BitBoard sift<Side::Left>(const BitBoard& l , Direction r) noexcept { return l << r; }
template<> inline BitBoard sift<Side::Right>(const BitBoard& l , Direction r) noexcept { return l >> r; }
template<Side side> inline BitBoard& sift_assign(BitBoard& l , Direction r) noexcept { l =  sift<side>(l, r); return l; }
