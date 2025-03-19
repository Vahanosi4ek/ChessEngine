#pragma once

#include <cstdint>

enum Square : int {
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    SQ_ALL, NO_SQ,
};

constexpr bool is_ok(Square sq) { return (SQ_A1 <= sq) && (sq <= SQ_H8); }

using Bitboard = uint64_t;

constexpr Bitboard square_to_bb(Square s) { return 1ull << int(s); }

inline Bitboard operator|(Bitboard b, Square s) { return b | square_to_bb(s); }
inline Bitboard operator&(Bitboard b, Square s) { return b & square_to_bb(s); }
inline Bitboard& operator|=(Bitboard& b, Square s) { return b = b | s; }

inline Bitboard operator-(Bitboard b1, Square b2) { return b1 - square_to_bb(b2); }
inline Bitboard& operator-=(Bitboard& b1, Square b2) { return b1 -= square_to_bb(b2); }

inline Bitboard operator|(Square s1, Square s2) { return square_to_bb(s1) | square_to_bb(s2); }

inline Square operator+(Square s, int i) { return Square(int(s) + i); }
inline Square operator-(Square s, int i) { return Square(int(s) - i); }

inline bool get_square(Bitboard board, Square sq) { return board & sq; }
inline void set_square(Bitboard& board, Square sq) { board |= sq; }
inline void clear_square(Bitboard& board, Square sq) { board -= sq; }

constexpr int get_row(Square sq) { return sq / 8; }
constexpr int get_col(Square sq) { return sq % 8; }

inline int lsb(Bitboard b) { return __builtin_ctzll(b); }
inline void pop_lsb(Bitboard& b) { b &= b - 1; }

void print_bb(Bitboard bb);