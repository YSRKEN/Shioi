#pragma once
#include <cstddef>
using std::size_t;
constexpr const char* kPositionStringX = "abcdefghijklmno";
constexpr const char* kPositionStringY = "123456789ABCDEF";
constexpr size_t kBoardSize = 15;
constexpr size_t kSearchWidth = 5;
constexpr size_t kShioiDepth1 = 20, kShioiDepth2 = 3;
constexpr int kScoreInf = 1000;
constexpr int kScoreInf2 = kScoreInf + 1;
constexpr size_t kPositionoffset[] = { 1, kBoardSize, kBoardSize - 1, kBoardSize + 1 };
