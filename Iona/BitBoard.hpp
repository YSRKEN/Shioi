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

struct BitBoard;
extern BitBoard kPositionArray[kAllBoardSize];
extern BitBoard kBitMaskArray[Direction::Directions][Side::Sides][kMaxShifts];
bool IsZero(const BitBoard&) noexcept;

/**
* @class BitBoard
* @brief BitBoard class for game board
*/
struct BitBoard {
	/**
	* @brief Member variable
	*/
	union
	{
		__m256i board_;
		uint16_t line_[16];
	};
	//! mask constant
	static const BitBoard kBitMaskR, kBitMaskL, kBitMaskU, kBitMaskD;
	static const BitBoard kBitMaskRU;
	static const BitBoard kBitMaskRD;
	static const BitBoard kBitMaskLD;
	static const BitBoard kBitMaskLU;
	static const BitBoard BoardFill;
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
	explicit operator __m256i() const noexcept { return board_; }
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
				if ((*this)[position]) {
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
				kBitMaskArray[dir][Side::Left][shift + 1] = BitBoard(kBitMaskArray[dir][Side::Left][shift]) >> (Direction)dir;
				kBitMaskArray[dir][Side::Right][shift + 1] = BitBoard(kBitMaskArray[dir][Side::Right][shift]) << (Direction)dir;
			}
		}
	}
	/**
	* ~japanese	@brief 各種演算子を定義する
	* ~english	@brief Definition of operator for BitBoard
	*/
	BitBoard operator | (const BitBoard& a) const noexcept { return _mm256_or_si256(board_, a.board_); }
	BitBoard operator & (const BitBoard& a) const noexcept { return _mm256_and_si256(board_, a.board_); }
	BitBoard operator ^ (const BitBoard& a) const noexcept { return _mm256_xor_si256(board_, a.board_); }
	BitBoard operator ! () const noexcept { return (*this ^ BoardFill); }
	bool operator [] (const size_t p) const noexcept { return !IsZero(*this & kPositionArray[p]); }
	bool operator == (const BitBoard& a) const noexcept {
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
		return IsZero(*this ^ a);
	}
	BitBoard operator << (const Direction dir) const noexcept {
		/**
		* 【通常のコード】
		* _mm256_store_si256でalignas(32) uint16_t temp[17]に書き出した後、
		* _mm256_loadu_si256で1要素だけズラして読み込む。その際、ビットシフトなのでマスク処理を
		* 最後に施す必要があるのと、アラインメントと16bit変数の相性が悪いので注意。
		* 【高度なコード】
		* _mm256_permute2x128_si256で適宜再配置した__m256i型を用意し、
		* _mm256_alignr_epi8で読み出す。アラインメント処理などを考えなくていいのが利点。
		* http://stackoverflow.com/questions/25248766/emulating-shifts-on-32-bytes-with-avx
		*/
		switch (dir) {
		case Direction::Row:
			return BitBoard(_mm256_slli_epi16(board_, 1)) & kBitMaskL;
			break;
		case Direction::Column:
			return BitBoard(_mm256_alignr_epi8(board_, _mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(0, 0, 2, 0)), 16 - 2)) & kBitMaskU;
			break;
		case Direction::DiagR:
			return BitBoard(_mm256_srli_epi16(_mm256_alignr_epi8(board_, _mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(0, 0, 2, 0)), 16 - 2), 1)) & kBitMaskRU;
			break;
		case Direction::DiagL:
			return BitBoard(_mm256_slli_epi16(_mm256_alignr_epi8(board_, _mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(0, 0, 2, 0)), 16 - 2), 1)) & kBitMaskLU;
			break;
		default:
			return *this;
		}
	}
	BitBoard& operator <<= (const Direction dir) noexcept { *this = *this << dir; return *this; }
	BitBoard operator >> (const Direction dir) const noexcept {
		/**
		* 【通常のコード】
		* _mm256_store_si256でalignas(32) uint16_t temp[17]に書き出した後、
		* _mm256_loadu_si256で1要素だけズラして読み込む。その際、ビットシフトなのでマスク処理を
		* 最後に施す必要があるのと、アラインメントと16bit変数の相性が悪いので注意。
		* 【高度なコード】
		* _mm256_permute2x128_si256で適宜再配置した__m256i型を用意し、
		* _mm256_alignr_epi8で読み出す。アラインメント処理などを考えなくていいのが利点。
		* http://stackoverflow.com/questions/25248766/emulating-shifts-on-32-bytes-with-avx
		*/
		switch (dir) {
		case Direction::Row:
			return BitBoard(_mm256_srli_epi16(board_, 1)) & kBitMaskR;
			break;
		case Direction::Column:
			return BitBoard(_mm256_alignr_epi8(_mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(2, 0, 0, 1)), board_, 2)) & kBitMaskD;
			break;
		case Direction::DiagR:
			return BitBoard(_mm256_slli_epi16(_mm256_alignr_epi8(_mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(2, 0, 0, 1)), board_, 2), 1)) & kBitMaskLD;
			break;
		case Direction::DiagL:
			return BitBoard(_mm256_srli_epi16(_mm256_alignr_epi8(_mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(2, 0, 0, 1)), board_, 2), 1)) & kBitMaskRD;
			break;
		default:
			return *this;
		}
	}
	BitBoard& operator >>= (const Direction dir) noexcept { *this = *this >> dir; return *this; }
	BitBoard& operator |= (const BitBoard& a) noexcept { board_ = _mm256_or_si256(board_, a.board_); return *this; }
	BitBoard& operator &= (const BitBoard& a) noexcept { board_ = _mm256_and_si256(board_, a.board_); return *this; }
	BitBoard& operator ^= (const BitBoard& a) noexcept { board_ = _mm256_xor_si256(board_, a.board_); return *this; }
};
