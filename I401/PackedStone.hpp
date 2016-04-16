#pragma once
#include <cstddef>
#include <cstdint>
#include "types.hpp"
class PackedStone
{
public:
	typedef std::size_t size_type;
	typedef std::uint64_t hold_type;
private:
	hold_type value_;
	size_type size_;
public:
	static constexpr size_type cap = sizeof(hold_type) * CHAR_BIT / 2;
	constexpr PackedStone() noexcept : value_(), size_() {}
	constexpr PackedStone(Stone s) noexcept : value_(s), size_(1) {}
	constexpr PackedStone(Stone s1, Stone s2) noexcept : value_((static_cast<hold_type>(s1) << 2U) + s2), size_(2) {}
	constexpr PackedStone(const PackedStone& o) noexcept : value_(o.value_), size_(o.size_) {}
	PackedStone& operator=(const PackedStone& o) noexcept {
		this->value_ = o.value_;
		this->size_  = o.size_;
		return *this;
	}
	constexpr PackedStone(const PackedStone& o, const Stone s) noexcept
		: value_((o.is_packable()) ? o.value_ : (o.value_ << 2U) + s), size_((o.is_packable()) ? o.size_ : o.size_ + 1) {}
	constexpr PackedStone(const Stone s, const PackedStone& o) noexcept
		: value_((o.is_packable()) ? s : (static_cast<hold_type>(s) << 2U) + o.value_), size_((o.is_packable()) ? 1 : o.size_ + 1) {}
	constexpr size_type capacity() const noexcept { return cap; }
	constexpr size_type size() const noexcept { return size_; }
	constexpr hold_type data() const noexcept { return value_; }
	constexpr bool is_packable() const noexcept { return size_ + 1U < cap; }
private:
	constexpr PackedStone(hold_type n, size_type s) noexcept : value_(n), size_(s) {}
	constexpr PackedStone operator<<(std::uint8_t n) const noexcept { return (is_packable()) ? PackedStone{ value_ << (2 * n), size_ } : *this; }
public:
	constexpr PackedStone operator|(PackedStone r) const noexcept {
		return (cap < this->size_ + r.size_) ? *this : PackedStone{(*this << r.size()).value_ + r.value_, this->size_ + r.size_};
	}

};
constexpr PackedStone operator|(Stone l, Stone r) noexcept { return{ l, r }; }
constexpr PackedStone operator|(PackedStone l, Stone r) noexcept { return{ l, r }; }
constexpr PackedStone operator|(Stone l, PackedStone r) noexcept { return{ l, r }; }
namespace detail {
	constexpr PackedStone PackPattern_n_impl(const PackedStone tmp, const Stone s, const size_t rest_count) {
		return (rest_count - 1) ? PackPattern_n_impl(tmp | s, s, rest_count - 1) : tmp | s;
	}
	constexpr PackedStone PackPattern_n_impl(const Stone s, const size_t rest_count) {
		return (rest_count - 1) ? PackPattern_n_impl(s, s, rest_count - 1) : s;
	}
}
constexpr PackedStone PackPattern_n(const Stone s, size_t n) {
	return (n < PackedStone::cap) ? PackedStone{} : detail::PackPattern_n_impl(s, n);
}
namespace detail {
	struct PackPattern_n_operator_helper {
		struct Impl {
			size_t n;
		};
		Impl p;
		constexpr Impl operator*() const {
			return p;
		}
	};
	constexpr PackedStone operator*(const Stone s, PackPattern_n_operator_helper::Impl n) {
		return PackPattern_n(s, n.n);
	}
}
constexpr detail::PackPattern_n_operator_helper operator "" _pack(unsigned long long n) { return{ { static_cast<size_t>(n) } }; }
