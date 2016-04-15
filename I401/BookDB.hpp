#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "constant.hpp"
#include "types.hpp"
#include "misc_functions.hpp"
// using declaration
using std::cout;
using std::endl;
using std::string;
using std::tuple;
using std::vector;
// Book class
class BookDB {
	std::unordered_map<std::uint64_t, std::unordered_set<size_t>> book_;
public:
	BookDB() {}
	BookDB(const char *book_name) {
		std::ifstream ifs(book_name);
		if (ifs.fail()) throw std::exception("Book.csv not found.");
		string str;
		while (getline(ifs, str)) {
			// Read CSV data
			string token;
			std::istringstream stream(str);
			vector<size_t> move;
			while (getline(stream, token, ',')) {
				move.push_back(stoi(token));
			}
			// Set BookData
			SetBookData(move);
			move = RotateMove(move);
			SetBookData(move);
			move = RotateMove(move);
			SetBookData(move);
			move = RotateMove(move);
			SetBookData(move);
			move = RotateMove(move);

			move = FlipMove(move);
			SetBookData(move);
			move = RotateMove(move);
			SetBookData(move);
			move = RotateMove(move);
			SetBookData(move);
			move = RotateMove(move);
			SetBookData(move);
		}
		/*for (auto &it : book_[8202465179494830182]) {
		cout << PositionToString(it) << endl;
		}*/
		return;
	}
	void PutMove(vector<size_t> &move) {
		for (size_t i = 0; i < move.size(); ++i) {
			cout << PositionToString(move[i]) << ",";
		}
		cout << endl;
	}
	vector<size_t> RotateMove(vector<size_t> &move) {
		vector<size_t> temp_move;
		for (auto &it : move) {
			size_t x = it % kBoardSize, y = it / kBoardSize;
			int x2 = x - 7, y2 = y - 7;
			temp_move.push_back(ToPosition(7 + y2, 7 - x2));
		}
		return temp_move;
	}
	vector<size_t> FlipMove(vector<size_t> &move) {
		vector<size_t> temp_move;
		for (auto &it : move) {
			size_t x = it % kBoardSize, y = it / kBoardSize;
			int x2 = x - 7, y2 = y - 7;
			temp_move.push_back(ToPosition(7 + x2, 7 - y2));
		}
		return temp_move;
	}
	void SetBookData(const vector<size_t> &move) {
		// Move on temporary board
		BaseBoard board;
		std::fill(board.begin(), board.end(), Stone::None);
		auto turn = Stone::Black;
		for (size_t i = 0; i < move.size() - 1; ++i) {
			board[move[i]] = turn;
			turn = EnemyTurn(turn);
		}
		// Set Data
		/*if (move[0] == 112 && move[1] == 97 && move[2] == 98 && move[3] == 113 && move[4] == 129 && move.size() == 5) {
		for (size_t p = 0; p < kBoardSize * kBoardSize; ++p) {
		if (board[p] != Stone::None) {
		cout << PositionToString(p) << " " << board[p] << endl;
		}
		}
		auto hash2 = GetHash(board);
		return;
		}*/
		auto hash = GetHash(board);
		if (book_.count(hash) == 0) {
			book_[hash] = std::unordered_set<size_t>();
		}
		book_[hash].insert(move[move.size() - 1]);
	}
	vector<size_t> GetBookData(const BaseBoard &board) {
		vector<size_t> result;
		uint64_t hash = GetHash(board);
		if (book_.count(hash) == 0) return result;
		return vector<size_t>(book_[hash].begin(), book_[hash].end());
	}
	static uint64_t GetHash(const BaseBoard &board) {
		uint64_t hash = 0, pow = 1;
		for (size_t p = 0; p < kBoardSize * kBoardSize; ++p) {
			hash += board[p] * pow;
			pow *= 17;
		}
		return hash;
	}
};
