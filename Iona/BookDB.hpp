/**
* @file BookDB.hpp
* ~japanese	@brief ゲームで使用する定石データを定義する
* ~english	@brief Definition of Book data for Renju game
* @author YSR
* @date 2016/05/03
*/
#pragma once
#include<cstdint>
#include<fstream>
#include<iostream>
#include<sstream>
#include<string>
#include<vector>
#include<unordered_set>
#include<unordered_map>
#include"BitBoard.hpp"
#include"misc_functions.hpp"

using std::string;
using std::vector;

/**
* @brief Book data class for Renju game
*/
class BookDB {
	std::unordered_map<std::uint64_t, std::unordered_set<size_t>> book_;
	static vector<size_t> RotateMove(const vector<size_t> &move) {
		vector<size_t> temp_move;
		for (auto &it : move) {
			size_t x = it % kBoardSize, y = it / kBoardSize;
			int x2 = x - 7, y2 = y - 7;
			temp_move.push_back(ToPosition(7 + y2, 7 - x2));
		}
		return temp_move;
	}
	static vector<size_t> FlipMove(const vector<size_t> &move) {
		vector<size_t> temp_move;
		for (auto &it : move) {
			size_t x = it % kBoardSize, y = it / kBoardSize;
			int x2 = x - 7, y2 = y - 7;
			temp_move.push_back(ToPosition(7 + x2, 7 - y2));
		}
		return temp_move;
	}
	void SetBookData(const vector<size_t> &move) {
		//! Move on temporary board
		BitBoard black_board, white_board;
		for (size_t i = 0; i < move.size() - 1; ++i) {
			if (i % 2 == 0) {
				black_board |= kPositionArray[move[i]];
			}else{
				white_board |= kPositionArray[move[i]];
			}
		}
		auto hash = GetHash(black_board, white_board);
		if (book_.count(hash) == 0) {
			book_[hash] = std::unordered_set<size_t>();
		}
		book_[hash].insert(move[move.size() - 1]);
	}
	static uint64_t GetHash(const BitBoard &black_board, const BitBoard &white_board) noexcept{
		uint64_t hash = 0;
		const size_t splits = sizeof(BitBoard) / sizeof(uint64_t);
		uint64_t *temp = (uint64_t*)_mm_malloc(sizeof(uint64_t) * splits * 2, 32);
		_mm256_store_si256((__m256i*)temp, black_board.board_);
		_mm256_store_si256((__m256i*)(temp + splits), white_board.board_);
		uint64_t multi = 1;
		REP(i, splits * 2) {
			hash += multi * temp[i];
			multi *= 17;
		}
		_mm_free(temp);
		return hash;
	}
public:
	/**
	* @brief Constructor
	* @param book_name file-path of book.csv
	* @detail Reset data of book for Renju game
	*/
	BookDB(const char *book_name) {
		std::ifstream ifs(book_name);
		if (ifs.fail()) throw std::runtime_error("Book.csv not found.");
		string str;
		while (getline(ifs, str)) {
			//! Read CSV data
			string token;
			std::istringstream stream(str);
			vector<size_t> move;
			while (getline(stream, token, ',')) {
				move.push_back(stoi(token));
			}
			//! Set BookData
			SetBookData(move);
			move = RotateMove(move);
			SetBookData(move);
			move = RotateMove(move);
			SetBookData(move);
			move = RotateMove(move);
			SetBookData(move);
			move = RotateMove(move);
			//!
			move = FlipMove(move);
			SetBookData(move);
			move = RotateMove(move);
			SetBookData(move);
			move = RotateMove(move);
			SetBookData(move);
			move = RotateMove(move);
			SetBookData(move);
		}
		return;
	}
	vector<size_t> GetBookData(const BitBoard &black_board, const BitBoard &white_board) const {
		auto hash = GetHash(black_board, white_board);
		if (book_.count(hash) == 0) return vector<size_t>{};
		return vector<size_t>(book_.at(hash).begin(), book_.at(hash).end());
	}
};
