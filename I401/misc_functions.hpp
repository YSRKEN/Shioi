﻿#pragma once
#include "types.hpp"
#include "constant.hpp"
#include <array>
using std::array;
using std::size_t;
constexpr inline size_t ToPosition(const size_t x, const size_t y) {
	return x + y * kBoardSize;
}
constexpr inline Stone EnemyTurn(const Stone turn) {
	return (turn == Stone::Black ? Stone::White : Stone::Black);
}
namespace detail {
	struct GetBoardValue_helper {
		size_t pos;
		Direction direction;
		int count;
	};
	constexpr Stone operator|(const std::array<Stone, kBoardSize * kBoardSize>& board, const GetBoardValue_helper& info) {
		return board[info.pos + kPositionoffset[info.direction] * info.count];
	}
	struct StoneNormalizer_helper {};
	constexpr Stone operator| (Stone value, StoneNormalizer_helper) {
		return (value != Stone::White) ? value : Stone::None;
	}
	struct PackPattern_helper {
		int start;
		int stop;
		size_t pos;
		Direction direction;
	};
	constexpr GetBoardValue_helper Get(size_t position, Direction dir, int count) {
		return{ position, dir, count };
	}
	constexpr StoneNormalizer_helper Normalize() { return{}; }
	size_t operator| (const array<Stone, kBoardSize * kBoardSize>& board, const PackPattern_helper& info) {
		using std::abs;
		if (abs(info.stop) < abs(info.start)) return 0;
		size_t re = 0;
		int i;
		for (i = info.start; abs(i) <= abs(info.stop) - 1; i += (i > 0) ? 1 : -1, re <<= 2U) re += board | Get(info.pos, info.direction, i);
		re += board | Get(info.pos, info.direction, i) | Normalize();
		return re;
	}
}
using detail::Get;
using detail::Normalize;
constexpr detail::PackPattern_helper PackPattern(size_t position, Direction dir, int start, int stop) {
	return{ start, stop, position, dir };
}
constexpr size_t operator|(Stone l, Stone r) { return (l << 2) + r; }
constexpr size_t operator|(size_t l, Stone r) { return (l << 2) + r; }
namespace detail {
	constexpr size_t PackPattern_n_impl(const size_t tmp, const Stone s, const size_t rest_count) {
		return (rest_count - 1) ? PackPattern_n_impl((tmp << 2) + s, s, rest_count - 1) : (tmp << 2) + s;
	}
}
constexpr size_t PackPattern_n(const Stone s, size_t n) {
	return (n * 2 > sizeof(size_t)) ? std::numeric_limits<size_t>::max() : detail::PackPattern_n_impl(0U, s, n);
}
namespace detail {
	template<typename T>struct limit_helper {
		T min, max;
	};
	template<typename T>struct limit_helper2 {
		T min, max;
	};
	template<typename Result, typename T, std::enable_if_t<std::is_arithmetic_v<Result> && std::is_arithmetic_v<T>, std::nullptr_t> = nullptr>
	constexpr Result operator|(const Result& r, const limit_helper<T>& info) {
		return (info.min <= r) ? (r <= info.max) ? r : info.max : info.min;
	}
	template<typename Result, typename T, std::enable_if_t<std::is_arithmetic_v<Result> && std::is_arithmetic_v<T>, std::nullptr_t> = nullptr>
	constexpr Result operator|(const Result& r, const limit_helper2<T>& info) {
		return (info.min <= r && r < info.max) ? r : info.max;
	}
}
template<typename T> constexpr detail::limit_helper<T> limit(const T& min, const T& max) { return{ min, max }; }
template<typename T> constexpr detail::limit_helper2<T> limit2(const T& min, const T& max) { return{ min, max }; }
