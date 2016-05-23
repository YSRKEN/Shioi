/**
* @file misc_functions.hpp
* ~japanese	@brief ゲームで使用する関数を定義する
* ~english	@brief Definition of misc functions
* @author YSR
* @date 2016/04/26
*/
#pragma once
#include <random>
#include <cstring>
#include"constant.hpp"

//! Definition of random_device
extern std::random_device rd;
//! Definition of pseudo-random generator
extern std::mt19937 mt;

/**
* @brief Convert two position one position
*/
constexpr size_t ToPosition(const size_t x, const size_t y) noexcept {
	return x + y * kBoardSize;
}

/**
* @brief Reverse turn
*/
constexpr Stone EnemyTurn(const Stone turn) noexcept {
	return (turn == Stone::Black ? Stone::White : Stone::Black);
}

/**
* @return Integer random number in [0, n)
*/
inline size_t RandInt(const size_t n) {
	return std::uniform_int_distribution<size_t>{0, n - 1}(mt);
}

/**
* @brief Convert char to Stone
* @param str /[BWN]/
* @return 0-2[Stone::Black, Stone::White, Stone::None]
* ~japanese	@detail strがkStoneStringのどれにも当てはまらない場合は例外を投げる
* ~english	@detail If you can't find str in kStoneString, this throw exception.
*/
inline Stone ToStone(const char str) {
	constexpr static char kStoneString[] = "BWN";
	auto find_ptr = std::strchr(kStoneString, str);
	if (find_ptr != NULL) {
		return static_cast<Stone>(find_ptr - kStoneString);
	}
	else {
		throw std::invalid_argument("Can't read board/turn data.");
	}
}
