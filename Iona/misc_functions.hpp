/**
* @file misc_functions.hpp
* ~japanese	@brief ゲームで使用する関数を定義する
* ~english	@brief Definition of misc functions
* @author YSR
* @date 2016/04/26
*/
#pragma once
#include"constant.hpp"
#include<type_traits>
template<bool con> using enable_if_t = typename std::enable_if<con, std::nullptr_t>::type;
/**
* @fn ToPosition
* @brief Convert two position one position
*/
constexpr size_t ToPosition(const size_t x, const size_t y){
	return x + y * kBoardSize;
}
