#include "bitboard.h"

#include <iostream>

void print_bb(Bitboard bb) {
    std::string int_to_char = " 1234567";
    std::cout << "  +---+---+---+---+---+---+---+---+\n8 ";

    for (int row = 7; row >= 0; row--) {
        for (int col = 0; col <= 7; col++) {
            std::cout << (bb & (1ull << (row * 8 + col)) ? "| X " : "|   ");
        }
        std::cout << "|\n  +---+---+---+---+---+---+---+---+\n" << int_to_char[row] << " ";
    }

    std::cout << "  a   b   c   d   e   f   g   h\n";
}
