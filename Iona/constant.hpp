﻿/**
* @file constant.cpp
* ~japanese	@brief 各種定数・マクロを記述する
* ~english	@brief Definition of constant values
* @author YSR
* @date 2016/04/26
*/
#pragma once
#include <cstddef>
using std::size_t;

//! basic marcos(FOR)
#define FOR(i, a, b) for(size_t i = (a); i < (b); ++i)
//! basic marcos(REP)
#define REP(i, n)    FOR(i, 0, n)

//! board size of Renju(15x15)
constexpr size_t kBoardSize = 15;
//! all board size of Renju(255=15x15)
constexpr size_t kAllBoardSize = kBoardSize * kBoardSize;
//! maximum pattern shifts
constexpr size_t kMaxShifts = 5;
