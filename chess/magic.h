#pragma once

#include "bitboard.h"

#define ROOK_ATTACKS (64 * 4096)
#define BISHOP_ATTACKS (64 * 512)

struct SMagic {
    Bitboard  mask;
    Bitboard  magic;
    int       shift;
};

Bitboard bmask(Square sq);
Bitboard rmask(Square sq);

Bitboard index_to_occ(int index, Bitboard mask);

extern const Bitboard bishop_magics[64];
extern const Bitboard rook_magics  [64];

void init_magics(bool gen_magics);
void print_magics();
Bitboard fast_bishop_attacks(Square sq, Bitboard occ);
Bitboard fast_rook_attacks(Square sq, Bitboard occ);
