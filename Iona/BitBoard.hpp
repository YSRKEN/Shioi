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
* @fn operator
* ~japanese	@brief __m256i向けの演算子オーバーロード
* ~english	@brief operator override for __m256i
*/
__m256i operator | (const __m256i a, const __m256i b) noexcept {return _mm256_or_si256(a, b);}
__m256i operator & (const __m256i a, const __m256i b) noexcept {return _mm256_and_si256(a, b);}
__m256i operator == (const __m256i a, const __m256i b) noexcept { return _mm256_cmpeq_epi64(a, b); }
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
	* ~japanese	@brief BitBoardにおけるpositionの位置のビットを立てる
	* ~english	@brief Set "1" to bit of position in BitBoard
	*/
	bool GetBit(const size_t position) const noexcept {
		auto temp = board_ & kPositionArray[position];
		auto temp2 = (temp == kPackedZero);
		auto temp3 = _mm256_movemask_epi8(temp2);
		return (temp3 != 0xFFFFFFFF);
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
		for (size_t y = 0; y < kBoardSize; ++y) {
			for (size_t x = 0; x < kBoardSize; ++x) {
				size_t position = ToPosition(x, y);
				uint16_t line[sizeof(__m256i) / sizeof(uint16_t)]{};
				line[y] = 1 << x;
				kPositionArray[position] = _mm256_loadu_si256((__m256i*)line);
			}
		}
	}
};
