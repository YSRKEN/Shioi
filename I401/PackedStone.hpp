﻿#pragma once
#include <cstddef>
#include <cstdint>
#include "types.hpp"
//基本方針：packできない時は変更を加えない
/**
\~japanese	@brief Stoneを省メモリに格納するクラスです
\~english	@brief Class for store Stone
\~japanese	@detail 格納できる要素数は32個です。64bitを2bitずつ使用します。この構造の実現のためにbit演算を多用します。
\~english	@detail The capacity of this class is 32(2 bit / elem, total 64bit).
\~
xx xx xx xx ... xx xx xx xx
MSB <---           ---> LSB : 64bit
front <-           --> back : 32 elem

ex.)
PackedStone{ Stone::Black, Stone::Black, Stone::White, Stone::None }
=> 00 00 ... 00 01 01 10 00
*/
class PackedStone
{
public:
	typedef std::size_t size_type;
	typedef std::uint64_t hold_type;
	/**
	\~japanese	@brief 一要素あたりの消費bit数
	\~english	@brief number of bit-consumption per 1 element.
	*/
	static constexpr std::uint8_t bit = 2;
private:
	/**
	\~japanese	@brief データ
	\~english	@brief data
	*/
	hold_type value_;
	/**
	\~japanese	@brief 要素数
	\~english	@brief Element count
	*/
	size_type size_;
public:
	/**
	\~japanese	@brief 容量
	\~english	@brief capacity
	*/
	static constexpr size_type cap = sizeof(hold_type) * CHAR_BIT / bit;
	constexpr PackedStone() noexcept : value_(), size_() {}
	/**
	\~japanese	@brief Stone型を1つ格納します
	\~english	@brief holds a single Stone object
	*/
	constexpr PackedStone(Stone s) noexcept : value_(s), size_(1) {}
	/**
	\~japanese	@brief Stone型を2つ格納します
	\~english	@brief holds two Stone object
	*/
	constexpr PackedStone(Stone s1, Stone s2) noexcept : value_((static_cast<hold_type>(s1) << bit) + s2), size_(2) {}

