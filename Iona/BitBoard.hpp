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
#include"types.hpp"
#include"constant.hpp"
#include"misc_functions.hpp"

//! constant table
__m256i kPositionArray[kAllBoardSize];
__m256i kBitMaskArray[Direction::Directions][Side::Sides][kMaxShifts];
//! mask constant
__m256i AllBit1 = _mm256_set1_epi16(0xFFFFu);
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
__m256i operator | (const __m256i a, const __m256i b) noexcept {return _mm256_or_si256(a, b);}
__m256i operator & (const __m256i a, const __m256i b) noexcept {return _mm256_and_si256(a, b);}
__m256i operator ^ (const __m256i a, const __m256i b) noexcept { return _mm256_xor_si256(a, b); }
__m256i operator ! (const __m256i a) noexcept { return _mm256_xor_si256(a, AllBit1); }
bool operator == (const __m256i a, const __m256i b) noexcept {
	/**
	* 【通常のコード】
	* _mm256_cmpeq_epi64は、__m256iを64bit毎に分割した要素毎に比較を行い、
	* 等しければ要素の全ビットに1を、等しくなければ0を立てる。
	* _mm256_movemask_epi8は__m256iを8bit毎に分割した要素の最上位ビットをかき集める。
	* 256/8=32なので、int型の変数に集約されることになるが、先ほどより0か1かなので、
	* 結局完全一致してたら0xFFFFFFFF、そうでなければどこかが異なることになる。
	*/
	/**
	* 【高度なコード】
	* 二引数のXORを取れば、等しいビットは全て0、等しくないビットは全て1になる。
	* つまり、全ビットが0ならば等しく、そうでなければ等しくないと言える。
	*/
	return IsZero(a ^ b);
}
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
const __m256i kBitMaskRU = kBitMaskR & kBitMaskU;
const __m256i kBitMaskRD = kBitMaskR & kBitMaskD;
const __m256i kBitMaskLD = kBitMaskL & kBitMaskD;
const __m256i kBitMaskLU = kBitMaskL & kBitMaskU;

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
	* @brief Constructor
	*/
	BitBoard() noexcept {
		board_ = __m256i{};
	}
	/**
	* @brief Constructor
	*/
	BitBoard(const __m256i a) noexcept{
		board_ = a;
	}
	/**
	* @brief Cast operator(to __m256i)
	*/
	operator __m256i() const { return board_; }
	/**
	* ~japanese	@brief BitBoardにおけるpositionの位置のビットを調べる
	* ~english	@brief Get bit of position in BitBoard
	*/
	bool GetBit(const size_t position) const noexcept {
		return !IsZero(board_ & kPositionArray[position]);
	}
	/**
	* ~japanese	@brief BitBoardにおけるpositionの位置のビットを立てる
	* ~english	@brief Set "1" to bit of position in BitBoard
	*/
	void SetBit(const size_t position) noexcept {
		board_ = board_ | kPositionArray[position];
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
	* ~japanese	@brief kPositionArrayとkBitMaskArrayを初期化する
	* ~english	@brief Initialize of kPositionArray and kBitMaskArray
	*/
	static void Initialize() {
		//! kPositionArray
		REP(y, kBoardSize){
			REP(x, kBoardSize) {
				size_t position = ToPosition(x, y);
				uint16_t line[sizeof(__m256i) / sizeof(uint16_t)]{};
				line[y] = 1 << x;
				kPositionArray[position] = _mm256_loadu_si256((__m256i*)line);
			}
		}
		//! kBitMaskArray
		kBitMaskArray[Direction::Row][Side::Left][0]     = kBitMaskL;
		kBitMaskArray[Direction::Column][Side::Left][0]  = kBitMaskU;
		kBitMaskArray[Direction::DiagR][Side::Left][0]   = kBitMaskRU;
		kBitMaskArray[Direction::DiagL][Side::Left][0]   = kBitMaskLU;
		kBitMaskArray[Direction::Row][Side::Right][0]    = kBitMaskR;
		kBitMaskArray[Direction::Column][Side::Right][0] = kBitMaskD;
		kBitMaskArray[Direction::DiagR][Side::Right][0]  = kBitMaskLD;
		kBitMaskArray[Direction::DiagL][Side::Right][0]  = kBitMaskRD;
		REP(dir, Direction::Directions) {
			REP(shift, kMaxShifts - 1) {
				kBitMaskArray[dir][Side::Left][shift + 1] = BitBoard(kBitMaskArray[dir][Side::Left][shift]).ShiftLeft((Direction)dir);
				kBitMaskArray[dir][Side::Right][shift + 1] = BitBoard(kBitMaskArray[dir][Side::Right][shift]).ShiftRight((Direction)dir);
			}
		}
		return;
	}
	/**
	* ~japanese	@brief BitBoardを、dir方向における「左」にシフトする
	* ~english	@brief shift BitBoard to "Left" direction
	*/
	BitBoard ShiftRight(const Direction dir) noexcept {
		switch (dir) {
		case Direction::Row:
			return BitBoard(_mm256_srli_epi16(board_, 1) & kBitMaskR);
			break;
		case Direction::Column:
			//! 最初考えていた方法
			/*{alignas(32) uint16_t temp[17]{};
			_mm256_store_si256((__m256i*)temp, board_);
			return BitBoard(_mm256_loadu_si256((__m256i*)(temp + 1)) & kBitMaskU);}*/
			//! 恐るべき解決手段
			//! http://stackoverflow.com/questions/25248766/emulating-shifts-on-32-bytes-with-avx
			return BitBoard(_mm256_alignr_epi8(_mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(2, 0, 0, 1)), board_, 2) & kBitMaskD);
			break;
		case Direction::DiagR:
			//! 最初考えていた方法
			/*{alignas(32) uint16_t temp[17]{};
			_mm256_store_si256((__m256i*)temp, board_);
			return BitBoard(_mm256_slli_epi16(_mm256_loadu_si256((__m256i*)(temp + 1)), 1) & kBitMaskLU);}*/
			//! 恐るべき解決手段
			return BitBoard(_mm256_slli_epi16(_mm256_alignr_epi8(_mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(2, 0, 0, 1)), board_, 2), 1) & kBitMaskLD);
			break;
		case Direction::DiagL:
			//! 最初考えていた方法
			/*{alignas(32) uint16_t temp[17]{};
			_mm256_store_si256((__m256i*)temp, board_);
			return BitBoard(_mm256_srli_epi16(_mm256_loadu_si256((__m256i*)(temp + 1)), 1) & kBitMaskRU);}*/
			//! 恐るべき解決手段
			return BitBoard(_mm256_srli_epi16(_mm256_alignr_epi8(_mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(2, 0, 0, 1)), board_, 2), 1) & kBitMaskRD);
			break;
		default:
			return *this;
		}
	}
	/**
	* ~japanese	@brief BitBoardを、dir方向における「右」にシフトする
	* ~english	@brief shift BitBoard to "Right" direction
	*/
	BitBoard ShiftLeft(const Direction dir) noexcept {
		switch (dir) {
		case Direction::Row:
			return BitBoard(_mm256_slli_epi16(board_, 1) & kBitMaskL);
			break;
		case Direction::Column:
			//! 最初考えていた方法
			/*{alignas(32) uint16_t temp[17]{};
			_mm256_storeu_si256((__m256i*)(temp + 1), board_);
			return BitBoard(_mm256_load_si256((__m256i*)temp) & kBitMaskD);}*/
			//! 恐るべき解決手段
			return BitBoard(_mm256_alignr_epi8(board_, _mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(0, 0, 2, 0)), 16 - 2) & kBitMaskU);
		case Direction::DiagR:
			//! 最初考えていた方法
			/*{alignas(32) uint16_t temp[17]{};
			_mm256_storeu_si256((__m256i*)(temp + 1), board_);
			return BitBoard(_mm256_srli_epi16(_mm256_load_si256((__m256i*)temp), 1) & kBitMaskRD);}*/
			//! 恐るべき解決手段
			return BitBoard(_mm256_srli_epi16(_mm256_alignr_epi8(board_, _mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(0, 0, 2, 0)), 16 - 2), 1) & kBitMaskRU);
			break;
		case Direction::DiagL:
			//! 最初考えていた方法
			/*{alignas(32) uint16_t temp[17]{};
			_mm256_storeu_si256((__m256i*)(temp + 1), board_);
			return BitBoard(_mm256_slli_epi16(_mm256_load_si256((__m256i*)temp), 1) & kBitMaskLD);}*/
			//! 恐るべき解決手段
			return BitBoard(_mm256_slli_epi16(_mm256_alignr_epi8(board_, _mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(0, 0, 2, 0)), 16 - 2), 1) & kBitMaskLU);
			break;
		default:
			return *this;
		}
	}
	/**
	* ~japanese	@brief BitBoardを、dir方向における「左」にシフトする(破壊的変更)
	* ~english	@brief shift BitBoard to "Left" direction(Destructive change)
	*/
	void ShiftLeftD(const Direction dir) noexcept {
		board_ = ShiftLeft(dir);
	}
	/**
	* ~japanese	@brief BitBoardを、dir方向における「右」にシフトする(破壊的変更)
	* ~english	@brief shift BitBoard to "Right" direction(Destructive change)
	*/
	void ShiftRightD(const Direction dir) noexcept {
		board_ = ShiftRight(dir);
	}
	BitBoard& operator |= (const __m256i a) noexcept { board_ = _mm256_or_si256(board_, a); return *this; }
	BitBoard& operator ^= (const __m256i a) noexcept { board_ = _mm256_xor_si256(board_, a); return *this; }
};
