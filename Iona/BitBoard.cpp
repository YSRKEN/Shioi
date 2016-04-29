#include "BitBoard.hpp"

const __m256i BitBoard::kBitMaskR = _mm256_set1_epi16(0x7FFFu);
const __m256i BitBoard::kBitMaskL = _mm256_set1_epi16(0xFFFEu);
const __m256i BitBoard::kBitMaskU = _mm256_set_epi16(
	0x0000u, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu,
	0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu);
const __m256i BitBoard::kBitMaskD = _mm256_set_epi16(
	0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu,
	0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0x0000u);
const __m256i BitBoard::kBitMaskRU = _mm256_and_si256(BitBoard::kBitMaskR, BitBoard::kBitMaskU);
const __m256i BitBoard::kBitMaskRD = _mm256_and_si256(BitBoard::kBitMaskR, BitBoard::kBitMaskD);
const __m256i BitBoard::kBitMaskLD = _mm256_and_si256(BitBoard::kBitMaskL, BitBoard::kBitMaskD);
const __m256i BitBoard::kBitMaskLU = _mm256_and_si256(BitBoard::kBitMaskL, BitBoard::kBitMaskU);
const __m256i BitBoard::AllBit1 = _mm256_set1_epi16(0xFFFFu);

/**
* ~japanese	@brief kPositionArrayを初期化する
* ~english	@brief Initialize of kPositionArray
*/
static auto init_BitBoard_PositionArray() {
	std::array<__m256i, kAllBoardSize> re;
	REP(y, kBoardSize) {
		REP(x, kBoardSize) {
			size_t position = ToPosition(x, y);
			uint16_t line[sizeof(__m256i) / sizeof(uint16_t)]{};
			line[y] = 1U << x;
			re[position] = _mm256_loadu_si256((__m256i*)line);
		}
	}
	return re;
}
const std::array<__m256i, kAllBoardSize> BitBoard::kPositionArray = init_BitBoard_PositionArray();

/**
* @brief Constructor
*/

BitBoard::BitBoard(const __m256i a) noexcept {
	board_ = a;
}

/**
* ~japanese	@brief BitBoardを、dir方向における「左」にシフトする
* ~english	@brief shift BitBoard to "Left" direction
*/
BitBoard BitBoard::operator<<(const Direction dir) const noexcept {
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
			return BitBoard(_mm256_alignr_epi8(_mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(2, 0, 0, 1)), board_, 2) & kBitMaskU);
			break;
		case Direction::DiagR:
			//! 最初考えていた方法
			/*{alignas(32) uint16_t temp[17]{};
			_mm256_store_si256((__m256i*)temp, board_);
			return BitBoard(_mm256_slli_epi16(_mm256_loadu_si256((__m256i*)(temp + 1)), 1) & kBitMaskLU);}*/
			//! 恐るべき解決手段
			return BitBoard(_mm256_slli_epi16(_mm256_alignr_epi8(_mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(2, 0, 0, 1)), board_, 2), 1) & kBitMaskLU);
			break;
		case Direction::DiagL:
			//! 最初考えていた方法
			/*{alignas(32) uint16_t temp[17]{};
			_mm256_store_si256((__m256i*)temp, board_);
			return BitBoard(_mm256_srli_epi16(_mm256_loadu_si256((__m256i*)(temp + 1)), 1) & kBitMaskRU);}*/
			//! 恐るべき解決手段
			return BitBoard(_mm256_srli_epi16(_mm256_alignr_epi8(_mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(2, 0, 0, 1)), board_, 2), 1) & kBitMaskRU);
			break;
		default:
			return *this;
	}
}

/**
* ~japanese	@brief BitBoardを、dir方向における「右」にシフトする
* ~english	@brief shift BitBoard to "Right" direction
*/
BitBoard BitBoard::operator>>(const Direction dir) const noexcept {
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
			return BitBoard(_mm256_alignr_epi8(board_, _mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(0, 0, 2, 0)), 16 - 2) & kBitMaskD);
		case Direction::DiagR:
			//! 最初考えていた方法
			/*{alignas(32) uint16_t temp[17]{};
			_mm256_storeu_si256((__m256i*)(temp + 1), board_);
			return BitBoard(_mm256_srli_epi16(_mm256_load_si256((__m256i*)temp), 1) & kBitMaskRD);}*/
			//! 恐るべき解決手段
			return BitBoard(_mm256_srli_epi16(_mm256_alignr_epi8(board_, _mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(0, 0, 2, 0)), 16 - 2), 1) & kBitMaskRD);
			break;
		case Direction::DiagL:
			//! 最初考えていた方法
			/*{alignas(32) uint16_t temp[17]{};
			_mm256_storeu_si256((__m256i*)(temp + 1), board_);
			return BitBoard(_mm256_slli_epi16(_mm256_load_si256((__m256i*)temp), 1) & kBitMaskLD);}*/
			//! 恐るべき解決手段
			return BitBoard(_mm256_slli_epi16(_mm256_alignr_epi8(board_, _mm256_permute2x128_si256(board_, board_, _MM_SHUFFLE(0, 0, 2, 0)), 16 - 2), 1) & kBitMaskLD);
			break;
		default:
			return *this;
	}
}
