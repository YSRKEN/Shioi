/**
* @file BitBoard.cpp
* ~japanese	@brief BitBoard.hppの実体部分
* ~english	@brief solution of ODR for BitBoard.hpp
* @author YSR
* @date 2016/05/02
*/
#include"BitBoard.hpp"

//! constant table
BitBoard kPositionArray[kAllBoardSize]{};
BitBoard kBitMaskArray[Direction::Directions][Side::Sides][kMaxShifts]{};
//! mask constant
const __m256i kBitMaskR = _mm256_set_epi16(
	0x0000u, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu,
	0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu);
const __m256i kBitMaskL = _mm256_set_epi16(
	0x0000u, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu,
	0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu);
const __m256i kBitMaskU = _mm256_set_epi16(
	0x0000u, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu,
	0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x0000u);
const __m256i kBitMaskD = _mm256_set_epi16(
	0x0000u, 0x0000u, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu,
	0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu);
const __m256i kBitMaskRU = _mm256_and_si256(kBitMaskR, kBitMaskU);
const __m256i kBitMaskRD = _mm256_and_si256(kBitMaskR, kBitMaskD);
const __m256i kBitMaskLD = _mm256_and_si256(kBitMaskL, kBitMaskD);
const __m256i kBitMaskLU = _mm256_and_si256(kBitMaskL, kBitMaskU);
const __m256i BoardFill = _mm256_set_epi16(
	0x0000u, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu,
	0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu);
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
