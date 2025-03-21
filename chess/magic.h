#include "bitboard.h"

struct SMagic {
   Bitboard mask;  // to mask relevant squares of both lines (no outer squares)
   Bitboard magic; // magic 64-bit factor
};

Bitboard bmask(int sq);
Bitboard rmask(int sq);

void init_magics();
Bitboard fast_bishop_attacks(Square sq, Bitboard occ);
Bitboard fast_rook_attacks(Square sq, Bitboard occ);
