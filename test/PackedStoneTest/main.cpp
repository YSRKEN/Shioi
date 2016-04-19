#include "../../I401/PackedStone.hpp"
int main() {
	static_assert((Stone::Black**5_pack).data() == 0b0101010101, "error");
	static_assert((Stone::White | Stone::None**2_pack | Stone::White).data() == 0b10000010, "error");
	static_assert(Stone::Black**4_pack == (Stone::Black | Stone::Black | Stone::Black | Stone::Black), "error");
	static_assert((Stone::Black | Stone::White | Stone::Black | Stone::White).front() == Stone::Black, "error");
	static_assert((Stone::Black | Stone::White | Stone::Black | Stone::White).back() == Stone::White, "error");
	static_assert((Stone::Black | Stone::White | Stone::Black | Stone::White).pop_front().front() == Stone::White, "error");
	static_assert((Stone::Black | Stone::White | Stone::Black | Stone::White).pop_back().back() == Stone::Black, "error");
}
