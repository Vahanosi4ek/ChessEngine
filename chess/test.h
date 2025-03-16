#pragma once

#include "board.h"

#include <iostream>
#include <cassert>

#define TEST(n, c, info) \
    std::cout << "Test #" << n << " - " << info << std::endl; \
    assert(c); \
    std::cout << "Done!" << std::endl;

int perft(Board& board, const int& depth);
int dump_perft(Board& board, const int& depth);
void test_time_movegen();
