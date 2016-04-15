#pragma once
#include "types.hpp"
#include "constant.hpp"
#include <array>
#include <string>
#include <cerrno>
#include <stdexcept>
using std::array;
using std::size_t;
std::string PositionToString(const size_t p) {
	return std::string(1, kPositionStringX[p % kBoardSize]) + std::string(1, kPositionStringY[p / kBoardSize]);
}
constexpr size_t ToPosition(const size_t x, const size_t y) {
	return x + y * kBoardSize;
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
namespace detail {
	constexpr size_t PackPattern_n_impl(const size_t tmp, const Stone s, const size_t rest_count) {
		return (rest_count - 1) ? PackPattern_n_impl((tmp << 2) + s, s, rest_count - 1) : (tmp << 2) + s;
	}
}
constexpr size_t PackPattern_n(const Stone s, size_t n) {
	return (n * 2 > sizeof(size_t) * CHAR_BIT) ? std::numeric_limits<size_t>::max() : detail::PackPattern_n_impl(0U, s, n);
}
namespace detail {
	struct PackPattern_n_operator_helper {
		struct Impl {
			size_t n;
		};
		Impl p;
		constexpr Impl operator*() {
			return p;
		}
	};
	constexpr size_t operator*(const Stone s, PackPattern_n_operator_helper::Impl n) {
		return PackPattern_n(s, n.n);
	}
}
constexpr detail::PackPattern_n_operator_helper operator "" _pack(unsigned long long n) { return{ n }; }
namespace detail {
	template<typename T>struct limit_helper {
		T min, max;
	};
	template<typename T>struct limit_helper2 {
		T min, max;
	};
	template<typename Result, typename T, std::enable_if_t<std::is_arithmetic<Result>::value && std::is_arithmetic<T>::value, std::nullptr_t> = nullptr>
	constexpr Result operator|(const Result& r, const limit_helper<T>& info) {
		return (info.min <= r) ? (r <= info.max) ? r : info.max : info.min;
	}
	template<typename Result, typename T, std::enable_if_t<std::is_arithmetic<Result>::value && std::is_arithmetic<T>::value, std::nullptr_t> = nullptr>
	constexpr Result operator|(const Result& r, const limit_helper2<T>& info) {
		return (info.min <= r && r < info.max) ? r : info.max;
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
	struct to_i_helper {};
	template<typename CharType>
	int operator|(const std::basic_string<CharType>& s, to_i_helper) { return std::stoi(s); }
	int operator|(const char* s, to_i_helper) { 
		errno = 0;
		const auto r = std::strtol(s, nullptr, 10);
		if (0 != errno) throw std::out_of_range("");
		static_assert(sizeof(int) == sizeof(long), "check function int operator|(const char*, to_i_helper)");
		return static_cast<int>(r);
	}
	int operator|(const wchar_t* s, to_i_helper) {
		errno = 0;
		const auto r = std::wcstol(s, nullptr, 10);
		if (0 != errno) throw std::out_of_range("");
		static_assert(sizeof(int) == sizeof(long), "check function int operator|(const char*, to_i_helper)");
		return static_cast<int>(r);
	}
}
constexpr detail::to_i_helper to_i() { return{}; }
namespace min_max_impl {
	using std::min;
	template<typename T>
	constexpr T& min(const T& a1, const T& a2, const T& a3) {
		return std::min(std::min(a1, a2), a3);
	}
	template<typename T, typename ...Rest>
	constexpr T& min(const T& a1, const T& a2, const Rest&... args) {
		return min(std::min(a1, a2), args...);
	}
	using std::max;
	template<typename T>
	constexpr T& max(const T& a1, const T& a2, const T& a3) {
		return std::max(std::max(a1, a2), a3);
	}
	template<typename T, typename ...Rest>
	constexpr T& max(const T& a1, const T& a2, const Rest&... args) {
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
