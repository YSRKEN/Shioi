/**
* @file BitBoard.hpp
* ~japanese	@brief ビットボードをAVX2で定義する
* ~english	@brief Definition of BitBoard used by AVX2
* @author YSR
* @date 2016/04/26
*/
#pragma once
#include<cstdint>
#include<immintrin.h>
#include"constant.hpp"
#include"misc_functions.hpp"

//! zero of __m256i
__m256i kPackedZero{};
//! constant table
__m256i kPositionArray[kAllBoardSize];
/**
* @fn IsZero
* ~japanese	@brief __m256iのビットが全て0か調べる
* ~english	@brief is all bits of __m256i "0" ?
*/
bool IsZero(const __m256i a) noexcept {
	/**
	* _mm256_testz_si256は、2つの引数のANDを取り、全てのビットが0ならZFフラグを設定し、
	* そうでなければZFフラグをクリアする。ZFフラグが設定されるとこの関数の返り値が非0になり、
	* そうでなければ返り値が0になる。つまり、引数に両方aを噛ますと、aがオール0ならば
	* 返り値が非0、そうでなければ返り値が0になる。
	*/
	return (_mm256_testz_si256(a, a) != 0);
}
/**
* @fn operator |
* ~japanese	@brief __m256i向けの演算子オーバーロード(OR)
* ~english	@brief operator override for __m256i(OR)
*/
__m256i operator | (const __m256i a, const __m256i b) noexcept {return _mm256_or_si256(a, b);}
/**
* @fn operator &
* ~japanese	@brief __m256i向けの演算子オーバーロード(AND)
* ~english	@brief operator override for __m256i(AND)
*/
__m256i operator & (const __m256i a, const __m256i b) noexcept {return _mm256_and_si256(a, b);}
/**
* @fn operator ^
* ~japanese	@brief __m256i向けの演算子オーバーロード(XOR
* ~english	@brief operator override for __m256i(XOR)
*/
__m256i operator ^ (const __m256i a, const __m256i b) noexcept { return _mm256_xor_si256(a, b); }
/**
* @fn operator ==
* ~japanese	@brief __m256i向けの演算子オーバーロード(Equal)
* ~english	@brief operator override for __m256i(Equal)
*/
bool operator == (const __m256i a, const __m256i b) noexcept {
	/**
	* 【通常のコード】
	* _mm256_cmpeq_epi64は、__m256iを64bit毎に分割した要素毎に比較を行い、
	* 等しければ要素の全ビットに1を、等しくなければ0を立てる。
	* _mm256_movemask_epi8は__m256iを8bit毎に分割した要素の最上位ビットをかき集める。
	* 256/8=32なので、int型の変数に集約されることになるが、先ほどより0か1かなので、
	* 結局完全一致してたら0xFFFFFFFF、そうでなければどこかが異なることになる。
	*/
	/*auto temp = a & b;
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

/**
* @class BitBoard
* @brief BitBoard class for game board
*/
struct BitBoard {
	union
	{
		__m256i board_;
		uint16_t line_[16];
	};
	/**
	* @fn BitBoard
	* @brief Constructor
	*/
	BitBoard() {
		board_ = __m256i{};
	}
	/**
	* @fn GetBit
	* ~japanese	@brief BitBoardにおけるpositionの位置のビットを調べる
	* ~english	@brief Get bit of position in BitBoard
	*/
	bool GetBit(const size_t position) const noexcept {
		return !IsZero(board_ & kPositionArray[position]);
	}
	/**
	* @fn SetBit
	* ~japanese	@brief BitBoardにおけるpositionの位置のビットを立てる
	* ~english	@brief Set "1" to bit of position in BitBoard
	*/
	void SetBit(const size_t position) noexcept {
		board_ = board_ | kPositionArray[position];
	}
	/**
	* @fn Initialize
	* ~japanese	@brief kPositionArrayを初期化する
	* ~english	@brief Initialize of kPositionArray
	*/
	static void Initialize() {
		REP(y, kBoardSize){
			REP(x, kBoardSize) {
				size_t position = ToPosition(x, y);
				uint16_t line[sizeof(__m256i) / sizeof(uint16_t)]{};
				line[y] = 1 << x;
				kPositionArray[position] = _mm256_loadu_si256((__m256i*)line);
			}
		}
	}
};
