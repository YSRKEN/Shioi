﻿#include "BitBoard.hpp"
#include "../I401/constant_range_loop.hpp"

const __m256i BitBoard::kBitMaskR = _mm256_set_epi16(
	0x0000u, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu,
	0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu, 0x3FFFu);
const __m256i BitBoard::kBitMaskL = _mm256_set_epi16(
	0x0000u, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu,
	0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu, 0x7FFEu);
const __m256i BitBoard::kBitMaskU = _mm256_set_epi16(
	0x0000u, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu,
	0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x0000u);
const __m256i BitBoard::kBitMaskD = _mm256_set_epi16(
	0x0000u, 0x0000u, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu,
	0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu);
const __m256i BitBoard::kBitMaskRU = _mm256_and_si256(BitBoard::kBitMaskR, BitBoard::kBitMaskU);
const __m256i BitBoard::kBitMaskRD = _mm256_and_si256(BitBoard::kBitMaskR, BitBoard::kBitMaskD);
const __m256i BitBoard::kBitMaskLD = _mm256_and_si256(BitBoard::kBitMaskL, BitBoard::kBitMaskD);
const __m256i BitBoard::kBitMaskLU = _mm256_and_si256(BitBoard::kBitMaskL, BitBoard::kBitMaskU);
const __m256i BitBoard::BoardFill = _mm256_set_epi16(
	0x0000u, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu,
	0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu, 0x7FFFu);
/**
* ~japanese	@brief kPositionArrayを初期化する
* ~english	@brief Initialize of kPositionArray
*/
static auto init_BitBoard_PositionArray() noexcept {
	std::array<BitBoard, kAllBoardSize> re;
	for(auto y : rep(kBoardSize)) {
		for(auto x : rep(kBoardSize)) {
			size_t position = ToPosition(x, y);
			uint16_t line[sizeof(__m256i) / sizeof(uint16_t)]{};
			line[y] = 1U << x;
			re[position] = _mm256_loadu_si256((__m256i*)line);
		}
	}
	return re;
}
const std::array<BitBoard, kAllBoardSize> BitBoard::kPositionArray = init_BitBoard_PositionArray();

static auto init_BitBoard_BitMaskArray() noexcept {
	array<array<array<BitBoard, kMaxShifts>, Side::Sides>, Direction::Directions> re;
	re[Direction::Row][Side::Left][0]     = BitBoard::kBitMaskL;
	re[Direction::Column][Side::Left][0]  = BitBoard::kBitMaskU;
	re[Direction::DiagR][Side::Left][0]   = BitBoard::kBitMaskRU;
	re[Direction::DiagL][Side::Left][0]   = BitBoard::kBitMaskLU;
	re[Direction::Row][Side::Right][0]    = BitBoard::kBitMaskR;
	re[Direction::Column][Side::Right][0] = BitBoard::kBitMaskD;
	re[Direction::DiagR][Side::Right][0]  = BitBoard::kBitMaskLD;
	re[Direction::DiagL][Side::Right][0]  = BitBoard::kBitMaskRD;
	for(auto dir : rep(Direction::Directions)) {
		auto& d = re[dir];
		for(auto shift : rep(kMaxShifts - 1)) d[Side::Left][shift + 1] = d[Side::Left][shift] >> dir;
		for(auto shift : rep(kMaxShifts - 1)) d[Side::Right][shift + 1] = d[Side::Right][shift] << dir;
	}
	return re;
}
const array<array<array<BitBoard, kMaxShifts>, Side::Sides>, Direction::Directions> BitBoard::kBitMaskArray = init_BitBoard_BitMaskArray();

/**
* @brief Constructor
*/

BitBoard::BitBoard(const __m256i a) noexcept {
	board_ = a;
}

void BitBoard::PutBoard() const noexcept {
	using std::cout;
	using std::endl;
	for(const auto y : rep(kBoardSize)) {
		for(const auto x : rep(kBoardSize)) {
			auto position = ToPosition(x, y);
			if ((*this)[position]) {
				cout << "□";
			}
			else {
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

BitBoard BitBoard::operator<<(const Direction dir) const noexcept {
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

BitBoard BitBoard::operator>>(const Direction dir) const noexcept {
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
