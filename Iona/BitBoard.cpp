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
const BitBoard kBitMaskR = _mm256_set_epi16(
	0x0000u, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu,
	0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu);
const BitBoard kBitMaskL = _mm256_set_epi16(
	0x0000u, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu,
	0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu);
const BitBoard kBitMaskU = _mm256_set_epi16(
	0x0000u, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu,
	0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x0000u);
const BitBoard kBitMaskD = _mm256_set_epi16(
	0x0000u, 0x0000u, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu,
	0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu);
const BitBoard kBitMaskRU = _mm256_and_si256(kBitMaskR.board_, kBitMaskU.board_);
const BitBoard kBitMaskRD = _mm256_and_si256(kBitMaskR.board_, kBitMaskD.board_);
const BitBoard kBitMaskLD = _mm256_and_si256(kBitMaskL.board_, kBitMaskD.board_);
const BitBoard kBitMaskLU = _mm256_and_si256(kBitMaskL.board_, kBitMaskU.board_);
const BitBoard BoardFill = _mm256_set_epi16(
	0x0000u, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu,
	0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu);
/**
* @fn IsZero
* ~japanese	@brief __m256iのビットが全て0か調べる
* ~english	@brief is all bits of __m256i "0" ?
*/
bool IsZero(const BitBoard& a) noexcept {
	/**
	* _mm256_testz_si256は、2つの引数のANDを取り、全てのビットが0ならZFフラグを設定し、
	* そうでなければZFフラグをクリアする。ZFフラグが設定されるとこの関数の返り値が非0になり、
	* そうでなければ返り値が0になる。つまり、引数に両方aを噛ますと、aがオール0ならば
	* 返り値が非0、そうでなければ返り値が0になる。
	*/
	return (_mm256_testz_si256(a.board_, a.board_) != 0);
}