	constexpr PackedStone(const PackedStone& o) noexcept : value_(o.value_), size_(o.size_) {}
	PackedStone(PackedStone&& o) = delete;
	PackedStone& operator=(const PackedStone& o) noexcept {
		this->value_ = o.value_;
		this->size_  = o.size_;
		return *this;
	}
	PackedStone& operator=(PackedStone&& o) = delete;
	/**
	\~japanese	@brief 第一引数のPackedStoneの後ろに新たにStoneを追加して格納します。格納できない場合は第一引数のPackedStoneをそのまま格納します
	\~english	@brief Store first argument and push back second argument. When it is failed, Store first argument.
	*/
	constexpr PackedStone(const PackedStone& o, const Stone s) noexcept
		: value_((o.is_packable()) ? o.value_ : (o.value_ << bit) + s), size_((o.is_packable()) ? o.size_ : o.size_ + 1) {}
	/**
	\~japanese	@brief 第一引数のPackedStoneの前に新たにStoneを追加して格納します。格納できない場合は第一引数のPackedStoneをそのまま格納します
	\~english	@brief Store first argument and push front second argument. When it is failed, Store first argument.
	*/
	constexpr PackedStone(const Stone s, const PackedStone& o) noexcept
		: value_((o.is_packable()) ? s : (static_cast<hold_type>(s) << bit) + o.value_), size_((o.is_packable()) ? 1 : o.size_ + 1) {}
	/**
	\~japanese	@brief 容量を取得します。
	\~english	@brief Get capacity
	*/
	constexpr size_type capacity() const noexcept { return cap; }
	/**
	\~japanese	@brief 現在の大きさを取得します。
	\~english	@brief Get current size
	*/
	constexpr size_type size() const noexcept { return size_; }
	/**
	\~japanese	@brief 内部データを直接取り出します。
	\~english	@brief Get raw data
	*/
	constexpr hold_type data() const noexcept { return value_; }
	/**
	\~japanese	@brief Stoneをさらに格納できるか調べます
	\~english	@brief Check 
	*/
	//@brief 
	constexpr bool is_packable() const noexcept { return size_ < cap; }
private:
	/**
	\~japanese	@brief 要素だけをand演算で取り出すためのmask
	\~english	@brief a mask for and operation to get element
	*/
	static constexpr hold_type mask = 0b11;
	/**
	\~japanese	@brief 要素bitを先頭に移動させるための左シフトの数を計算する
	\~english	@brief calc how many bit is needed to shift elem-bit to front-elem-bits
	*/
	constexpr std::uint8_t lshift_num_to_get_front() const noexcept { return static_cast<std::uint8_t>(this->size_ - 1); };
public:
	/**
	\~japanese	@brief Getter: 先頭要素を取得する
	\~english	@brief Getter: Get first elem
	*/
	constexpr Stone front() const noexcept {
		return static_cast<Stone>(this->value_ >> lshift_num_to_get_front());
	}
	/**
	\~japanese	@brief Setter: 先頭要素に書き込む
	\~english	@brief Setter: Set first elem
	\~japanese	@return 変更されたPackedStoneクラス
	\~english	@return modified PackedStone class.
	*/
	constexpr PackedStone front(Stone s) const noexcept { 
		//                      ************* bit mask *************
		return{ (this->value_ & ~(mask << lshift_num_to_get_front())) | (static_cast<hold_type>(s) << lshift_num_to_get_front()), this->size_ };
	}
	/**
	\~japanese	@brief Getter: 末尾要素を取得する
	\~english	@brief Getter: Get last elem
	*/
	constexpr Stone back() const noexcept {
		return static_cast<Stone>(this->value_ & mask);
	}
	/**
	\~japanese	@brief Setter: 末尾要素に書き込む
	\~english	@brief Setter: Set last elem
	\~japanese	@return 変更されたPackedStoneクラス
	\~english	@return modified PackedStone class.
	*/
	constexpr PackedStone back(Stone s) const noexcept { 
		return{ (this->value_ & ~mask) | s, this->size_ };
	}
	constexpr PackedStone push_front(Stone s) const noexcept { return{ s, *this }; }
	constexpr PackedStone push_back(Stone s) const noexcept { return{ *this, s }; }
	constexpr PackedStone pop_front() const noexcept {
		return (this->size_) ? PackedStone{ this->value_ & (mask << lshift_num_to_get_front()), this->size_ - 1 } : *this;
	}
	constexpr PackedStone pop_back() const noexcept { return (this->size_) ? PackedStone{ this->value_ >> bit, this->size_ - 1 } : *this; }
private:
	constexpr PackedStone(hold_type n, size_type s) noexcept : value_(n), size_(s) {}
	constexpr PackedStone operator<<(std::size_t n) const noexcept { return (is_packable()) ? PackedStone{ value_ << (2 * n), size_ } : *this; }
public:
	constexpr PackedStone operator<<(std::uint8_t n) const noexcept { return *this << n; }
	constexpr PackedStone operator|(PackedStone r) const noexcept {
		return (cap < this->size_ + r.size_) ? *this : PackedStone{(*this << r.size()).value_ + r.value_, this->size_ + r.size_};
	}
	constexpr PackedStone operator|(Stone r) const noexcept {
		return{ *this, r };
	}
};
constexpr bool operator==(PackedStone l, PackedStone r) noexcept {
	return l.size() == r.size() && l.data() == r.data();
}
constexpr bool operator!=(PackedStone l, PackedStone r) noexcept {
	return !(l == r);
}
constexpr PackedStone operator|(Stone l, Stone r) noexcept { return{ l, r }; }
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


typedef array<array<PackedStone, 5>, 2> Pattern;

