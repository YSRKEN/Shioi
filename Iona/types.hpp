/**
* @file types.hpp
* ~japanese	@brief 各種型を定義する
* ~english	@brief Definition of types
* @author YSR
* @date 2016/04/26
*/
#pragma once

/**
* @enum Turn
* Information of turn player
*/
enum Stone : uint8_t {
	Black,
	White,
	None,
	Stones = 2,
};

/**
* @enum Side
* Information of side
*/
enum Side : uint8_t {
	Left,
	Right,
	Sides,
};

/**
* @enum Direction
* Information of direction
*/
enum Direction : uint8_t {
	//! Row(─) R[1, 0] L[-1, 0]
	Row,
	//! Column(│) R[0, 1] L[0, -1]
	Column,
	//! Diagonally right(／) R[-1, 1] L[1, -1]
	DiagR,
	//! Diagonally left(＼) R[1, 1] L[-1, -1]
	DiagL,
	Directions,
};
