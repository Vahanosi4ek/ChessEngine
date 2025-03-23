#pragma once

#include "board.h"

#include <iostream>
#include <cassert>

#define TEST(n, c, info, print) \
    if (print) std::cout << "Test #" << n << " - " << info << std::endl; \
    assert(c); \
    if (print) std::cout << "Done!" << std::endl;

int perft(Board& board, const int& depth);
int dump_perft(Board& board, const int& depth);
int test_time_movegen(Board& board, bool print);
int test_startpos_perft(Board& board, bool print);
float repeated_tests(Board& board, int times);
