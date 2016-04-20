#pragma once
#include "types.hpp"
#include "constant.hpp"
#include <array>
#include <string>
#include <cstring>
#include <cerrno>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include "PackedStone.hpp"
using std::array;
using std::size_t;
using std::string;
string PositionToString(const size_t p) {
	return std::string(1, kPositionStringX[p % kBoardSize]) + std::string(1, kPositionStringY[p / kBoardSize]);
}
constexpr size_t ToPosition(const size_t x, const size_t y) {
	return x + y * kBoardSize;
}
size_t StringToPosition(const string str) {
	const auto kPSX = string(kPositionStringX);
	const auto kPSY = string(kPositionStringY);
	auto x = kPSX.find(str[0]);
	auto y = kPSY.find(str[1]);
	return ToPosition(x, y);
}
constexpr Stone EnemyTurn(const Stone turn) {
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
	constexpr PackedStone operator| (PackedStone value, StoneNormalizer_helper) {
		return (value.back() != Stone::White) ? value : value.back(Stone::None);//pickup 2bit and compare.
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
	PackedStone operator| (const array<Stone, kBoardSize * kBoardSize>& board, const PackPattern_helper& info) {
		using std::abs;
		if (abs(info.stop) < abs(info.start)) return{};
		PackedStone re{};
		for (int i = info.start; abs(i) <= abs(info.stop); i += (i > 0) ? 1 : -1) {
	//		auto hoge = (board | Get(info.pos, info.direction, i));
			re = re | (board | Get(info.pos, info.direction, i));
		}
		return re;
	}
}
using detail::Get;
using detail::Normalize;
constexpr detail::PackPattern_helper PackPattern(size_t position, Direction dir, int start, int stop) {
	return{ start, stop, position, dir };
}
namespace detail {
	template<typename T>struct limit_helper {
		T min, max;
	};
	template<typename T>struct limit_helper2 {
		T min, max;
	};
	template<typename Left, typename Right, bool all_arithmetic, bool left_is_sign, bool right_is_sign> struct less_or_equal_helper {
		Left l;
		Right r;
		constexpr bool operator()() const {
			return l <= r;
		}
	};
	template<typename Left, typename Right> struct less_or_equal_helper<Left, Right, true, false, true> {
		Left l;//unsigned
		Right r;//signed
		constexpr bool operator()() const {
			return (r < 0) ? false : l <= static_cast<std::make_unsigned_t<Right>>(r);
		}
	};
	template<typename Left, typename Right> struct less_or_equal_helper<Left, Right, true, true, false> {
		Left l;//signed
		Right r;//unsigned
		constexpr bool operator()() const {
			return (l < 0) ? true : static_cast<std::make_unsigned_t<Right>>(l) <= r;
		}
	};
	template<typename Left, typename Right>
	constexpr less_or_equal_helper<
		Left, Right, 
		std::is_arithmetic<Left>::value && std::is_arithmetic<Right>::value, 
		std::is_signed<Left>::value, std::is_signed<Right>::value
	> less_or_equal(const Left l, const Right r) { return{l, r}; }
	template<typename Left, typename Right, bool all_arithmetic, bool left_is_sign, bool right_is_sign> struct less_helper {
		Left l;
		Right r;
		constexpr bool operator()() const {
			return l < r;
		}
	};
	template<typename Left, typename Right> struct less_helper<Left, Right, true, false, true> {
		Left l;//unsigned
		Right r;//signed
		constexpr bool operator()() const {
			return (r <= 0) ? false : l < static_cast<std::make_unsigned_t<Right>>(r);
		}
	};
	template<typename Left, typename Right> struct less_helper<Left, Right, true, true, false> {
		Left l;//signed
		Right r;//unsigned
		constexpr bool operator()() const {
			return (l <= 0) ? true : static_cast<std::make_unsigned_t<Right>>(l) < r;
		}
	};
	template<typename Left, typename Right>
	constexpr less_helper<
		Left, Right,
		std::is_arithmetic<Left>::value && std::is_arithmetic<Right>::value,
		std::is_signed<Left>::value, std::is_signed<Right>::value
	> less(const Left l, const Right r) { return{ l, r }; }
	template<typename Result, typename T, std::enable_if_t<std::is_arithmetic<Result>::value && std::is_arithmetic<T>::value, std::nullptr_t> = nullptr>
	constexpr Result operator|(const Result& r, const limit_helper<T>& info) {
		return less_or_equal(info.min, r)() ? less_or_equal(r, info.max)() ? r : info.max : info.min;
	}
	template<typename Result, typename T, std::enable_if_t<std::is_arithmetic<Result>::value && std::is_arithmetic<T>::value, std::nullptr_t> = nullptr>
	constexpr T operator|(const Result& r, const limit_helper2<T>& info) {
		return less_or_equal(info.min, r)() && less(r, info.max)() ? r : info.max;
	}
}
template<typename T> constexpr detail::limit_helper<T> limit(const T& min, const T& max) { return{ min, max }; }
template<typename T> constexpr detail::limit_helper2<T> limit2(const T& min, const T& max) { return{ min, max }; }
namespace detail {
	template<typename T>struct min_helper { T min; };
	template<typename Result, typename T, std::enable_if_t<std::is_arithmetic<Result>::value && std::is_arithmetic<T>::value, std::nullptr_t> = nullptr>
	constexpr Result operator|(const Result& r, const min_helper<T>& info) {
		return std::min(r, info.min);
	}
}
template<typename T> constexpr detail::min_helper<T> min(const T& min) { return{ min }; }
namespace detail {
	template<typename T>struct max_helper { T max; };
	template<typename Result, typename T, std::enable_if_t<std::is_arithmetic<Result>::value && std::is_arithmetic<T>::value, std::nullptr_t> = nullptr>
	constexpr Result operator|(const Result& r, const max_helper<T>& info) {
		return std::max(r, info.max);
	}
}
template<typename T> constexpr detail::max_helper<T> max(const T& max) { return{ max }; }

namespace detail {
	template<bool int_and_long_are_same> struct range_check {
		constexpr bool operator()(long n) const noexcept {
			using lim = std::numeric_limits<int>;
			return (lim::min() <= n && n <= lim::max());
		}
	};
	template<> struct range_check<true> {
		constexpr bool operator()(long) const noexcept {
			return true;
		}
	};
	struct to_i_helper {};
	template<typename CharType>
	int operator|(const std::basic_string<CharType>& s, to_i_helper) { return std::stoi(s); }
	int operator|(const char* s, to_i_helper) { 
		errno = 0;
		const auto r = std::strtol(s, nullptr, 10);
		if (0 != errno) throw std::out_of_range("");
		if (!range_check<sizeof(int) == sizeof(long)>()(r)) throw std::out_of_range("");
		return static_cast<int>(r);
	}
	int operator|(const wchar_t* s, to_i_helper) {
		errno = 0;
		const auto r = std::wcstol(s, nullptr, 10);
		if (0 != errno) throw std::out_of_range("");
		if (!range_check<sizeof(int) == sizeof(long)>()(r)) throw std::out_of_range("");
		return static_cast<int>(r);
	}
}
constexpr detail::to_i_helper to_i() { return{}; }
namespace min_max_impl {
	using std::min;
	template<typename T>
	constexpr const T& min(const T& a1, const T& a2, const T& a3) {
		return std::min(std::min(a1, a2), a3);
	}
	template<typename T, typename ...Rest>
	constexpr const T& min(const T& a1, const T& a2, const Rest&... args) {
		return min(std::min(a1, a2), args...);
	}
	using std::max;
	template<typename T>
	constexpr const T& max(const T& a1, const T& a2, const T& a3) {
		return std::max(std::max(a1, a2), a3);
	}
	template<typename T, typename ...Rest>
	constexpr const T& max(const T& a1, const T& a2, const Rest&... args) {
		return max(std::max(a1, a2), args...);
	}
}
using min_max_impl::max;
using min_max_impl::min;
namespace detail {
	struct toStone_helper {
		const char* err_message;
	};
	Stone operator|(const char c, toStone_helper info) {
		constexpr const char kStoneString[] = "-*O";
		const auto p = std::strchr(kStoneString, c);
		if (!p) throw std::invalid_argument(info.err_message);
		return static_cast<Stone>(p - kStoneString);
	}
}
constexpr detail::toStone_helper toStone(const char* err_message) { return{ err_message }; }
template<typename T>constexpr T safe_dist(const T& a1, const T& a2) { return (a1 < a2) ? a2 - a1 : a1 - a2; }
constexpr size_t operator "" _sz(unsigned long long n) { return static_cast<size_t>(n); }
