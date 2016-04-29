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
#include <type_traits>

inline __m256i operator | (const __m256i a, const __m256i b) noexcept {return _mm256_or_si256(a, b);}
inline __m256i& operator |= (__m256i& a, const __m256i b) noexcept { a = _mm256_or_si256(a, b); return a; }
inline __m256i operator & (const __m256i a, const __m256i b) noexcept {return _mm256_and_si256(a, b);}
inline __m256i operator ^ (const __m256i a, const __m256i b) noexcept { return _mm256_xor_si256(a, b); }
inline bool operator==(const __m256i l, std::nullptr_t) noexcept {
	/**
	* _mm256_testz_si256は、2つの引数のANDを取り、全てのビットが0ならZFフラグを設定し、
	* そうでなければZFフラグをクリアする。ZFフラグが設定されるとこの関数の返り値が非0になり、
	* そうでなければ返り値が0になる。つまり、引数に両方aを噛ますと、aがオール0ならば
	* 返り値が非0、そうでなければ返り値が0になる。
	*/
	return (_mm256_testz_si256(l, l) != 0);
}
inline bool operator==(std::nullptr_t, const __m256i r) noexcept { return 0 == r; }
inline bool operator == (const __m256i a, const __m256i b) noexcept {
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
	return IsZero(a ^ b);
}
bool operator!=(const __m256i& l, const __m256i& r) { return !(l == r); }
bool operator!=(std::nullptr_t l, const __m256i& r) { return !(l == r); }
bool operator!=(const __m256i& l, std::nullptr_t r) { return !(l == r); }

/**
* @class BitBoard
* @brief BitBoard class for game board
*/
struct BitBoard {
	//! constant table
	static const std::array<__m256i, kAllBoardSize> kPositionArray;
	//! mask constant
	static const __m256i kBitMaskR;
	static const __m256i kBitMaskL;
	static const __m256i kBitMaskU;
	static const __m256i kBitMaskD;
	static const __m256i kBitMaskRU;
	static const __m256i kBitMaskRD;
	static const __m256i kBitMaskLD;
	static const __m256i kBitMaskLU;
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
		return 0 != (board_ & kPositionArray[position]);
	}
	/**
	* ~japanese	@brief BitBoardにおけるpositionの位置のビットを立てる
	* ~english	@brief Set "1" to bit of position in BitBoard
	*/
	void SetBit(const size_t position) noexcept {
		board_ |= kPositionArray[position];
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
	BitBoard operator|(const BitBoard& r) const { return this->board_ | r.board_; }
	BitBoard& operator|=(const BitBoard& r) noexcept { *this = *this | r; return *this; }
	BitBoard operator&(const BitBoard& r) const { return this->board_ & r.board_; }
};
inline __m256i operator & (const __m256i a, const BitBoard& b) noexcept { return a & b.board_; }
