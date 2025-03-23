#include "board.h"
#include "test.h"
#include "bitboard.h"
#include "magic.h"

#include <iostream>

int main() {
    init();

    Board board;
    repeated_tests(board, 1000);

    return 0;
}
